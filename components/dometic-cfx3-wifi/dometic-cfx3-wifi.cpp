#include "dometic-cfx3-wifi.h"
#include "esphome.h"
#include "esphome/core/hal.h"
#include "esphome/components/json/json_util.h"  // ArduinoJson wrapper
using esphome::json::build_json;

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/queue.h"
  #include "freertos/semphr.h"
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace esphome {
namespace dometic_cfx {

static const char *const TAG = "dometic-cfx3";

// === Wire protocol opcodes (from exporter) ===
enum DdmActions { PUBLISH=0, SUBSCRIBE=1, PING=2, HELLO=3, ACK=4, NAK=5, NOP=6 };

// --- ddmTopics "PARAM" tuples mirrored from exporter (group,id,domain,ver) ---
struct Topic { int a,b,c,d; const char *name; const char *type; };
static const Topic TOPICS[] = {
  // Subscribe-All
  {1,0,0,129, "SUBSCRIBE_APP_SZ", "EMPTY"},
  {2,0,0,129, "SUBSCRIBE_APP_SZI", "EMPTY"},
  {3,0,0,129, "SUBSCRIBE_APP_DZ", "EMPTY"},

  // Product info
  {0,193,0,0, "PRODUCT_SERIAL_NUMBER", "UTF8_STRING"},

  // Properties
  {0,128,0,1, "COMPARTMENT_COUNT", "INT8_NUMBER"},
  {0,129,0,1, "ICEMAKER_COUNT", "INT8_NUMBER"},

  // Compartment state
  {0,0,1,1, "COMPARTMENT_0_POWER", "INT8_BOOLEAN"},
  {16,0,1,1, "COMPARTMENT_1_POWER", "INT8_BOOLEAN"},
  {0,1,1,1, "COMPARTMENT_0_MEASURED_TEMPERATURE", "INT16_DECIDEGREE_CELSIUS"},
  {16,1,1,1, "COMPARTMENT_1_MEASURED_TEMPERATURE", "INT16_DECIDEGREE_CELSIUS"},
  {0,8,1,1, "COMPARTMENT_0_DOOR_OPEN", "INT8_BOOLEAN"},
  {16,8,1,1, "COMPARTMENT_1_DOOR_OPEN", "INT8_BOOLEAN"},
  {0,2,1,1, "COMPARTMENT_0_SET_TEMPERATURE", "INT16_DECIDEGREE_CELSIUS"},
  {16,2,1,1, "COMPARTMENT_1_SET_TEMPERATURE", "INT16_DECIDEGREE_CELSIUS"},
  {0,129,1,1, "COMPARTMENT_0_RECOMMENDED_RANGE", "INT16_ARRAY"},
  {16,129,1,1, "COMPARTMENT_1_RECOMMENDED_RANGE", "INT16_ARRAY"},
  {0,0,2,1, "PRESENTED_TEMPERATURE_UNIT", "INT8_NUMBER"},
  {0,128,1,1, "COMPARTMENT_0_TEMPERATURE_RANGE", "INT16_ARRAY"},
  {16,128,1,1, "COMPARTMENT_1_TEMPERATURE_RANGE", "INT16_ARRAY"},

  // Power
  {0,0,3,1, "COOLER_POWER", "INT8_BOOLEAN"},
  {0,1,3,1, "BATTERY_VOLTAGE_LEVEL", "INT16_DECICURRENT_VOLT"},
  {0,2,3,1, "BATTERY_PROTECTION_LEVEL", "UINT8_NUMBER"},
  {0,5,3,1, "POWER_SOURCE", "INT8_NUMBER"},
  {0,6,3,1, "ICEMAKER_POWER", "INT8_BOOLEAN"},

  // Errors
  {0,3,4,1, "COMMUNICATION_ALARM", "INT8_BOOLEAN"},
  {0,1,4,1, "NTC_OPEN_LARGE_ERROR", "INT8_BOOLEAN"},
  {0,2,4,1, "NTC_SHORT_LARGE_ERROR", "INT8_BOOLEAN"},
  {0,9,4,1, "SOLENOID_VALVE_ERROR", "INT8_BOOLEAN"},
  {0,17,4,1, "NTC_OPEN_SMALL_ERROR", "INT8_BOOLEAN"},
  {0,18,4,1, "NTC_SHORT_SMALL_ERROR", "INT8_BOOLEAN"},
  {0,50,4,1, "FAN_OVERVOLTAGE_ERROR", "INT8_BOOLEAN"},
  {0,51,4,1, "COMPRESSOR_START_FAIL_ERROR", "INT8_BOOLEAN"},
  {0,52,4,1, "COMPRESSOR_SPEED_ERROR", "INT8_BOOLEAN"},
  {0,53,4,1, "CONTROLLER_OVER_TEMPERATURE", "INT8_BOOLEAN"},

  // Alerts
  {0,3,5,1, "TEMPERATURE_ALERT_DCM", "INT8_BOOLEAN"},
  {0,0,5,1, "TEMPERATURE_ALERT_CC", "INT8_BOOLEAN"},
  {0,1,5,1, "DOOR_ALERT", "INT8_BOOLEAN"},
  {0,2,5,1, "VOLTAGE_ALERT", "INT8_BOOLEAN"},

  // Communication
  {0,0,6,1, "DEVICE_NAME", "UTF8_STRING"},
  {0,1,6,1, "WIFI_MODE", "INT8_BOOLEAN"},
  {0,3,6,1, "BLUETOOTH_MODE", "INT8_BOOLEAN"},
  {0,8,6,1, "WIFI_AP_CONNECTED", "INT8_BOOLEAN"},

  // WiFi settings
  {0,0,7,1, "STATION_SSID_0", "UTF8_STRING"},
  {1,0,7,1, "STATION_SSID_1", "UTF8_STRING"},
  {2,0,7,1, "STATION_SSID_2", "UTF8_STRING"},
  {0,1,7,1, "STATION_PASSWORD_0", "UTF8_STRING"},
  {1,1,7,1, "STATION_PASSWORD_1", "UTF8_STRING"},
  {2,1,7,1, "STATION_PASSWORD_2", "UTF8_STRING"},
  {3,1,7,1, "STATION_PASSWORD_3", "UTF8_STRING"},
  {4,1,7,1, "STATION_PASSWORD_4", "UTF8_STRING"},
  {0,2,7,1, "CFX_DIRECT_PASSWORD_0", "UTF8_STRING"},
  {1,2,7,1, "CFX_DIRECT_PASSWORD_1", "UTF8_STRING"},
  {2,2,7,1, "CFX_DIRECT_PASSWORD_2", "UTF8_STRING"},
  {3,2,7,1, "CFX_DIRECT_PASSWORD_3", "UTF8_STRING"},
  {4,2,7,1, "CFX_DIRECT_PASSWORD_4", "UTF8_STRING"},

  // History
  {0,64,1,1, "COMPARTMENT_0_TEMPERATURE_HISTORY_HOUR", "HISTORY_DATA_ARRAY"},
  {16,64,1,1,"COMPARTMENT_1_TEMPERATURE_HISTORY_HOUR","HISTORY_DATA_ARRAY"},
  {0,65,1,1, "COMPARTMENT_0_TEMPERATURE_HISTORY_DAY", "HISTORY_DATA_ARRAY"},
  {16,65,1,1,"COMPARTMENT_1_TEMPERATURE_HISTORY_DAY", "HISTORY_DATA_ARRAY"},
  {0,66,1,1, "COMPARTMENT_0_TEMPERATURE_HISTORY_WEEK","HISTORY_DATA_ARRAY"},
  {16,66,1,1,"COMPARTMENT_1_TEMPERATURE_HISTORY_WEEK","HISTORY_DATA_ARRAY"},
  {0,64,3,1, "DC_CURRENT_HISTORY_HOUR", "HISTORY_DATA_ARRAY"},
  {0,65,3,1, "DC_CURRENT_HISTORY_DAY",  "HISTORY_DATA_ARRAY"},
  {0,66,3,1, "DC_CURRENT_HISTORY_WEEK", "HISTORY_DATA_ARRAY"},
};

// ---------- NEW: FreeRTOS plumbing (declare if not in header) ----------
#ifndef CFX3_FREERTOS_PLUMBING_DECLARED
#define CFX3_FREERTOS_PLUMBING_DECLARED 1
// Add these to your class in the header if not present:
/// Queue of complete CR-terminated lines (owned strings)
/// Using pointers to avoid copying large frames through the queue.
#endif

// Forward decls for helpers
static bool json_try_get_array(const std::string &line, std::vector<int> &out);

// ===== Component lifecycle =====
void DometicCFXComponent::setup() {
  ESP_LOGI(TAG, "Starting CFX3 component: %s:%u", host_.c_str(), (unsigned)port_);

  if (!this->line_queue_) {
    this->line_queue_ = xQueueCreate(/*len*/ 12, sizeof(std::string*));
  }
  if (!this->send_mutex_) {
    this->send_mutex_ = xSemaphoreCreateMutex();
  }
  if (this->socket_task_handle_ == nullptr) {
    // Use a modest stack; increase if you hit stack overflow.
    xTaskCreatePinnedToCore(
      [](void *param) {
        static_cast<DometicCFXComponent*>(param)->socket_task_();
      },
      "dometic-cfx3-socket",
      8192,
      this,
      4,                          // priority
      &this->socket_task_handle_,
      1                           // core 1 helps keep Wi-Fi on core 0
    );
  }
}

void DometicCFXComponent::loop() {
  // Non-blocking dequeue of any received line(s)
  for (int i = 0; i < 3; ++i) {   // process up to 3 frames per loop to keep fair
    std::string *line_ptr = nullptr;
    if (xQueueReceive(this->line_queue_, &line_ptr, 0) != pdTRUE) break;
    if (line_ptr) {
      this->handle_payload_(*line_ptr);
      delete line_ptr;
      this->last_activity_ms_ = millis();
    }
  }
}

// ===== Socket task (all blocking I/O lives here) =====
void DometicCFXComponent::socket_task_() {
  ESP_LOGI(TAG, "Socket task started");
  this->rxbuf_.clear();

  while (true) {
    if (this->sock_ < 0) {
      if (!this->connect_task_()) {
        ESP_LOGE(TAG, "Connection attempt failed, will retry in 2 seconds");
        // connection attempt failed; wait then retry
        vTaskDelay(pdMS_TO_TICKS(2000));
        continue;
      }
    }

    // Keepalive ping (every 15s)
    if (millis() - this->last_ping_ms_ > 15000 || this->last_ping_ms_ == 0) {
      ESP_LOGI(TAG, "Sending keepalive ping...");
      this->send_ping_(); // thread-safe via send_mutex_
      this->last_ping_ms_ = millis();
    }

    // Read bytes with short timeout and assemble lines
    this->poll_recv_();  // pushes complete lines to queue

    // Idle timeout -> force reconnect (60 secs)
    if (millis() - this->last_activity_ms_ > 60000) {
      ESP_LOGW(TAG, "No activity, reconnecting...");
      this->close_();
      // loop continues and tries reconnect
    }

    vTaskDelay(pdMS_TO_TICKS(30));      // yield
  }
}

// Called only from socket task context
bool DometicCFXComponent::connect_task_() {
  ESP_LOGI(TAG, "Connecting to %s:%u", host_.c_str(), (unsigned)port_);
  this->close_();

  ESP_LOGI(TAG, "Preparing Socket");

  this->sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock_ < 0) {
    ESP_LOGE(TAG, "socket() failed");
    return false;
  }

  // Short recv timeout for responsive task loop
  struct timeval tv;
  tv.tv_sec = 0; tv.tv_usec = 150000; // 150 ms
  setsockopt(this->sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  ESP_LOGI(TAG, "Socket created");

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_);
  if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
    ESP_LOGE(TAG, "Invalid host IP (use dotted-quad)");
    this->close_();
    return false;
  }

  ESP_LOGI(TAG, "Connecting TCP...");
  if (::connect(this->sock_, (sockaddr *) &addr, sizeof(addr)) < 0) {
    ESP_LOGE(TAG, "connect() failed");
    this->close_();
    return false;
  }

  // Protocol handshake: expect NOP, then PING/ACK, then subscribe-all
  // Receive first line (NOP)
  std::string line;
  if (!this->recv_line_once_(line)) { this->close_(); return false; }
  if (!this->handle_payload_inline_(line)) { this->close_(); return false; }

  ESP_LOGI(TAG, "Handshake NOP received: %s", line.c_str());

  // Send PING, wait ACK
  if (!this->send_ping_()) { this->close_(); return false; }
  if (!this->recv_line_once_(line)) { this->close_(); return false; }
  if (!this->handle_payload_inline_(line)) { this->close_(); return false; }

  ESP_LOGI(TAG, "Send PING, wait for ACK completed");
  
  // Subscribe to all
  if (!this->send_subscribe_all_()) { this->close_(); return false; }

  this->last_activity_ms_ = millis();
  ESP_LOGI(TAG, "Connected and subscribed.");

  return true;
}

