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
  // Dynamic lookup helpers
  sensor::Sensor *get_sensor(const std::string &topic);
  binary_sensor::BinarySensor *get_binary_sensor(const std::string &topic);
  text_sensor::TextSensor *get_text_sensor(const std::string &topic);
  void set_host(const std::string &host) { host_ = host; }
  void set_port(uint16_t port) { port_ = port; }

  // Dynamic registration for new YAML config style
  void register_sensor(const std::string &topic, sensor::Sensor *sensor) {
    sensors_[topic] = sensor;
  }
  void register_binary_sensor(const std::string &topic, binary_sensor::BinarySensor *sensor) {
    binary_sensors_[topic] = sensor;
  }
  void register_text_sensor(const std::string &topic, text_sensor::TextSensor *sensor) {
    text_sensors_[topic] = sensor;
  }

  // Component
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  std::string host_;
  uint16_t port_{13142};

  // Dynamic sensor maps
  std::map<std::string, sensor::Sensor *> sensors_;
  std::map<std::string, binary_sensor::BinarySensor *> binary_sensors_;
  std::map<std::string, text_sensor::TextSensor *> text_sensors_;

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

}  // namespace cfx
}  // namespace esphome
