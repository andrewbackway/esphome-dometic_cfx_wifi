import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_ICON,
    UNIT_CELSIUS,
    UNIT_VOLT,
    UNIT_AMPERE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_DOOR,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_CONNECTIVITY,
    STATE_CLASS_MEASUREMENT,
)

DEPENDENCIES = ["network", "json", "sensor", "binary_sensor", "text_sensor"]
AUTO_LOAD = ["network", "json", "sensor", "binary_sensor", "text_sensor"]

# Use underscores (C++ identifiers can't contain '-')
dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic_cfx")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometicCFXComponent", cg.Component)

# Sensor schemas using ESPHome's built-in helpers (handles icon/unit/device_class/etc. properly)
TEMP_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_CELSIUS,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_TEMPERATURE,
    state_class=STATE_CLASS_MEASUREMENT,
)

VOLT_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_VOLT,
    accuracy_decimals=2,
    device_class=DEVICE_CLASS_VOLTAGE,
    state_class=STATE_CLASS_MEASUREMENT,
)

GENERIC_SENSOR_SCHEMA = sensor.sensor_schema(
    accuracy_decimals=0,
)

# Binary sensor schemas
POWER_BINARY_SENSOR_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_POWER,
)

DOOR_BINARY_SENSOR_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_DOOR,
)

PROBLEM_BINARY_SENSOR_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_PROBLEM,
)

CONNECTIVITY_BINARY_SENSOR_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_CONNECTIVITY,
)

GENERIC_BINARY_SENSOR_SCHEMA = binary_sensor.binary_sensor_schema()

# Text sensor schema
GENERIC_TEXT_SENSOR_SCHEMA = text_sensor.text_sensor_schema()

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required("host"): cv.string,
    cv.Optional("port", default=13142): cv.port,

    # Numeric sensors
    cv.Optional("comp0_temp"): TEMP_SENSOR_SCHEMA,
    cv.Optional("comp1_temp"): TEMP_SENSOR_SCHEMA,
    cv.Optional("comp0_set_temp"): TEMP_SENSOR_SCHEMA,
    cv.Optional("comp1_set_temp"): TEMP_SENSOR_SCHEMA,
    cv.Optional("dc_voltage"): VOLT_SENSOR_SCHEMA,
    cv.Optional("battery_protection_level"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("power_source"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("compartment_count"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("icemaker_count"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp0_open_count"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp1_open_count"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("presented_temp_unit"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_hour_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_hour_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_day_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_day_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_week_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_week_latest"): GENERIC_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_hour_latest"): GENERIC_SENSOR_SCHEMA,

    # Binary sensors
    cv.Optional("comp0_door_open"): DOOR_BINARY_SENSOR_SCHEMA,
    cv.Optional("comp1_door_open"): DOOR_BINARY_SENSOR_SCHEMA,
    cv.Optional("cooler_power"): POWER_BINARY_SENSOR_SCHEMA,
    cv.Optional("comp0_power"): POWER_BINARY_SENSOR_SCHEMA,
    cv.Optional("comp1_power"): POWER_BINARY_SENSOR_SCHEMA,
    cv.Optional("wifi_mode"): CONNECTIVITY_BINARY_SENSOR_SCHEMA,
    cv.Optional("bluetooth_mode"): GENERIC_BINARY_SENSOR_SCHEMA,
    cv.Optional("wifi_ap_connected"): CONNECTIVITY_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_comm_alarm"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_ntc_open_large"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_ntc_short_large"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_solenoid_valve"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_ntc_open_small"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_ntc_short_small"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_fan_overvoltage"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_compressor_start_fail"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_compressor_speed"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("err_controller_overtemp"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("alert_temp_dcm"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("alert_temp_cc"): PROBLEM_BINARY_SENSOR_SCHEMA,
    cv.Optional("alert_door"): DOOR_BINARY_SENSOR_SCHEMA,
    cv.Optional("alert_voltage"): GENERIC_BINARY_SENSOR_SCHEMA,

    # Text sensors
    cv.Optional("device_name"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("product_serial"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_recommended_range"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_recommended_range"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_temp_range"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_temp_range"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_hour_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_hour_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_day_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_day_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_week_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_week_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_hour_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_day_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_week_json"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_0"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_1"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_2"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_0"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_1"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_2"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_3"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_4"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_0"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_1"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_2"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_3"): GENERIC_TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_4"): GENERIC_TEXT_SENSOR_SCHEMA,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_host(config["host"]))
    cg.add(var.set_port(config.get("port", 13142)))

    # Helper to register float sensors
    async def setup_float_sensor(sensor_key):
        if sensor_key in config:
            conf = config[sensor_key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(var, f"set_{sensor_key}")(sens))

    # Helper to register binary sensors
    async def setup_binary_sensor(sensor_key):
        if sensor_key in config:
            conf = config[sensor_key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(var, f"set_{sensor_key}")(sens))

    # Helper to register text sensors
    async def setup_text_sensor(sensor_key):
        if sensor_key in config:
            conf = config[sensor_key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(var, f"set_{sensor_key}")(sens))

    # === Float sensors ===
    float_sensors = [
        "comp0_temp", "comp1_temp", "comp0_set_temp", "comp1_set_temp",
        "dc_voltage", "battery_protection_level", "power_source",
        "compartment_count", "icemaker_count",
        "comp0_hist_hour_latest", "comp1_hist_hour_latest",
        "comp0_hist_day_latest", "comp1_hist_day_latest",
        "comp0_hist_week_latest", "comp1_hist_week_latest",
        "dc_current_hist_hour_latest",
        "comp0_open_count", "comp1_open_count",
        "presented_temp_unit"
    ]
    for key in float_sensors:
        await setup_float_sensor(key)

    # === Binary sensors ===
    binary_sensors = [
        "cooler_power", "comp0_power", "comp1_power", "comp0_door_open", "comp1_door_open",
        "icemaker_power", "wifi_mode", "bluetooth_mode", "wifi_ap_connected",
        "err_comm_alarm", "err_ntc_open_large", "err_ntc_short_large", "err_solenoid_valve",
        "err_ntc_open_small", "err_ntc_short_small", "err_fan_overvoltage",
        "err_compressor_start_fail", "err_compressor_speed", "err_controller_overtemp",
        "alert_temp_dcm", "alert_temp_cc", "alert_door", "alert_voltage"
    ]
    for key in binary_sensors:
        await setup_binary_sensor(key)

    # === Text sensors ===
    text_sensors = [
        "product_serial", "device_name",
        "comp0_recommended_range", "comp1_recommended_range",
        "comp0_temp_range", "comp1_temp_range",
        "comp0_hist_hour_json", "comp1_hist_hour_json",
        "comp0_hist_day_json", "comp1_hist_day_json",
        "comp0_hist_week_json", "comp1_hist_week_json",
        "dc_current_hist_hour_json", "dc_current_hist_day_json", "dc_current_hist_week_json",
        "station_ssid_0", "station_ssid_1", "station_ssid_2",
        "station_password_0", "station_password_1", "station_password_2",
        "station_password_3", "station_password_4",
        "cfx_direct_password_0", "cfx_direct_password_1", "cfx_direct_password_2",
        "cfx_direct_password_3", "cfx_direct_password_4"
    ]
    for key in text_sensors:
        await setup_text_sensor(key)