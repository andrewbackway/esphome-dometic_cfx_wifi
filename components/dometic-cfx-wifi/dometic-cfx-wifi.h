#ifndef DOMETIC_CFX_WIFI_H
#define DOMETIC_CFX_WIFI_H

#include "esphome.h"
#include <WiFiClient.h>
#include <ArduinoJson.h>

class DometicCfxWifi : public Component {
 private:
  static const char* DDMP_KEY;
  static const char* CARRIAGE_RETURN;
  static const char* LOG_TAG;
  static const uint32_t RECONNECT_INTERVAL_MS = 3600000UL;  // 1 hour
  static const uint32_t RETRY_DELAY_MS = 5000;              // 5 seconds

  std::string host_;
  int port_;
  WiFiClient client_;
  unsigned long connected_at_ = 0;
  std::string buffer_;
  enum State { DISCONNECTED, WAIT_NOP, SENT_PING, SUBSCRIBED };
  State state_ = DISCONNECTED;

  float compartment_0_temp_ = NAN;
  float compartment_1_temp_ = NAN;
  float compartment_0_set_temp_ = NAN;
  float compartment_1_set_temp_ = NAN;
  float dc_voltage_ = NAN;
  float dc_current_ = NAN;
  bool compartment_0_open_ = false;
  bool compartment_0_open_previous_ = false;
  int compartment_0_open_count_ = 0;
  bool compartment_1_open_ = false;
  bool compartment_1_open_previous_ = false;
  int compartment_1_open_count_ = 0;
  std::string product_serial_ = "";

  // Sensor pointers
  sensor::Sensor *sensor_comp0_temp_{nullptr};
  sensor::Sensor *sensor_comp1_temp_{nullptr};
  sensor::Sensor *sensor_comp0_set_temp_{nullptr};
  sensor::Sensor *sensor_comp1_set_temp_{nullptr};
  sensor::Sensor *sensor_comp0_open_count_{nullptr};
  sensor::Sensor *sensor_comp1_open_count_{nullptr};
  binary_sensor::BinarySensor *binary_comp0_open_{nullptr};
  binary_sensor::BinarySensor *binary_comp1_open_{nullptr};
  sensor::Sensor *sensor_dc_voltage_{nullptr};
  sensor::Sensor *sensor_dc_current_{nullptr};
  text_sensor::TextSensor *text_serial_{nullptr};

  void send_action(int action);
  void send_sub(const uint8_t param[4]);
  void subscribe_topics();
  void process_publish(JsonArray &arr);
  void process_message();

 public:
  void set_host(std::string host) { host_ = host; }
  void set_port(int port) { port_ = port; }

  // Setters for sensors (called from __init__.py)
  void set_sensor_comp0_temp(sensor::Sensor *s) { sensor_comp0_temp_ = s; }
  void set_sensor_comp1_temp(sensor::Sensor *s) { sensor_comp1_temp_ = s; }
  void set_sensor_comp0_set_temp(sensor::Sensor *s) { sensor_comp0_set_temp_ = s; }
  void set_sensor_comp1_set_temp(sensor::Sensor *s) { sensor_comp1_set_temp_ = s; }
  void set_sensor_comp0_open_count(sensor::Sensor *s) { sensor_comp0_open_count_ = s; }
  void set_sensor_comp1_open_count(sensor::Sensor *s) { sensor_comp1_open_count_ = s; }
  void set_binary_comp0_open(binary_sensor::BinarySensor *b) { binary_comp0_open_ = b; }
  void set_binary_comp1_open(binary_sensor::BinarySensor *b) { binary_comp1_open_ = b; }
  void set_sensor_dc_voltage(sensor::Sensor *s) { sensor_dc_voltage_ = s; }
  void set_sensor_dc_current(sensor::Sensor *s) { sensor_dc_current_ = s; }
  void set_text_serial(text_sensor::TextSensor *t) { text_serial_ = t; }

  // Getters
  float get_compartment_0_temp() { return compartment_0_temp_; }
  float get_compartment_1_temp() { return compartment_1_temp_; }
  float get_compartment_0_set_temp() { return compartment_0_set_temp_; }
  float get_compartment_1_set_temp() { return compartment_1_set_temp_; }
  float get_dc_voltage() { return dc_voltage_; }
  float get_dc_current() { return dc_current_; }
  bool get_compartment_0_open() { return compartment_0_open_; }
  bool get_compartment_1_open() { return compartment_1_open_; }
  int get_compartment_0_open_count() { return compartment_0_open_count_; }
  int get_compartment_1_open_count() { return compartment_1_open_count_; }
  std::string get_product_serial() { return product_serial_; }

  void setup() override;
  void loop() override;
};

#endif  // DOMETIC_CFX_WIFI_H
