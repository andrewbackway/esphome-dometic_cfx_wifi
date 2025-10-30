#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <vector>
#include <string>

// ESP-IDF / lwIP
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace esphome {
namespace dometic_cfx {

class DometicCFXComponent : public Component {
 public:
  void set_host(const std::string &host) { host_ = host; }
  void set_port(uint16_t port) { port_ = port; }

  // Float sensor setters
  void set_comp0_temp(sensor::Sensor *sensor);
  void set_comp1_temp(sensor::Sensor *sensor);
  void set_comp0_set_temp(sensor::Sensor *sensor);
  void set_comp1_set_temp(sensor::Sensor *sensor);
  void set_dc_voltage(sensor::Sensor *sensor);
  void set_battery_protection_level(sensor::Sensor *sensor);
  void set_power_source(sensor::Sensor *sensor);
  void set_compartment_count(sensor::Sensor *sensor);
  void set_icemaker_count(sensor::Sensor *sensor);
  void set_comp0_hist_hour_latest(sensor::Sensor *sensor);
  void set_comp1_hist_hour_latest(sensor::Sensor *sensor);
  void set_comp0_hist_day_latest(sensor::Sensor *sensor);
  void set_comp1_hist_day_latest(sensor::Sensor *sensor);
  void set_comp0_hist_week_latest(sensor::Sensor *sensor);
  void set_comp1_hist_week_latest(sensor::Sensor *sensor);
  void set_dc_current_hist_hour_latest(sensor::Sensor *sensor);
  void set_comp0_open_count(sensor::Sensor *sensor);
  void set_comp1_open_count(sensor::Sensor *sensor);
  void set_presented_temp_unit(sensor::Sensor *sensor);

  // Binary sensor setters
  void set_cooler_power(binary_sensor::BinarySensor *sensor);
  void set_comp0_power(binary_sensor::BinarySensor *sensor);
  void set_comp1_power(binary_sensor::BinarySensor *sensor);
  void set_comp0_door_open(binary_sensor::BinarySensor *sensor);
  void set_comp1_door_open(binary_sensor::BinarySensor *sensor);
  void set_icemaker_power(binary_sensor::BinarySensor *sensor);
  void set_wifi_mode(binary_sensor::BinarySensor *sensor);
  void set_bluetooth_mode(binary_sensor::BinarySensor *sensor);
  void set_wifi_ap_connected(binary_sensor::BinarySensor *sensor);
  void set_err_comm_alarm(binary_sensor::BinarySensor *sensor);
  void set_err_ntc_open_large(binary_sensor::BinarySensor *sensor);
  void set_err_ntc_short_large(binary_sensor::BinarySensor *sensor);
  void set_err_solenoid_valve(binary_sensor::BinarySensor *sensor);
  void set_err_ntc_open_small(binary_sensor::BinarySensor *sensor);
  void set_err_ntc_short_small(binary_sensor::BinarySensor *sensor);
  void set_err_fan_overvoltage(binary_sensor::BinarySensor *sensor);
  void set_err_compressor_start_fail(binary_sensor::BinarySensor *sensor);
  void set_err_compressor_speed(binary_sensor::BinarySensor *sensor);
  void set_err_controller_overtemp(binary_sensor::BinarySensor *sensor);
  void set_alert_temp_dcm(binary_sensor::BinarySensor *sensor);
  void set_alert_temp_cc(binary_sensor::BinarySensor *sensor);
  void set_alert_door(binary_sensor::BinarySensor *sensor);
  void set_alert_voltage(binary_sensor::BinarySensor *sensor);

  // Text sensor setters
  void set_product_serial(text_sensor::TextSensor *sensor);
  void set_device_name(text_sensor::TextSensor *sensor);
  void set_comp0_recommended_range(text_sensor::TextSensor *sensor);
  void set_comp1_recommended_range(text_sensor::TextSensor *sensor);
  void set_comp0_temp_range(text_sensor::TextSensor *sensor);
  void set_comp1_temp_range(text_sensor::TextSensor *sensor);
  void set_comp0_hist_hour_json(text_sensor::TextSensor *sensor);
  void set_comp1_hist_hour_json(text_sensor::TextSensor *sensor);
  void set_comp0_hist_day_json(text_sensor::TextSensor *sensor);
  void set_comp1_hist_day_json(text_sensor::TextSensor *sensor);
  void set_comp0_hist_week_json(text_sensor::TextSensor *sensor);
  void set_comp1_hist_week_json(text_sensor::TextSensor *sensor);
  void set_dc_current_hist_hour_json(text_sensor::TextSensor *sensor);
  void set_dc_current_hist_day_json(text_sensor::TextSensor *sensor);
  void set_dc_current_hist_week_json(text_sensor::TextSensor *sensor);
  void set_station_ssid_0(text_sensor::TextSensor *sensor);
  void set_station_ssid_1(text_sensor::TextSensor *sensor);
  void set_station_ssid_2(text_sensor::TextSensor *sensor);
  void set_station_password_0(text_sensor::TextSensor *sensor);
  void set_station_password_1(text_sensor::TextSensor *sensor);
  void set_station_password_2(text_sensor::TextSensor *sensor);
  void set_station_password_3(text_sensor::TextSensor *sensor);
  void set_station_password_4(text_sensor::TextSensor *sensor);
  void set_cfx_direct_password_0(text_sensor::TextSensor *sensor);
  void set_cfx_direct_password_1(text_sensor::TextSensor *sensor);
  void set_cfx_direct_password_2(text_sensor::TextSensor *sensor);
  void set_cfx_direct_password_3(text_sensor::TextSensor *sensor);
  void set_cfx_direct_password_4(text_sensor::TextSensor *sensor);


  // Component
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  std::string host_;
  uint16_t port_{13142};

  void set_all_sensors_undefined_();
  
  // === Sensors (floats) ===
  // Compartment temps & setpoints
  sensor::Sensor *comp0_temp{nullptr};
  sensor::Sensor *comp1_temp{nullptr};
  sensor::Sensor *comp0_set_temp{nullptr};
  sensor::Sensor *comp1_set_temp{nullptr};

  // Power & electrical
  sensor::Sensor *dc_voltage{nullptr};
  sensor::Sensor *battery_protection_level{nullptr}; // raw uint8
  sensor::Sensor *power_source{nullptr};             // raw int8 mapping unknown
  sensor::Sensor *compartment_count{nullptr};
  sensor::Sensor *icemaker_count{nullptr};

  // History (latest entries)
  sensor::Sensor *comp0_hist_hour_latest{nullptr};
  sensor::Sensor *comp1_hist_hour_latest{nullptr};
  sensor::Sensor *comp0_hist_day_latest{nullptr};
  sensor::Sensor *comp1_hist_day_latest{nullptr};
  sensor::Sensor *comp0_hist_week_latest{nullptr};
  sensor::Sensor *comp1_hist_week_latest{nullptr};
  sensor::Sensor *dc_current_hist_hour_latest{nullptr};

  // Open-counts (to mirror exporter’s counters)
  sensor::Sensor *comp0_open_count{nullptr};
  sensor::Sensor *comp1_open_count{nullptr};

  // Presented temperature unit (raw)
  sensor::Sensor *presented_temp_unit{nullptr};

  // === Binary sensors ===
  binary_sensor::BinarySensor *cooler_power{nullptr};
  binary_sensor::BinarySensor *comp0_power{nullptr};
  binary_sensor::BinarySensor *comp1_power{nullptr};
  binary_sensor::BinarySensor *comp0_door_open{nullptr};
  binary_sensor::BinarySensor *comp1_door_open{nullptr};
  binary_sensor::BinarySensor *icemaker_power{nullptr};
  binary_sensor::BinarySensor *wifi_mode{nullptr};
  binary_sensor::BinarySensor *bluetooth_mode{nullptr};
  binary_sensor::BinarySensor *wifi_ap_connected{nullptr};

  // Errors (binary)
  binary_sensor::BinarySensor *err_comm_alarm{nullptr};
  binary_sensor::BinarySensor *err_ntc_open_large{nullptr};
  binary_sensor::BinarySensor *err_ntc_short_large{nullptr};
  binary_sensor::BinarySensor *err_solenoid_valve{nullptr};
  binary_sensor::BinarySensor *err_ntc_open_small{nullptr};
  binary_sensor::BinarySensor *err_ntc_short_small{nullptr};
  binary_sensor::BinarySensor *err_fan_overvoltage{nullptr};
  binary_sensor::BinarySensor *err_compressor_start_fail{nullptr};
  binary_sensor::BinarySensor *err_compressor_speed{nullptr};
  binary_sensor::BinarySensor *err_controller_overtemp{nullptr};

  // Alerts (binary)
  binary_sensor::BinarySensor *alert_temp_dcm{nullptr};
  binary_sensor::BinarySensor *alert_temp_cc{nullptr};
  binary_sensor::BinarySensor *alert_door{nullptr};
  binary_sensor::BinarySensor *alert_voltage{nullptr};

  // === Text sensors ===
  text_sensor::TextSensor *product_serial{nullptr};
  text_sensor::TextSensor *device_name{nullptr};
  // Arrays/strings surfaced as JSON/strings
  text_sensor::TextSensor *comp0_recommended_range{nullptr};
  text_sensor::TextSensor *comp1_recommended_range{nullptr};
  text_sensor::TextSensor *comp0_temp_range{nullptr};
  text_sensor::TextSensor *comp1_temp_range{nullptr};
  text_sensor::TextSensor *comp0_hist_hour_json{nullptr};
  text_sensor::TextSensor *comp1_hist_hour_json{nullptr};
  text_sensor::TextSensor *comp0_hist_day_json{nullptr};
  text_sensor::TextSensor *comp1_hist_day_json{nullptr};
  text_sensor::TextSensor *comp0_hist_week_json{nullptr};
  text_sensor::TextSensor *comp1_hist_week_json{nullptr};
  text_sensor::TextSensor *dc_current_hist_hour_json{nullptr};
  text_sensor::TextSensor *dc_current_hist_day_json{nullptr};
  text_sensor::TextSensor *dc_current_hist_week_json{nullptr};

  // WiFi settings (strings — warning: may be sensitive on some setups)
  text_sensor::TextSensor *station_ssid_0{nullptr};
  text_sensor::TextSensor *station_ssid_1{nullptr};
  text_sensor::TextSensor *station_ssid_2{nullptr};
  text_sensor::TextSensor *station_password_0{nullptr};
  text_sensor::TextSensor *station_password_1{nullptr};
  text_sensor::TextSensor *station_password_2{nullptr};
  text_sensor::TextSensor *station_password_3{nullptr};
  text_sensor::TextSensor *station_password_4{nullptr};
  text_sensor::TextSensor *cfx_direct_password_0{nullptr};
  text_sensor::TextSensor *cfx_direct_password_1{nullptr};
  text_sensor::TextSensor *cfx_direct_password_2{nullptr};
  text_sensor::TextSensor *cfx_direct_password_3{nullptr};
  text_sensor::TextSensor *cfx_direct_password_4{nullptr};
  
  int sock_{-1};
  uint32_t last_activity_ms_{0};
  uint32_t last_ping_ms_{0};


  // FreeRTOS resources
  TaskHandle_t socket_task_handle_ = nullptr;
  QueueHandle_t line_queue_ = nullptr;
  SemaphoreHandle_t send_mutex_ = nullptr;

  // RX line assembler buffer (task context)
  std::string rxbuf_;

  // Task + helpers
  void socket_task_();
  bool connect_task_();
  void poll_recv_();
  bool recv_line_once_(std::string &out);
  bool handle_payload_inline_(const std::string &line);

  bool comp0_door_prev_{false};
  bool comp1_door_prev_{false};

  bool connect_();
  void close_();
  bool send_json_(const std::string &json);
  bool send_ack_();
  bool send_ping_();
  bool send_subscribe_all_();

  // ? bool recv_line_(std::string &out); // reads up to '\r'
  bool handle_payload_(const std::string &line);

  // decoding helpers
  static float int16_deci_to_float_(int b0, int b1);
  static std::string to_json_array_(const std::vector<int> &vals);

  // publish helpers
  void publish_bool_(binary_sensor::BinarySensor *b, bool v);
  void publish_float_(sensor::Sensor *s, float v);
  void publish_text_(text_sensor::TextSensor *t, const std::string &v);
};

}  // namespace cfx3
}  // namespace esphome