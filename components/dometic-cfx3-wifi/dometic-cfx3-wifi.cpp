
#include "dometic-cfx3-wifi.h"
#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <cstring>
#include <vector>
#include <lwip/sockets.h>

namespace esphome {
namespace dometic_cfx {

static const char *TAG = "dometic-cfx3";

void DometicCFXComponent::setup() {
  ESP_LOGI(TAG, "Setting up Dometic CFX3 WiFi component");
  this->connect_();
}

void DometicCFXComponent::loop() {
  static uint32_t last_poll = 0;
  const uint32_t interval = 60000;  // Poll every 60 seconds

  if (millis() - last_poll > interval) {
    last_poll = millis();
    if (this->socket_ < 0) {
      ESP_LOGW(TAG, "Socket not connected, attempting reconnect");
      this->connect_();
    }
    if (this->socket_ >= 0) {
      this->request_data_();
    }
  }
}

void DometicCFXComponent::connect_() {
  ESP_LOGI(TAG, "Connecting to %s:%d", this->host_.c_str(), this->port_);

  struct sockaddr_in server_addr;
  this->socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (this->socket_ < 0) {
    ESP_LOGE(TAG, "Failed to create socket");
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(this->port_);
  inet_pton(AF_INET, this->host_.c_str(), &server_addr.sin_addr);

  if (connect(this->socket_, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    ESP_LOGE(TAG, "Connection failed");
    close(this->socket_);
    this->socket_ = -1;
    return;
  }

  ESP_LOGI(TAG, "Connected successfully");
}

void DometicCFXComponent::request_data_() {

}

void DometicCFXComponent::parse_response_(const std::vector<uint8_t> &data) {
  if (data.size() < 20) {
    ESP_LOGW(TAG, "Response too short");
    return;
  }

}  // namespace dometic_cfx
}  // namespace esphome
