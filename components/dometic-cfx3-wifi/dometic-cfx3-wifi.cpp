#include "dometic-cfx3-wifi.h"

const char* DometicCfxWifi::DDMP_KEY = "ddmp";
const char* DometicCfxWifi::CARRIAGE_RETURN = "\r";
const char* DometicCfxWifi::LOG_TAG = "dometic_cfx_wifi";

void DometicCfxWifi::send_action(int action) {
  DynamicJsonDocument doc(256);
  JsonArray arr = doc.createNestedArray(DDMP_KEY);
  arr.add(action);
  String json;
  serializeJson(doc, json);
  client_.print(json);
  client_.print(CARRIAGE_RETURN);
}

void DometicCfxWifi::send_sub(const uint8_t param[4]) {
  DynamicJsonDocument doc(256);
  JsonArray arr = doc.createNestedArray(DDMP_KEY);
  arr.add(1);  // SUBSCRIBE
  for (int i = 0; i < 4; i++) arr.add(param[i]);
  String json;
  serializeJson(doc, json);
  client_.print(json);
  client_.print(CARRIAGE_RETURN);
}

void DometicCfxWifi::subscribe_topics() {
  uint8_t params[][4] = {
    {3, 0, 0, 129},  // SUBSCRIBE_APP_DZ
    {0, 1, 3, 1},    // BATTERY_VOLTAGE_LEVEL
    {0, 193, 0, 0},  // PRODUCT_SERIAL_NUMBER
    {0, 1, 1, 1},    // COMPARTMENT_0_MEASURED_TEMPERATURE
    {16, 1, 1, 1},   // COMPARTMENT_1_MEASURED_TEMPERATURE
    {0, 8, 1, 1},    // COMPARTMENT_0_DOOR_OPEN
    {16, 8, 1, 1},   // COMPARTMENT_1_DOOR_OPEN
    {0, 2, 1, 1},    // COMPARTMENT_0_SET_TEMPERATURE
    {16, 2, 1, 1},   // COMPARTMENT_1_SET_TEMPERATURE
    {0, 64, 3, 1}    // DC_CURRENT_HISTORY_HOUR
  };
  for (size_t i = 0; i < sizeof(params) / sizeof(params[0]); i++) {
    send_sub(params[i]);
  }
}

void DometicCfxWifi::process_publish(JsonArray& arr) {
  if (arr.size() < 5) return;
  uint8_t topic[4] = {arr[1].as<uint8_t>(), arr[2].as<uint8_t>(), arr[3].as<uint8_t>(), arr[4].as<uint8_t>()};
  std::vector<uint8_t> value;
  for (size_t i = 5; i < arr.size(); i++) {
    value.push_back(arr[i].as<uint8_t>());
  }

  if (topic[0] == 0 && topic[1] == 1 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_0_MEASURED_TEMPERATURE
    if (value.size() >= 2) {
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;  // Handle negative
      float temp = raw / 10.0f;
      compartment_0_temp_ = temp;
      id(sensor_comp0_temp).publish_state(temp);
    }
  } else if (topic[0] == 16 && topic[1] == 1 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_1_MEASURED_TEMPERATURE
    if (value.size() >= 2) {
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;
      float temp = raw / 10.0f;
      compartment_1_temp_ = temp;
      id(sensor_comp1_temp).publish_state(temp);
    }
  } else if (topic[0] == 0 && topic[1] == 2 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_0_SET_TEMPERATURE
    if (value.size() >= 2) {
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;
      float temp = raw / 10.0f;
      compartment_0_set_temp_ = temp;
      id(sensor_comp0_set_temp).publish_state(temp);
    }
  } else if (topic[0] == 16 && topic[1] == 2 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_1_SET_TEMPERATURE
    if (value.size() >= 2) {
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;
      float temp = raw / 10.0f;
      compartment_1_set_temp_ = temp;
      id(sensor_comp1_set_temp).publish_state(temp);
    }
  } else if (topic[0] == 0 && topic[1] == 8 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_0_DOOR_OPEN
    if (value.size() >= 1) {
      bool is_open = value[0] != 0;
      if (is_open && !compartment_0_open_previous_) {
        compartment_0_open_count_++;
        id(sensor_comp0_open_count).publish_state(compartment_0_open_count_);
      }
      compartment_0_open_previous_ = is_open;
      compartment_0_open_ = is_open;
      id(binary_comp0_open).publish_state(is_open);
    }
  } else if (topic[0] == 16 && topic[1] == 8 && topic[2] == 1 && topic[3] == 1) {  // COMPARTMENT_1_DOOR_OPEN
    if (value.size() >= 1) {
      bool is_open = value[0] != 0;
      if (is_open && !compartment_1_open_previous_) {
        compartment_1_open_count_++;
        id(sensor_comp1_open_count).publish_state(compartment_1_open_count_);
      }
      compartment_1_open_previous_ = is_open;
      compartment_1_open_ = is_open;
      id(binary_comp1_open).publish_state(is_open);
    }
  } else if (topic[0] == 0 && topic[1] == 1 && topic[2] == 3 && topic[3] == 1) {  // BATTERY_VOLTAGE_LEVEL
    if (value.size() >= 2) {
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;
      float volt = raw / 10.0f;
      dc_voltage_ = volt;
      id(sensor_dc_voltage).publish_state(volt);
    }
  } else if (topic[0] == 0 && topic[1] == 64 && topic[2] == 3 && topic[3] == 1) {  // DC_CURRENT_HISTORY_HOUR
    if (value.size() >= 2) {  // At least most recent
      int16_t raw = (int16_t)((value[1] << 8) | value[0]);
      if (value[1] >= 0x80) raw -= 65536;
      float curr = raw / 10.0f;
      dc_current_ = curr;
      id(sensor_dc_current).publish_state(curr);
    }
  } else if (topic[0] == 0 && topic[1] == 193 && topic[2] == 0 && topic[3] == 0) {  // PRODUCT_SERIAL_NUMBER
    std::string serial;
    for (auto b : value) serial += static_cast<char>(b);
    product_serial_ = serial;
    id(text_serial).publish_state(serial);
  }
}