// Non-blocking poll of socket; assembles CR-terminated frames to queue
void DometicCFXComponent::poll_recv_() {
  if (this->sock_ < 0) return;

  char buf[128];
  // recv() will return -1 with EWOULDBLOCK after ~timeout or 0 on close
  ssize_t n = ::recv(this->sock_, buf, sizeof(buf), 0);
  if (n == 0) {
    ESP_LOGW(TAG, "Peer closed connection");
    this->close_();
    return;
  }
  if (n < 0) {
    // timeout or transient; just return
    return;
  }

  ESP_LOGD(TAG, "Receiving data...");

  // Append to buffer and split on '\r'
  this->rxbuf_.append(buf, buf + n);

  // Extract complete frames
  size_t start = 0;
  while (true) {
    size_t pos = this->rxbuf_.find('\r', start);
    if (pos == std::string::npos) {
      // keep partial data
      // but protect against runaway buffer
      if (this->rxbuf_.size() > 8192) {
        ESP_LOGW(TAG, "RX buffer overflow, resetting");
        this->rxbuf_.clear();
      }
      break;
    }
    // One full line [start..pos-1]
    std::string *line = new std::string(this->rxbuf_.substr(start, pos - start));

    ESP_LOGD(TAG, "Received line: %s", line->c_str());

    if (xQueueSend(this->line_queue_, &line, 0) != pdTRUE) {
      // queue full; drop oldest behavior: discard this line
      ESP_LOGW(TAG, "Line queue full, dropping frame");
      delete line;
    } else {
      // activity noted
      this->last_activity_ms_ = millis();
    }
    start = pos + 1;
  }

  // Keep any tail after the last '\r'
  if (start > 0) {
    this->rxbuf_.erase(0, start);
  }
}

