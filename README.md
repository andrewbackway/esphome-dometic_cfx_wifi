# ESPHome Dometic CFX3 WiFi Integration

- **Customizable Configuration**: Uses ESPHome's YAML configuration for easy customization.
  - ESP32 or ESP8266 microcontroller (e.g., NodeMCU, Wemos D1 Mini).
```yaml

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

    name: "Compartment 1 Temperature"
  ssid: !secret wifi_ssid
  password: !secret wifi_password

external_components:
  - source: github://andrewbackway/esphome-dometic-cfx3-wifi

    icon: "mdi:thermometer"
  - id: fridge1
    host: 10.1.0.55
  - id: fridge2
    host: 10.1.0.56

  dc_voltage:
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_0_MEASURED_TEMPERATURE
    name: "Fridge 1 Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge2
    type: COMPARTMENT_0_MEASURED_TEMPERATURE
    name: "Fridge 2 Temp"
    icon: "mdi:thermometer"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: DC_VOLTAGE
    name: "Fridge 1 DC Voltage"
    icon: "mdi:flash"

    name: "DC Voltage"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: COMPARTMENT_0_DOOR_OPEN
    name: "Fridge 1 Door"
    icon: "mdi:door"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge2
    type: COMPARTMENT_0_DOOR_OPEN
    name: "Fridge 2 Door"
    icon: "mdi:door"

text_sensor:
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge1
    type: PRODUCT_SERIAL_NUMBER
    name: "Fridge 1 Serial"
  - platform: dometic_cfx_wifi
    dometic_cfx_wifi_id: fridge2
    type: PRODUCT_SERIAL_NUMBER
    name: "Fridge 2 Serial"
```

For detailed configuration, refer to the `example.yaml` file in this repository.
    icon: "mdi:flash"
  comp0_door_open:
    name: "Compartment 0 Door"
    icon: "mdi:door"
  comp1_door_open:
    name: "Compartment 1 Door"
    icon: "mdi:door"
  device_name:
    name: "Device Name"
  product_serial:
    name: "Product Serial"
  comp0_power:
    name: "Compresser 1 Power"
  comp1_power:
    name: "Compresser 2 Power"
  cooler_power:
    name: "Power"
```

For detailed configuration, refer to the `example.yaml` file in this repository.

## Usage

- Once the ESP device is running, it will connect to the configured WiFi network and communicate with the Dometic CFX3.
- Use Home Assistant or another MQTT-compatible platform to monitor temperature, power status, door state.
- Check the ESPHome logs for debugging or connection issues.

## Troubleshooting

- **Connection Issues**: Ensure the ESP device is properly connected to the Dometic CFX3 and that the WiFi credentials are correct.
- **Home Assistant Not Detecting**: Verify that the ESPHome integration is enabled and that the API is correctly configured in the YAML file.
- **Firmware Upload Fails**: Check the ESP board type and USB connection. Use the ESPHome logs for detailed error messages.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes. Ensure that your code follows the ESPHome coding standards and includes appropriate documentation.

---

*Built based on https://github.com/DanielLeone/dometic-cfx-exporter/, and ported to ESPHome with Chat GPT 5, Grok, and a substantial level of troubleshooting for the Dometic CFX3 community.*