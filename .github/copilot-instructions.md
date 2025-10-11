# Copilot Instructions for AI Agents

## Project Overview
- This project provides an ESPHome external component for integrating Dometic CFX3 WiFi-enabled fridge/freezers with ESPHome and Home Assistant.
- The main logic is implemented as a C++ ESPHome component in `components/dometic-cfx3-wifi/`.
- Configuration is managed via YAML (see `example.yaml`), with the component loaded as an external component in ESPHome.

## Key Files & Structure
- `components/dometic-cfx3-wifi/dometic-cfx3-wifi.cpp` / `.h`: Main C++ implementation of the ESPHome component.
- `components/dometic-cfx3-wifi/__init__.py`: Python stub for ESPHome external component registration.
- `example.yaml`: Example ESPHome configuration for users.
- `README.md`: High-level usage, requirements, and configuration guidance.

## Architecture & Data Flow
- The ESP device (ESP32/ESP8266) runs ESPHome firmware, loading this custom component.
- The component communicates with the Dometic CFX3 fridge over WiFi (using the fridge's IP address).
- Data (temperature, voltage, door state, etc.) is polled from the fridge and exposed as ESPHome sensors.
- Home Assistant (or other ESPHome-compatible platforms) can consume these sensors for automation and monitoring.

## Developer Workflows
- **Build/Flash**: Use ESPHome CLI or dashboard to build and upload firmware. Example: `esphome run example.yaml`.
- **Debugging**: Use `esphome logs <device>` to view runtime logs. Check for connection issues or sensor errors.
- **Component Development**: Edit C++ files in `components/dometic-cfx3-wifi/`. Rebuild and flash to test changes.
- **Configuration Testing**: Modify `example.yaml` to test new features or options.

## Project-Specific Conventions
- Component is named `dometic-cfx3-wifi` in YAML and code.
- All configuration options are nested under the `dometic-cfx3-wifi:` YAML key.
- Sensor names and icons are customizable via YAML.
- The fridge's IP address must be specified as `host:` in the YAML config.
- Follows ESPHome's external component structure (see [ESPHome docs](https://esphome.io/components/external_components.html)).

## Integration Points
- Integrates with ESPHome as an external component (see `external_components:` in YAML).
- Communicates with Dometic CFX3 fridge via WiFi (HTTP or proprietary protocol, see C++ code for details).
- Exposes sensors to Home Assistant via ESPHome API.

## Examples
- See `example.yaml` for a full working configuration.
- Example YAML snippet:
  ```yaml
  dometic-cfx3-wifi:
    host: 192.168.1.100
    comp0_temp:
      name: "Compartment 0 Temperature"
  ```

## References
- [ESPHome External Components](https://esphome.io/components/external_components.html)
- [Dometic CFX3 API Reference](https://github.com/DanielLeone/dometic-cfx-exporter/)

---
If any project-specific conventions or workflows are unclear, please ask for clarification or check the latest `README.md` and `example.yaml`.