// One-shot blocking read for handshake (task context only)
bool DometicCFXComponent::recv_line_once_(std::string &out) {
  out.clear();
  if (this->sock_ < 0) return false;

  // Use existing SO_RCVTIMEO (~150ms) and loop with finite attempts
  char ch;
  uint32_t deadline = millis() + 2000; // 2s max for a line during handshake
  while (millis() < deadline) {
    ssize_t n = ::recv(this->sock_, &ch, 1, 0);
    if (n == 0) return false;         // closed
    if (n < 0) {                      // timeout
      vTaskDelay(pdMS_TO_TICKS(20));
      continue;
    }
    if (ch == '\r') return true;
    out.push_back(ch);
    if (out.size() > 4096) { ESP_LOGW(TAG, "Frame too large, dropping"); return false; }
  }
  return false;
}

// Handle payload used during handshake (socket task context)
bool DometicCFXComponent::handle_payload_inline_(const std::string &line) {
  // We reuse the same parser; it may publish entities.
  // During handshake we don't want to ACK back from loop; handle_payload_() will send ACK itself.
  return this->handle_payload_(line);
}

// ===== Socket utils =====
void DometicCFXComponent::close_() {
  if (this->sock_ >= 0) {
    ESP_LOGI(TAG, "Closing connection");
    ::close(this->sock_);
    this->sock_ = -1;
  }
  this->rxbuf_.clear();
}

