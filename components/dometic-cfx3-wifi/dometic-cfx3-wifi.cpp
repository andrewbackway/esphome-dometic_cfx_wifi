#include "dometic-cfx3-wifi.h"
#include "esphome.h"
#include "esphome/core/hal.h"
#include "esphome/components/json/json_util.h"  // ArduinoJson wrapper
using esphome::json::build_json;

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
  {0,17,4,1, "NTC_OPEN_SMALL_ERROR", "INT8_BOOLEAN"},      // notes in exporter
  {0,18,4,1, "NTC_SHORT_SMALL_ERROR", "INT8_BOOLEAN"},     // notes in exporter
  {0,50,4,1, "FAN_OVERVOLTAGE_ERROR", "INT8_BOOLEAN"},     // notes in exporter
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

void DometicCFXComponent::setup() {
  ESP_LOGI(TAG, "Starting CFX3 component: %s:%u", host_.c_str(), (unsigned)port_);
}

void DometicCFXComponent::loop() {
  const uint32_t now = millis();

  if (sock_ < 0) {
    if (!this->connect_()) {
      delay(2000);
      return;
    }
  }

  // Periodic keepalive ping
  if (now - last_ping_ms_ > 15000) {
    this->send_ping_();
    last_ping_ms_ = now;
  }

  // Read any incoming line (CR-terminated)
  std::string line;
  if (this->recv_line_(line)) {
    this->handle_payload_(line);
    last_activity_ms_ = now;
  }

  // If idle for a long time, reconnect
  if (now - last_activity_ms_ > 60000) {
    ESP_LOGW(TAG, "No activity, reconnecting …");
    this->close_();
  }
}

bool DometicCFXComponent::connect_() {
  ESP_LOGI(TAG, "Connecting to %s:%u", host_.c_str(), (unsigned)port_);
  this->close_();

  sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    ESP_LOGE(TAG, "socket() failed");
    return false;
  }
  // 30s timeout like exporter
  struct timeval tv;
  tv.tv_sec = 30; tv.tv_usec = 0;
  setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_);
  if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
    ESP_LOGE(TAG, "Invalid host IP (use dotted-quad)");
    this->close_();
    return false;
  }
  ESP_LOGI(TAG, "Connecting TCP …");
  if (::connect(sock_, (sockaddr *) &addr, sizeof(addr)) < 0) {
    ESP_LOGE(TAG, "connect() failed");
    this->close_();
    return false;
  }

  // Expect NOP first
  std::string line;
  if (!this->recv_line_(line)) { this->close_(); return false; }
  bool ok = this->handle_payload_(line); // should be NOP
  if (!ok) { this->close_(); return false; }

  // Send PING, wait for ACK
  if (!this->send_ping_()) { this->close_(); return false; }
  if (!this->recv_line_(line)) { this->close_(); return false; }
  ok = this->handle_payload_(line); // should include ACK
  if (!ok) { this->close_(); return false; }

  // Subscribe to all streams (SZ, SZI, DZ) like the exporter
  if (!this->send_subscribe_all_()) { this->close_(); return false; }

  last_activity_ms_ = millis();
  ESP_LOGI(TAG, "Connected & subscribed.");
  return true;
}

void DometicCFXComponent::close_() {
  ESP_LOGI(TAG, "Closing connection");
  if (sock_ >= 0) {
    ::close(sock_);
    sock_ = -1;
  }
}

bool DometicCFXComponent::send_json_(const std::string &json) {
  if (sock_ < 0) return false;
  std::string framed = json + "\r";
  ssize_t n = ::send(sock_, framed.data(), framed.size(), 0);
  return n == (ssize_t)framed.size();
}

bool DometicCFXComponent::send_ack_() {
  ESP_LOGI(TAG, "Sending ACK");
  std::string payload = esphome::json::build_json([&](JsonObject root) {
    JsonArray arr = root["ddmp"].to<JsonArray>();
    arr.add((int) ACK);
  });
  return this->send_json_(payload);
}

bool DometicCFXComponent::send_ping_() {
  ESP_LOGI(TAG, "Sending PING");
  std::string payload = esphome::json::build_json([&](JsonObject root) {
    JsonArray arr = root["ddmp"].to<JsonArray>();
    arr.add((int) PING);
  });
  return this->send_json_(payload);
}

