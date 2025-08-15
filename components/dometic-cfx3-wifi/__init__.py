import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_ICON,
    CONF_NAME,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_DOOR,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_VOLTAGE,
)
from esphome.components import sensor, binary_sensor, text_sensor

DEPENDENCIES = ["wifi"]

dometic_cfx3_wifi_ns = cg.esphome_ns.namespace("dometic_cfx3_wifi")
DometicCFX3WiFi = dometic_cfx3_wifi_ns.class_("DometicCFX3WiFi", cg.Component)

# Sensor schema for numeric sensors
SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(sensor.Sensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("unit_of_measurement"): cv.string,
    cv.Optional("device_class"): cv.string,
    cv.Optional("accuracy_decimals", default=1): cv.int_range(min=0, max=5),
    cv.Optional("disabled_by_default", default=False): cv.boolean,
})

# Binary sensor schema
BINARY_SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("device_class"): cv.string,
    cv.Optional("disabled_by_default", default=False): cv.boolean,
})

# Text sensor schema
TEXT_SENSOR_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
    cv.Optional("disabled_by_default", default=False): cv.boolean,
})

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCFX3WiFi),

    # Numeric sensors
    cv.Optional("cooler_temp_set"): SENSOR_SCHEMA,
    cv.Optional("comp0_temp_set"): SENSOR_SCHEMA,
    cv.Optional("comp1_temp_set"): SENSOR_SCHEMA,
    cv.Optional("cooler_voltage"): SENSOR_SCHEMA,
    cv.Optional("cooler_current"): SENSOR_SCHEMA,
    cv.Optional("cooler_power"): SENSOR_SCHEMA,
    cv.Optional("cooler_energy"): SENSOR_SCHEMA,
    cv.Optional("cooler_temp"): SENSOR_SCHEMA,
    cv.Optional("comp0_temp"): SENSOR_SCHEMA,
    cv.Optional("comp1_temp"): SENSOR_SCHEMA,
    cv.Optional("comp0_fan_speed"): SENSOR_SCHEMA,
    cv.Optional("comp1_fan_speed"): SENSOR_SCHEMA,

    # Binary sensors
    cv.Optional("comp0_door_open"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_DOOR): cv.string,
    }),
    cv.Optional("comp1_door_open"): BINARY_SENSOR_SCHEMA.extend({
        cv.Optional("device_class", default=DEVICE_CLASS_DOOR): cv.string,
    }),
    cv.Optional("cooler_power_on"): BINARY_SENSOR_SCHEMA.extend({
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
    cv.Optional("serial_number"): TEXT_SENSOR_SCHEMA,
    cv.Optional("model_number"): TEXT_SENSOR_SCHEMA,
    cv.Optional("firmware_version"): TEXT_SENSOR_SCHEMA,
    cv.Optional("mac_address"): TEXT_SENSOR_SCHEMA,
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Numeric sensors
    for sensor_key in [
        "cooler_temp_set", "comp0_temp_set", "comp1_temp_set",
        "cooler_voltage", "cooler_current", "cooler_power",
        "cooler_energy", "cooler_temp", "comp0_temp", "comp1_temp",
        "comp0_fan_speed", "comp1_fan_speed"
    ]:
        if sensor_key in config:
            sensor_conf = config[sensor_key]
            sensor_obj = cg.new_Pvariable(sensor_conf[CONF_ID])
            if CONF_ICON in sensor_conf:
                cg.add(sensor_obj.set_icon(sensor_conf[CONF_ICON]))
            if "unit_of_measurement" in sensor_conf:
                cg.add(sensor_obj.set_unit_of_measurement(sensor_conf["unit_of_measurement"]))
            if "device_class" in sensor_conf:
                cg.add(sensor_obj.set_device_class(sensor_conf["device_class"]))
            if "accuracy_decimals" in sensor_conf:
                cg.add(sensor_obj.set_accuracy_decimals(sensor_conf["accuracy_decimals"]))
            await sensor.register_sensor(sensor_obj, sensor_conf)
            cg.add(getattr(var, f"set_{sensor_key}")(sensor_obj))

    # Binary sensors
    for bs_key in [
        "comp0_door_open", "comp1_door_open", "cooler_power_on",
        "comp0_power", "comp1_power", "wifi_mode", "bluetooth_mode",
        "wifi_ap_connected", "err_comm_alarm", "err_ntc_open_large",
        "err_ntc_short_large", "err_solenoid_valve", "err_ntc_open_small",
        "err_ntc_short_small", "err_fan_overvoltage", "err_compressor_start_fail",
        "err_compressor_speed", "err_controller_overtemp", "alert_temp_dcm",
        "alert_temp_cc", "alert_door", "alert_voltage"
    ]:
        if bs_key in config:
            bs_conf = config[bs_key]
            bs_obj = cg.new_Pvariable(bs_conf[CONF_ID])
            if CONF_ICON in bs_conf:
                cg.add(bs_obj.set_icon(bs_conf[CONF_ICON]))
            if "device_class" in bs_conf:
                cg.add(bs_obj.set_device_class(bs_conf["device_class"]))
            await binary_sensor.register_binary_sensor(bs_obj, bs_conf)
            cg.add(getattr(var, f"set_{bs_key}")(bs_obj))

    # Text sensors
    for ts_key in ["serial_number", "model_number", "firmware_version", "mac_address"]:
        if ts_key in config:
            ts_conf = config[ts_key]
            ts_obj = cg.new_Pvariable(ts_conf[CONF_ID])
            if CONF_ICON in ts_conf:
                cg.add(ts_obj.set_icon(ts_conf[CONF_ICON]))
            await text_sensor.register_text_sensor(ts_obj, ts_conf)
            cg.add(getattr(var, f"set_{ts_key}")(ts_obj))
