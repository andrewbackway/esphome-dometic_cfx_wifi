
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include <string>

namespace esphome {
namespace dometic_cfx {

class DometicCFXComponent : public Component {
 public:
  void set_host(const std::string &host) { host_ = host; }
  void set_port(uint16_t port) { port_ = port; }

  /*
  void set_compartment_temp_sensor(sensor::Sensor *sensor) { compartment_temp_ = sensor; }
  void set_ambient_temp_sensor(sensor::Sensor *sensor) { ambient_temp_ = sensor; }
  void set_voltage_sensor(sensor::Sensor *sensor) { voltage_ = sensor; }
  void set_door_status_sensor(sensor::Sensor *sensor) { door_status_ = sensor; }
  */
  void setup() override;
  void loop() override;

 protected:
  std::string host_;
  uint16_t port_;
  int socket_ = -1;

  void connect_();
  void request_data_();
  void parse_response_(const std::vector<uint8_t> &data);

  /*
  sensor::Sensor *compartment_temp_ = nullptr;
  sensor::Sensor *ambient_temp_ = nullptr;
  sensor::Sensor *voltage_ = nullptr;
  sensor::Sensor *door_status_ = nullptr;
*/