
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
  this->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
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
  const uint8_t request[] = {
    0x44, 0x44, 0x4D, 0x50,  // DDMP
    0x00, 0x00, 0x00, 0x01,  // Message ID
    0x00, 0x00, 0x00, 0x01,  // Request type
    0x00, 0x00, 0x00, 0x00   // Payload length
  };

  int sent = send(this->socket_, request, sizeof(request), 0);
  if (sent < 0) {
    ESP_LOGE(TAG, "Failed to send request");
    close(this->socket_);
    this->socket_ = -1;
    return;
  }

  uint8_t buffer[512];
  int received = recv(this->socket_, buffer, sizeof(buffer), 0);
  if (received <= 0) {
    ESP_LOGE(TAG, "Failed to receive response");
    close(this->socket_);
    this->socket_ = -1;
    return;
  }

  std::vector<uint8_t> data(buffer, buffer + received);
  this->parse_response_(data);
}

void DometicCFXComponent::parse_response_(const std::vector<uint8_t> &data) {
  if (data.size() < 20) {
    ESP_LOGW(TAG, "Response too short");
    return;
  }

  // Example parsing logic based on known offsets
  float compartment_temp = static_cast<int16_t>((data[20] << 8) | data[21]) / 10.0f;
  float ambient_temp = static_cast<int16_t>((data[22] << 8) | data[23]) / 10.0f;
  float voltage = static_cast<int16_t>((data[24] << 8) | data[25]) / 100.0f;
  bool door_open = data[26] != 0;

  ESP_LOGI(TAG, "Compartment Temp: %.1f °C", compartment_temp);
  ESP_LOGI(TAG, "Ambient Temp: %.1f °C", ambient_temp);
  ESP_LOGI(TAG, "Voltage: %.2f V", voltage);
  ESP_LOGI(TAG, "Door Status: %s", door_open ? "Open" : "Closed");

  /*
  if (this->compartment_temp_ != nullptr)
    this->compartment_temp_->publish_state(compartment_temp);
  #if (this->ambient_temp_ != nullptr)
  #  this->ambient_temp_->publish_state(ambient_temp);
  if (this->voltage_ != nullptr)
    this->voltage_->publish_state(voltage);
  if (this->door_status_ != nullptr)
    this->door_status_->publish_state(door_open ? 1.0f : 0.0f);
  */
}

}  // namespace dometic_cfx
}  // namespace esphome
