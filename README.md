# ESPHome Dometic CFX3 WiFi Integration

- **Customizable Configuration**: Uses ESPHome's YAML configuration for easy customization.
  - ESP32 or ESP8266 microcontroller (e.g., NodeMCU, Wemos D1 Mini).
<<<<<<< HEAD
```yaml
=======
  - Dometic CFX3 fridge/freezer with fridge configured in WiFi mode and connected to same wireless network as ESPHome device (configured via Mobile Cooling App)
- **Software**:
  - [ESPHome](https://esphome.io/) installed and configured.
  - [Home Assistant](https://www.home-assistant.io/) (optional, for integration).
  - A working WiFi network.

>>>>>>> 32532f5 (a)

## Configuration Example

Below is a sample snippet of the new ESPHome YAML configuration (multiple fridges supported):

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
  - source: github://andrewbackway/esphome-dometic-cfx-wifi

dometic_cfx_wifi:
  - id: fridge1
    host: 10.1.0.198

sensor:
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_0_MEASURED_TEMPERATURE
    name: "Fridge 1 Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_0_SET_TEMPERATURE
    name: "Fridge 1 Target Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_1_MEASURED_TEMPERATURE
    name: "Fridge 2 Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_1_SET_TEMPERATURE
    name: "Fridge 2 Target Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: DC_VOLTAGE
    name: "Fridge DC Voltage"
    icon: "mdi:flash"

binary_sensor:
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_0_DOOR_OPEN
    name: "Fridge 1 Door"
    icon: "mdi:door"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_1_DOOR_OPEN
    name: "Fridge 2 Door"
    icon: "mdi:door"

text_sensor:
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: DEVICE_NAME
    name: "Fridge 1 Name"
```

For detailed configuration, refer to the `example.yaml` file in this repository.

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