bool DometicCFXComponent::send_json_(const std::string &json) {
  ESP_LOGD(TAG, "Sending JSON: %s", json.c_str());

  if (this->sock_ < 0) return false;

  std::string framed = json + "\r\r";

  ESP_LOGD(TAG, "Sending JSON (2): %s", framed.c_str());

  bool ok = false;
  if (this->send_mutex_ && xSemaphoreTake(this->send_mutex_, pdMS_TO_TICKS(200)) == pdTRUE) {
    ssize_t n = ::send(this->sock_, framed.data(), framed.size(), 0);
    ok = (n == (ssize_t)framed.size());
    xSemaphoreGive(this->send_mutex_);
  } else {
    // As a fallback try unsynchronized (should be rare)
    ESP_LOGW(TAG, "send_mutex_ not available, trying unsynchronized send");
    ssize_t n = ::send(this->sock_, framed.data(), framed.size(), 0);
    ok = (n == (ssize_t)framed.size());
  }
  if (!ok) ESP_LOGW(TAG, "send() short or failed");
  return ok;
}

bool DometicCFXComponent::send_ack_() {
  ESP_LOGD(TAG, "Sending ACK");
  std::string payload = esphome::json::build_json([&](JsonObject root) {
    JsonArray arr = root["ddmp"].to<JsonArray>();
    arr.add((int) ACK);
  });
  return this->send_json_(payload);
}