// Assumes you iterate a list of special SUBSCRIBE topics (e.g., SUBSCRIBE_APP_SZ/SZI/DZ)
// and/or your TOPICS table. Keep your existing outer loop; just replace the JSON build.
bool DometicCFXComponent::send_subscribe_all_() {
  ESP_LOGI(TAG, "Sending SUBSCRIBE to all");
  for (const Topic &t : TOPICS) {
    if (std::string(t.name) == "SUBSCRIBE_APP_SZ" ||
        std::string(t.name) == "SUBSCRIBE_APP_SZI" ||
        std::string(t.name) == "SUBSCRIBE_APP_DZ") {
      std::string payload = esphome::json::build_json([&](JsonObject root) {
        JsonArray arr = root["ddmp"].to<JsonArray>();
        arr.add((int) SUBSCRIBE);
        arr.add(t.a);
        arr.add(t.b);
        arr.add(t.c);
        arr.add(t.d);
      });
      if (!this->send_json_(payload)) return false;
      delay(20);
    }
  }
  return true;
}


bool DometicCFXComponent::recv_line_(std::string &out) {
  out.clear();
  if (sock_ < 0) return false;

  char ch;
  while (true) {
    ssize_t n = ::recv(sock_, &ch, 1, 0);
    if (n <= 0) return false;
    if (ch == '\r') break;
    out.push_back(ch);
    // protect against very long/invalid frames
    if (out.size() > 4096) { ESP_LOGW(TAG, "Frame too large, dropping"); return false; }
  }
  return true;
}

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
  // Same as exporter: signed 16-bit big-endian fixed with /10
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
  if (b) b->publish_state(v);
}

void DometicCFXComponent::publish_float_(sensor::Sensor *s, float v) {
  if (s) s->publish_state(v);
}

void DometicCFXComponent::publish_text_(text_sensor::TextSensor *t, const std::string &v) {
  if (t) t->publish_state(v);
}

