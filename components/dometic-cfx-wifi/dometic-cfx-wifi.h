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
  static const uint32_t RETRY_DELAY_MS = 5000;  // 5 seconds

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

  void send_action(int action);
  void send_sub(const uint8_t param[4]);
  void subscribe_topics();
  void process_publish(JsonArray& arr);
  void process_message();

 public:
  void set_host(std::string host) { host_ = host; }
  void set_port(int port) { port_ = port; }
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