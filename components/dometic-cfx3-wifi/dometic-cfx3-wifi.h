#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include <lwip/sockets.h>
#include <string>

namespace esphome {
namespace cfx3 {

class Cfx3Component : public Component {
 public:
  // User config
  std::string host_;
  int port_;

  // Sensors
  // TOOD UPDATE WITH ACTUAL LIST
  
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_host(const std::string &host) { host_ = host; }
  void set_port(int port) { port_ = port; }

 protected:
  int sockfd_ = -1;
  uint64_t last_connect_attempt_ = 0;
  uint64_t reconnect_delay_ms_ = 1000; // Start at 1 sec

  std::string rx_buffer_;

  bool connect_socket_();
  void close_socket_();
  void handle_incoming_data_();
  void parse_and_publish_(const std::string &json_str);
};

}  // namespace cfx3
}  // namespace esphome
