# ESPHome Dometic CFX3 WiFi Integration

## Overview

This repository contains an ESPHome configuration for integrating Dometic CFX3 portable fridge/freezers with WiFi functionality. It enables remote monitoring of the fridge using Home Assistant or other platforms compatible with ESPHome.

## Features

- **WiFi Connectivity**: Connects the Dometic CFX3 to a WiFi network for remote access.
- **Temperature Monitoring**: Reads and reports the internal temperature of the fridge.
- **Home Assistant Integration**: Seamlessly integrates with Home Assistant for automation and monitoring.
- **Customizable Configuration**: Uses ESPHome's YAML configuration for easy customization.

## Requirements

- **Hardware**:
  - ESP32 or ESP8266 microcontroller (e.g., NodeMCU, Wemos D1 Mini).
  - Dometic CFX3 fridge/freezer with fridge configured in WiFi mode and connected to same wireless network as ESPHome device (configured via Mobile Cooling App)
- **Software**:
  - [ESPHome](https://esphome.io/) installed and configured.
  - [Home Assistant](https://www.home-assistant.io/) (optional, for integration).
  - A working WiFi network.


## Configuration Example

Below is a sample snippet of the ESPHome YAML configuration:

```yaml
esphome:
  name: dometic-cfx3
  friendly_name: ESPHome Dometic CFX3

esp32:
  board: esp32dev
  framework:
    type: esp-idf

wifi:
  ssid: !secret wifi_ssid 
  password: !secret wifi_password

external_components:
 - source: github://andrewbackway/esphome-dometic_cfx3_wifi

web_server:
  version: 3

api:

dometic-cfx3-wifi:
  host: <<IP OF FRIDGE>>
   cooler_power:
    name: "CFX Power"
  device_name:
    name: "CFX Device Name"
  dc_voltage:
    name: "CFX DC Voltage"
    icon: "mdi:flash"
  comp0_temp:
    name: "CFX Compartment 1 Temp"
    icon: "mdi:thermometer"
  comp1_temp:
    name: "CFX Compartment 2 Temp"
    icon: "mdi:thermometer"
  comp0_set_temp:
    name: "CFX Compartment 1 Target Temp"
    icon: "mdi:thermometer"
  comp1_set_temp:
    name: "CFX Compartment 2 Target Temp"
    icon: "mdi:thermometer"
  comp0_door_open:
    name: "CFX Compartment 1 Door"
    icon: "mdi:door"
  comp1_door_open:
    name: "CFX Compartment 2 Door"
    icon: "mdi:door"
  comp0_power:
    name: "CFX Compartment 1 Power"
  comp1_power:
    name: "CFX Compartment 2 Power"
```

For detailed configuration, refer to the `example.yaml` file in this repository.

## Usage
- Ensure you have the Dometic CFX3 connected to the same WiFi network as your ESP device (you can configure wifi via the Dometic Mobile Cooling App).
- Once the ESP device is running, it will connect to the configured WiFi network and communicate with the Dometic CFX3.
- Use Home Assistant or another MQTT-compatible platform to monitor temperature, power status, door state.
- Check the ESPHome logs for debugging or connection issues.

## Troubleshooting

- **Connection Issues**: Ensure the ESP device is properly connected to the same WiFi network as the Dometic CFX3 and that the WiFi credentials are correct.
- **Home Assistant Not Detecting**: Verify that the ESPHome integration is enabled and that the API is correctly configured in the YAML file.
- **Firmware Upload Fails**: Check the ESP board type and USB connection. Use the ESPHome logs for detailed error messages.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes. Ensure that your code follows the ESPHome coding standards and includes appropriate documentation.

---

*Built based on https://github.com/DanielLeone/dometic-cfx-exporter/, and ported to ESPHome with Chat GPT 5, Grok, and a substantial level of troubleshooting for the Dometic CFX3 community.*