bool DometicCFXComponent::handle_payload_(const std::string &line) {
  ESP_LOGI(TAG, "Received payload: %s", line.c_str());
  std::vector<int> arr;
  if (!json_try_get_array(line, arr) || arr.empty()) {
    ESP_LOGW(TAG, "Invalid frame");
    return false;
  }

  int code = arr[0];
  // ddmp frame format variants:
  //  - NOP/ACK/…: [code]
  //  - SUBSCRIBE ack: [ACK, ...]
  //  - PUBLISH: [PUBLISH, a,b,c,d, valueType, <value bytes...>]
  if (code == NOP) {
    ESP_LOGD(TAG, "NOP");
    // reply? The exporter doesn't ACK NOP; keep going
    return true;
  }
  if (code == ACK) {
    ESP_LOGD(TAG, "ACK");
    return true;
  }
  if (code == PUBLISH) {
    if (arr.size() < 7) return true;

    // Extract topic tuple and type
    int a=arr[1], b=arr[2], c=arr[3], d=arr[4];
    // valueType as enum mapped like exporter strings; we parse int index by matching
    // The exporter uses string names; DDMP here sends a numeric type id.
    // We'll decode based on lengths/patterns like the Python did.

    int value_type = arr[5];

    // Remaining are "value bytes" (unsigned 8-bit ints)
    std::vector<int> val;
    for (size_t i=6; i<arr.size(); ++i) val.push_back(arr[i] & 0xFF);

    // Helper to match topic
    auto topic_is = [&](int ta,int tb,int tc,int td){ return ta==a && tb==b && tc==c && td==d; };

    auto decode_bool = [&]()->bool { return !val.empty() && (val[0] != 0); };
    auto decode_int8  = [&]()->float { return val.empty()?NAN:(float)(int8_t)val[0]; };
    auto decode_uint8 = [&]()->float { return val.empty()?NAN:(float)(uint8_t)val[0]; };
    auto decode_temp  = [&]()->float {
      if (val.size()<2) return NAN;
      return int16_deci_to_float_(val[0], val[1]);
    };
    auto decode_deci  = decode_temp;

    // History array = 7 temps (t1..t7) + timestamp byte (per exporter comments)
    auto decode_history = [&]()->std::pair<float,std::string> {
      if (val.size() < 15) return {NAN, ""};
      float latest = int16_deci_to_float_(val[0], val[1]);
      std::vector<int> as_ints(val.begin(), val.end());
      return {latest, to_json_array_(as_ints)};
    };

    // === Map to published entities (cover everything the exporter emits) ===

    // Temps & setpoints
    if (topic_is(0,1,1,1)) publish_float_(comp0_temp, decode_temp());
    else if (topic_is(16,1,1,1)) publish_float_(comp1_temp, decode_temp());
    else if (topic_is(0,2,1,1)) publish_float_(comp0_set_temp, decode_temp());
    else if (topic_is(16,2,1,1)) publish_float_(comp1_set_temp, decode_temp());

    // Door state + rising-edge open counters (to mirror Prometheus counters)
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

    // Power & electrical
    else if (topic_is(0,0,3,1)) publish_bool_(cooler_power, decode_bool());
    else if (topic_is(0,1,3,1)) publish_float_(dc_voltage, decode_deci());
    else if (topic_is(0,2,3,1)) publish_float_(battery_protection_level, decode_uint8());
    else if (topic_is(0,5,3,1)) publish_float_(power_source, decode_int8());
    else if (topic_is(0,6,3,1)) publish_bool_(icemaker_power, decode_bool());

    // Compartment power toggles
    else if (topic_is(0,0,1,1)) publish_bool_(comp0_power, decode_bool());
    else if (topic_is(16,0,1,1)) publish_bool_(comp1_power, decode_bool());

    // Counts / properties / units
    else if (topic_is(0,128,0,1)) publish_float_(compartment_count, decode_uint8());
    else if (topic_is(0,129,0,1)) publish_float_(icemaker_count, decode_uint8());
    else if (topic_is(0,0,2,1))   publish_float_(presented_temp_unit, decode_uint8());

    // Errors
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

    // Alerts
    else if (topic_is(0,3,5,1)) publish_bool_(alert_temp_dcm, decode_bool());
    else if (topic_is(0,0,5,1)) publish_bool_(alert_temp_cc, decode_bool());
    else if (topic_is(0,1,5,1)) publish_bool_(alert_door, decode_bool());
    else if (topic_is(0,2,5,1)) publish_bool_(alert_voltage, decode_bool());

    // Communication
    else if (topic_is(0,1,6,1)) publish_bool_(wifi_mode, decode_bool());
    else if (topic_is(0,3,6,1)) publish_bool_(bluetooth_mode, decode_bool());
    else if (topic_is(0,8,6,1)) publish_bool_(wifi_ap_connected, decode_bool());

    // Strings
    else if (topic_is(0,193,0,0)) publish_text_(product_serial, std::string(val.begin(), val.end()));
    else if (topic_is(0,0,6,1))   publish_text_(device_name, std::string(val.begin(), val.end()));

    // Ranges/arrays → JSON text
    else if (topic_is(0,129,1,1)) publish_text_(comp0_recommended_range, to_json_array_(val));
    else if (topic_is(16,129,1,1)) publish_text_(comp1_recommended_range, to_json_array_(val));
    else if (topic_is(0,128,1,1)) publish_text_(comp0_temp_range, to_json_array_(val));
    else if (topic_is(16,128,1,1)) publish_text_(comp1_temp_range, to_json_array_(val));

    // WiFi settings strings (note: bytes are UTF-8 chars)
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

    // History arrays: publish JSON + latest sample as a numeric sensor
    else if (topic_is(0,64,1,1)) { auto p=decode_history(); publish_float_(comp0_hist_hour_latest,p.first); publish_text_(comp0_hist_hour_json,p.second); }
    else if (topic_is(16,64,1,1)) { auto p=decode_history(); publish_float_(comp1_hist_hour_latest,p.first); publish_text_(comp1_hist_hour_json,p.second); }
    else if (topic_is(0,65,1,1)) { auto p=decode_history(); publish_text_(comp0_hist_day_json,p.second); }
    else if (topic_is(16,65,1,1)) { auto p=decode_history(); publish_text_(comp1_hist_day_json,p.second); }
    else if (topic_is(0,66,1,1)) { auto p=decode_history(); publish_text_(comp0_hist_week_json,p.second); }
    else if (topic_is(16,66,1,1)) { auto p=decode_history(); publish_text_(comp1_hist_week_json,p.second); }
    else if (topic_is(0,64,3,1)) { auto p=decode_history(); publish_float_(dc_current_hist_hour_latest,p.first); publish_text_(dc_current_hist_hour_json,p.second); }
    else if (topic_is(0,65,3,1)) { auto p=decode_history(); publish_text_(dc_current_hist_day_json,p.second); }
    else if (topic_is(0,66,3,1)) { auto p=decode_history(); publish_text_(dc_current_hist_week_json,p.second); }

    // SUBSCRIBE_* and others don’t carry values we publish; ignore.

    // For safety: ACK all publishes to match app behavior
    this->send_ack_();
    return true;
  }

  // default
  return true;
}

}  // namespace cfx3
}  // namespace esphome