void DometicCfxWifi::process_message() {
  if (!buffer_.empty() && buffer_.back() == '\r') buffer_.pop_back();
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, buffer_);
  if (error) {
    ESP_LOGW(LOG_TAG, "JSON parse failed: %s", error.c_str());
    return;
  }
  if (!doc.containsKey(DDMP_KEY) || !doc[DDMP_KEY].is<JsonArray>()) {
    ESP_LOGW(LOG_TAG, "Invalid DDMP");
    return;
  }
  JsonArray arr = doc[DDMP_KEY];
  if (arr.size() < 1) return;
  int code = arr[0].as<int>();

  if (state_ == WAIT_NOP) {
    if (code == 6) {  // NOP
      send_action(2);  // PING
      state_ = SENT_PING;
      ESP_LOGD(LOG_TAG, "Got NOP, sent PING");
    } else {
      ESP_LOGW(LOG_TAG, "Expected NOP, got %d", code);
      client_.stop();
      state_ = DISCONNECTED;
    }
    return;
  }
  if (state_ == SENT_PING) {
    if (code == 4) {  // ACK
      subscribe_topics();
      state_ = SUBSCRIBED;
      ESP_LOGD(LOG_TAG, "Got ACK, subscribed");
    } else {
      ESP_LOGW(LOG_TAG, "Expected ACK, got %d", code);
      client_.stop();
      state_ = DISCONNECTED;
    }
    return;
  }
  if (state_ == SUBSCRIBED) {
    if (code == 2) {  // PING
      send_action(4);  // ACK
    } else if (code == 0) {  // PUBLISH
      send_action(4);  // ACK
      process_publish(arr);
    } else {
      ESP_LOGW(LOG_TAG, "Unexpected code %d", code);
    }
  }
}

void DometicCfxWifi::setup() {
  // Initializations if needed
}

void DometicCfxWifi::loop() {
  unsigned long now = millis();
  if (state_ != DISCONNECTED && (now - connected_at_ > RECONNECT_INTERVAL_MS)) {
    ESP_LOGI(LOG_TAG, "Reconnecting after 1 hour");
    client_.stop();
    state_ = DISCONNECTED;
  }
  if (state_ == DISCONNECTED) {
    if (client_.connect(host_.c_str(), port_)) {
      connected_at_ = now;
      state_ = WAIT_NOP;
      ESP_LOGD(LOG_TAG, "Connected, awaiting NOP");
    } else {
      ESP_LOGW(LOG_TAG, "Connection failed");
      delay(RETRY_DELAY_MS);  // Wait before retry
    }
    return;
  }
  while (client_.available()) {
    char c = client_.read();
    buffer_ += c;
    if (c == '\r') {
      process_message();
      buffer_.clear();
    }
  }
  if (!client_.connected()) {
    ESP_LOGW(LOG_TAG, "Disconnected unexpectedly");
    state_ = DISCONNECTED;
  }
}