bool DometicCFXComponent::send_ping_() {
  ESP_LOGD(TAG, "Sending PING");
  std::string payload = esphome::json::build_json([&](JsonObject root) {
    JsonArray arr = root["ddmp"].to<JsonArray>();
    arr.add((int) PING);
  });
  return this->send_json_(payload);
}

bool DometicCFXComponent::send_subscribe_all_() {
  ESP_LOGI(TAG, "Sending SUBSCRIBE commands");
  for (const Topic &t : TOPICS) {
    // only subscribe to topics that are relevant
    if ( std::string(t.name) == "SUBSCRIBE_APP_DZ" ||
          //std::string(t.name) == "BATTERY_VOLTAGE_LEVEL" ||
          std::string(t.name) == "PRODUCT_SERIAL_NUMBER" ) { //||
          //std::string(t.name) == "COMPARTMENT_0_MEASURED_TEMPERATURE" ||
          //std::string(t.name) == "COMPARTMENT_1_MEASURED_TEMPERATURE" ||
          //std::string(t.name) == "COMPARTMENT_0_DOOR_OPEN" ||
          //std::string(t.name) == "COMPARTMENT_1_DOOR_OPEN" ||
          //std::string(t.name) == "COMPARTMENT_0_SET_TEMPERATURE" ||
          //std::string(t.name) == "COMPARTMENT_1_SET_TEMPERATURE" ||
          //std::string(t.name) == "DC_CURRENT_HISTORY_HOUR" ) {

        ESP_LOGI(TAG, "Sending SUBSCRIBE for topic: %s", t.name);
        
        std::string payload = esphome::json::build_json([&](JsonObject root) {
          JsonArray arr = root["ddmp"].to<JsonArray>();
          arr.add((int) SUBSCRIBE);
          arr.add(t.a);
          arr.add(t.b);
          arr.add(t.c);
          arr.add(t.d);
        });
      if (!this->send_json_(payload)) return false;
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
  return true;
}

// ===== Parsing & publishing =====

static bool json_try_get_array(const std::string &line, std::vector<int> &out) {
  JsonDocument doc;  // ArduinoJson v7
  auto err = deserializeJson(doc, line);
  if (err) return false;

  JsonArray arr = doc["ddmp"].as<JsonArray>();
  if (arr.isNull()) return false;

  out.clear();
  out.reserve(arr.size());
  for (JsonVariant v : arr) out.push_back((int) v.as<long>());
  return true;
}

float DometicCFXComponent::int16_deci_to_float_(int b0, int b1) {
  int16_t raw = (int16_t)((b1 << 8) | (b0 & 0xFF));
  return ((float) raw) / 10.0f;
}

std::string DometicCFXComponent::to_json_array_(const std::vector<int> &vals) {
  std::string s = "[";
  for (size_t i=0;i<vals.size();++i) {
    s += std::to_string(vals[i]);
    if (i+1<vals.size()) s += ",";
  }
  s += "]";
  return s;
}

void DometicCFXComponent::publish_bool_(binary_sensor::BinarySensor *b, bool v) {
  ESP_LOGD(TAG, "Publishing bool: %s = %s", b ? b->get_name().c_str() : "(null)", v ? "true" : "false");
  if (b) b->publish_state(v);
}
void DometicCFXComponent::publish_float_(sensor::Sensor *s, float v) {
  ESP_LOGD(TAG, "Publishing float: %s = %f", s ? s->get_name().c_str() : "(null)", v);
  if (s) s->publish_state(v);
}
void DometicCFXComponent::publish_text_(text_sensor::TextSensor *t, const std::string &v) {
  ESP_LOGD(TAG, "Publishing text: %s = %s", t ? t->get_name().c_str() : "(null)", v.c_str());
  if (t) t->publish_state(v);
}

bool DometicCFXComponent::handle_payload_(const std::string &line) {
  ESP_LOGV(TAG, "Received payload: %s", line.c_str());
  std::vector<int> arr;
  if (!json_try_get_array(line, arr) || arr.empty()) {
    ESP_LOGW(TAG, "Invalid frame");
    return false;
  }

  int code = arr[0];
  if (code == NOP) {
    ESP_LOGD(TAG, "Received NOP");
    return true;
  }
  if (code == ACK) {
    ESP_LOGD(TAG, "Received ACK");
    return true;
  }
  if (code == PUBLISH) {
    ESP_LOGD(TAG, "Received Publish");
    if (arr.size() < 7) return true;

    int a=arr[1], b=arr[2], c=arr[3], d=arr[4];
    int value_type = arr[5]; (void)value_type;

    std::vector<int> val;
    for (size_t i=6; i<arr.size(); ++i) val.push_back(arr[i] & 0xFF);

    auto topic_is = [&](int ta,int tb,int tc,int td){ return ta==a && tb==b && tc==c && td==d; };
    auto decode_bool = [&]()->bool { return !val.empty() && (val[0] != 0); };
    auto decode_int8  = [&]()->float { return val.empty()?NAN:(float)(int8_t)val[0]; };
    auto decode_uint8 = [&]()->float { return val.empty()?NAN:(float)(uint8_t)val[0]; };
    auto decode_temp  = [&]()->float {
      if (val.size()<2) return NAN;
      return int16_deci_to_float_(val[0], val[1]);
    };
    auto decode_deci  = decode_temp;

    auto decode_history = [&]()->std::pair<float,std::string> {
      if (val.size() < 15) return {NAN, ""};
      float latest = int16_deci_to_float_(val[0], val[1]);
      std::vector<int> as_ints(val.begin(), val.end());
      return {latest, to_json_array_(as_ints)};
    };

    // === Map to published entities (unchanged logic) ===
    if (topic_is(0,1,1,1)) publish_float_(comp0_temp, decode_temp());
    else if (topic_is(16,1,1,1)) publish_float_(comp1_temp, decode_temp());
    else if (topic_is(0,2,1,1)) publish_float_(comp0_set_temp, decode_temp());
    else if (topic_is(16,2,1,1)) publish_float_(comp1_set_temp, decode_temp());
    else if (topic_is(0,8,1,1)) {
      bool cur = decode_bool();
      publish_bool_(comp0_door_open, cur);
      if (cur && !comp0_door_prev_ && comp0_open_count) comp0_open_count->publish_state((comp0_open_count->state) + 1.0f);
      comp0_door_prev_ = cur;
    } else if (topic_is(16,8,1,1)) {
      bool cur = decode_bool();
      publish_bool_(comp1_door_open, cur);
      if (cur && !comp1_door_prev_ && comp1_open_count) comp1_open_count->publish_state((comp1_open_count->state) + 1.0f);
      comp1_door_prev_ = cur;
    }
    else if (topic_is(0,0,3,1)) publish_bool_(cooler_power, decode_bool());
    else if (topic_is(0,1,3,1)) publish_float_(dc_voltage, decode_deci());
    else if (topic_is(0,2,3,1)) publish_float_(battery_protection_level, decode_uint8());
    else if (topic_is(0,5,3,1)) publish_float_(power_source, decode_int8());
    else if (topic_is(0,6,3,1)) publish_bool_(icemaker_power, decode_bool());
    else if (topic_is(0,0,1,1)) publish_bool_(comp0_power, decode_bool());
    else if (topic_is(16,0,1,1)) publish_bool_(comp1_power, decode_bool());
    else if (topic_is(0,128,0,1)) publish_float_(compartment_count, decode_uint8());
    else if (topic_is(0,129,0,1)) publish_float_(icemaker_count, decode_uint8());
    else if (topic_is(0,0,2,1))   publish_float_(presented_temp_unit, decode_uint8());
    else if (topic_is(0,3,4,1)) publish_bool_(err_comm_alarm, decode_bool());
    else if (topic_is(0,1,4,1)) publish_bool_(err_ntc_open_large, decode_bool());
    else if (topic_is(0,2,4,1)) publish_bool_(err_ntc_short_large, decode_bool());
    else if (topic_is(0,9,4,1)) publish_bool_(err_solenoid_valve, decode_bool());
    else if (topic_is(0,17,4,1)) publish_bool_(err_ntc_open_small, decode_bool());
    else if (topic_is(0,18,4,1)) publish_bool_(err_ntc_short_small, decode_bool());
    else if (topic_is(0,50,4,1)) publish_bool_(err_fan_overvoltage, decode_bool());
    else if (topic_is(0,51,4,1)) publish_bool_(err_compressor_start_fail, decode_bool());
    else if (topic_is(0,52,4,1)) publish_bool_(err_compressor_speed, decode_bool());
    else if (topic_is(0,53,4,1)) publish_bool_(err_controller_overtemp, decode_bool());
    else if (topic_is(0,3,5,1)) publish_bool_(alert_temp_dcm, decode_bool());
    else if (topic_is(0,0,5,1)) publish_bool_(alert_temp_cc, decode_bool());
    else if (topic_is(0,1,5,1)) publish_bool_(alert_door, decode_bool());
    else if (topic_is(0,2,5,1)) publish_bool_(alert_voltage, decode_bool());
    else if (topic_is(0,1,6,1)) publish_bool_(wifi_mode, decode_bool());
    else if (topic_is(0,3,6,1)) publish_bool_(bluetooth_mode, decode_bool());
    else if (topic_is(0,8,6,1)) publish_bool_(wifi_ap_connected, decode_bool());
    else if (topic_is(0,193,0,0)) publish_text_(product_serial, std::string(val.begin(), val.end()));
    else if (topic_is(0,0,6,1))   publish_text_(device_name, std::string(val.begin(), val.end()));
    else if (topic_is(0,129,1,1)) publish_text_(comp0_recommended_range, to_json_array_(val));
    else if (topic_is(16,129,1,1)) publish_text_(comp1_recommended_range, to_json_array_(val));
    else if (topic_is(0,128,1,1)) publish_text_(comp0_temp_range, to_json_array_(val));
    else if (topic_is(16,128,1,1)) publish_text_(comp1_temp_range, to_json_array_(val));
    else if (topic_is(0,0,7,1)) publish_text_(station_ssid_0, std::string(val.begin(), val.end()));
    else if (topic_is(1,0,7,1)) publish_text_(station_ssid_1, std::string(val.begin(), val.end()));
    else if (topic_is(2,0,7,1)) publish_text_(station_ssid_2, std::string(val.begin(), val.end()));
    else if (topic_is(0,1,7,1)) publish_text_(station_password_0, std::string(val.begin(), val.end()));
    else if (topic_is(1,1,7,1)) publish_text_(station_password_1, std::string(val.begin(), val.end()));
    else if (topic_is(2,1,7,1)) publish_text_(station_password_2, std::string(val.begin(), val.end()));
    else if (topic_is(3,1,7,1)) publish_text_(station_password_3, std::string(val.begin(), val.end()));
    else if (topic_is(4,1,7,1)) publish_text_(station_password_4, std::string(val.begin(), val.end()));
    else if (topic_is(0,2,7,1)) publish_text_(cfx_direct_password_0, std::string(val.begin(), val.end()));
    else if (topic_is(1,2,7,1)) publish_text_(cfx_direct_password_1, std::string(val.begin(), val.end()));
    else if (topic_is(2,2,7,1)) publish_text_(cfx_direct_password_2, std::string(val.begin(), val.end()));
    else if (topic_is(3,2,7,1)) publish_text_(cfx_direct_password_3, std::string(val.begin(), val.end()));
    else if (topic_is(4,2,7,1)) publish_text_(cfx_direct_password_4, std::string(val.begin(), val.end()));
    else if (topic_is(0,64,1,1)) { auto p=decode_history(); publish_float_(comp0_hist_hour_latest,p.first); publish_text_(comp0_hist_hour_json,p.second); }
    else if (topic_is(16,64,1,1)) { auto p=decode_history(); publish_float_(comp1_hist_hour_latest,p.first); publish_text_(comp1_hist_hour_json,p.second); }
    else if (topic_is(0,65,1,1)) { auto p=decode_history(); publish_text_(comp0_hist_day_json,p.second); }
    else if (topic_is(16,65,1,1)) { auto p=decode_history(); publish_text_(comp1_hist_day_json,p.second); }
    else if (topic_is(0,66,1,1)) { auto p=decode_history(); publish_text_(comp0_hist_week_json,p.second); }
    else if (topic_is(16,66,1,1)) { auto p=decode_history(); publish_text_(comp1_hist_week_json,p.second); }
    else if (topic_is(0,64,3,1)) { auto p=decode_history(); publish_float_(dc_current_hist_hour_latest,p.first); publish_text_(dc_current_hist_hour_json,p.second); }
    else if (topic_is(0,65,3,1)) { auto p=decode_history(); publish_text_(dc_current_hist_day_json,p.second); }
    else if (topic_is(0,66,3,1)) { auto p=decode_history(); publish_text_(dc_current_hist_week_json,p.second); }

    // ACK all publishes to match app behavior
    this->send_ack_();
    return true;
  }

  return true;
}

}  // namespace dometic_cfx
}  // namespace esphome
