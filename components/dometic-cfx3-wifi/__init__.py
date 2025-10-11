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
)

DEPENDENCIES = ["network", "json", "sensor", "binary_sensor", "text_sensor"]
AUTO_LOAD = ["network", "json", "sensor", "binary_sensor", "text_sensor"]

# Use underscores (C++ identifiers can't contain '-')
dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic_cfx")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometicCFXComponent", cg.Component)

# Sensor schema for numeric sensors
SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(sensor.Sensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("unit_of_measurement"): cv.string,
    cv.Optional("device_class"): cv.string,
    cv.Optional("accuracy_decimals", default=1): cv.int_range(min=0, max=5),
    cv.Optional("disabled_by_default", default=False): cv.boolean,
    cv.Optional("force_update", default=False): cv.boolean
})

# Binary sensor schema
BINARY_SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("device_class"): cv.string,
    cv.Optional("disabled_by_default", default=False): cv.boolean,
    cv.Optional("force_update", default=False): cv.boolean
})

# Text sensor schema
TEXT_SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("disabled_by_default", default=False): cv.boolean,
    cv.Optional("force_update", default=False): cv.boolean,
})

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required("host"): cv.string,
    cv.Optional("port", default=13142): cv.port,

    # Numeric sensors
    cv.Optional("comp0_temp"): SENSOR_SCHEMA.extend({
        cv.Optional("unit_of_measurement", default=UNIT_CELSIUS): cv.string,
        cv.Optional("device_class", default=DEVICE_CLASS_TEMPERATURE): cv.string,
    }),
    cv.Optional("comp1_temp"): SENSOR_SCHEMA.extend({
        cv.Optional("unit_of_measurement", default=UNIT_CELSIUS): cv.string,
        cv.Optional("device_class", default=DEVICE_CLASS_TEMPERATURE): cv.string,
    }),
    cv.Optional("dc_voltage"): SENSOR_SCHEMA.extend({
        cv.Optional("unit_of_measurement", default=UNIT_VOLT): cv.string,
        cv.Optional("device_class", default=DEVICE_CLASS_VOLTAGE): cv.string,
    }),
    cv.Optional("battery_protection_level"): SENSOR_SCHEMA,
    cv.Optional("power_source"): SENSOR_SCHEMA,
    cv.Optional("compartment_count"): SENSOR_SCHEMA,
    cv.Optional("icemaker_count"): SENSOR_SCHEMA,
    cv.Optional("comp0_open_count"): SENSOR_SCHEMA,
    cv.Optional("comp1_open_count"): SENSOR_SCHEMA,
    cv.Optional("presented_temp_unit"): SENSOR_SCHEMA,
    cv.Optional("comp0_hist_hour_latest"): SENSOR_SCHEMA,
    cv.Optional("comp1_hist_hour_latest"): SENSOR_SCHEMA,
    cv.Optional("comp0_hist_day_latest"): SENSOR_SCHEMA,
    cv.Optional("comp1_hist_day_latest"): SENSOR_SCHEMA,
    cv.Optional("comp0_hist_week_latest"): SENSOR_SCHEMA,
    cv.Optional("comp1_hist_week_latest"): SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_hour_latest"): SENSOR_SCHEMA,

    # Binary sensors
    cv.Optional("comp0_door_open"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_DOOR): cv.string,
    }),
    cv.Optional("comp1_door_open"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_DOOR): cv.string,
    }),
    cv.Optional("cooler_power"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_POWER): cv.string,
    }),
    cv.Optional("comp0_power"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_POWER): cv.string,
    }),
    cv.Optional("comp1_power"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_POWER): cv.string,
    }),
    cv.Optional("wifi_mode"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_CONNECTIVITY): cv.string,
    }),
    cv.Optional("bluetooth_mode"): BINARY_SENSOR_SCHEMA,
    cv.Optional("wifi_ap_connected"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_CONNECTIVITY): cv.string,
    }),
    cv.Optional("err_comm_alarm"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_ntc_open_large"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_ntc_short_large"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_solenoid_valve"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_ntc_open_small"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_ntc_short_small"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_fan_overvoltage"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_compressor_start_fail"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_compressor_speed"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("err_controller_overtemp"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("alert_temp_dcm"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("alert_temp_cc"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_PROBLEM): cv.string,
    }),
    cv.Optional("alert_door"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_DOOR): cv.string,
    }),
    cv.Optional("alert_voltage"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_VOLTAGE): cv.string,
    }),

    # Text sensors
    cv.Optional("device_name"): TEXT_SENSOR_SCHEMA,
    cv.Optional("product_serial"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_recommended_range"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_recommended_range"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_temp_range"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_temp_range"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_hour_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_hour_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_day_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_day_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp0_hist_week_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("comp1_hist_week_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_hour_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_day_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("dc_current_hist_week_json"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_0"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_1"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_ssid_2"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_0"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_1"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_2"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_3"): TEXT_SENSOR_SCHEMA,
    cv.Optional("station_password_4"): TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_0"): TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_1"): TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_2"): TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_3"): TEXT_SENSOR_SCHEMA,
    cv.Optional("cfx_direct_password_4"): TEXT_SENSOR_SCHEMA,
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
            obj = cg.new_Pvariable(conf[CONF_ID])
            if CONF_ICON in conf:
                cg.add(obj.set_icon(conf[CONF_ICON]))
            if "unit_of_measurement" in conf:
                cg.add(obj.set_unit_of_measurement(conf["unit_of_measurement"]))
            if "device_class" in conf:
                cg.add(obj.set_device_class(conf["device_class"]))
            if "accuracy_decimals" in conf:
                cg.add(obj.set_accuracy_decimals(conf["accuracy_decimals"]))
            cg.add(obj.set_disabled_by_default(conf.get("disabled_by_default", False)))
            cg.add(obj.set_force_update(conf.get("force_update", False)))
            await sensor.register_sensor(obj, conf)
            cg.add(getattr(var, f"set_{sensor_key}")(obj))

    # Helper to register binary sensors
    async def setup_binary_sensor(sensor_key):
        if sensor_key in config:
            conf = config[sensor_key]
            obj = cg.new_Pvariable(conf[CONF_ID])
            if CONF_ICON in conf:
                cg.add(obj.set_icon(conf[CONF_ICON]))
            if "device_class" in conf:
                cg.add(obj.set_device_class(conf["device_class"]))
            cg.add(obj.set_disabled_by_default(conf.get("disabled_by_default", False)))
            await binary_sensor.register_binary_sensor(obj, conf)
            cg.add(getattr(var, f"set_{sensor_key}")(obj))

    # Helper to register text sensors
    async def setup_text_sensor(sensor_key):
        if sensor_key in config:
            conf = config[sensor_key]
            obj = cg.new_Pvariable(conf[CONF_ID])
            if CONF_ICON in conf:
                cg.add(obj.set_icon(conf[CONF_ICON]))
            cg.add(obj.set_disabled_by_default(conf.get("disabled_by_default", False)))
            await text_sensor.register_text_sensor(obj, conf)
            cg.add(getattr(var, f"set_{sensor_key}")(obj))

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