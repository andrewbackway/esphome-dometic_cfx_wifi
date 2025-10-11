
# New-style ESPHome external component config for dometic_cfx_wifi
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_ICON,
)

DEPENDENCIES = ["network", "json"]
AUTO_LOAD = ["network", "json"]

dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic_cfx")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometicCFXComponent", cg.Component)

# List of valid topic names (should match C++ TOPICS array)
TOPICS = [
    "COMPARTMENT_0_MEASURED_TEMPERATURE",
    "COMPARTMENT_1_MEASURED_TEMPERATURE",
    "COMPARTMENT_0_SET_TEMPERATURE",
    "COMPARTMENT_1_SET_TEMPERATURE",
    "COMPARTMENT_0_DOOR_OPEN",
    "COMPARTMENT_1_DOOR_OPEN",
    "COMPARTMENT_0_POWER",
    "COMPARTMENT_1_POWER",
    "COOLER_POWER",
    "DC_VOLTAGE",
    "BATTERY_PROTECTION_LEVEL",
    "POWER_SOURCE",
    "COMPARTMENT_COUNT",
    "ICEMAKER_COUNT",
    "ICEMAKER_POWER",
    "COMPARTMENT_0_OPEN_COUNT",
    "COMPARTMENT_1_OPEN_COUNT",
    "PRESENTED_TEMPERATURE_UNIT",
    "COMPARTMENT_0_HIST_HOUR_LATEST",
    "COMPARTMENT_1_HIST_HOUR_LATEST",
    "COMPARTMENT_0_HIST_DAY_LATEST",
    "COMPARTMENT_1_HIST_DAY_LATEST",
    "COMPARTMENT_0_HIST_WEEK_LATEST",
    "COMPARTMENT_1_HIST_WEEK_LATEST",
    "DC_CURRENT_HIST_HOUR_LATEST",
    "DEVICE_NAME",
    "PRODUCT_SERIAL_NUMBER",
    "COMPARTMENT_0_RECOMMENDED_RANGE",
    "COMPARTMENT_1_RECOMMENDED_RANGE",
    "COMPARTMENT_0_TEMPERATURE_RANGE",
    "COMPARTMENT_1_TEMPERATURE_RANGE",
    "COMPARTMENT_0_HIST_HOUR_JSON",
    "COMPARTMENT_1_HIST_HOUR_JSON",
    "COMPARTMENT_0_HIST_DAY_JSON",
    "COMPARTMENT_1_HIST_DAY_JSON",
    "COMPARTMENT_0_HIST_WEEK_JSON",
    "COMPARTMENT_1_HIST_WEEK_JSON",
    "DC_CURRENT_HIST_HOUR_JSON",
    "DC_CURRENT_HIST_DAY_JSON",
    "DC_CURRENT_HIST_WEEK_JSON",
    "STATION_SSID_0",
    "STATION_SSID_1",
    "STATION_SSID_2",
    "STATION_PASSWORD_0",
    "STATION_PASSWORD_1",
    "STATION_PASSWORD_2",
    "STATION_PASSWORD_3",
    "STATION_PASSWORD_4",
    "CFX_DIRECT_PASSWORD_0",
    "CFX_DIRECT_PASSWORD_1",
    "CFX_DIRECT_PASSWORD_2",
    "CFX_DIRECT_PASSWORD_3",
    "CFX_DIRECT_PASSWORD_4",
    "WIFI_MODE",
    "BLUETOOTH_MODE",
    "WIFI_AP_CONNECTED",
    "ERR_COMM_ALARM",
    "ERR_NTC_OPEN_LARGE",
    "ERR_NTC_SHORT_LARGE",
    "ERR_SOLENOID_VALVE",
    "ERR_NTC_OPEN_SMALL",
    "ERR_NTC_SHORT_SMALL",
    "ERR_FAN_OVERVOLTAGE",
    "ERR_COMPRESSOR_START_FAIL",
    "ERR_COMPRESSOR_SPEED",
    "ERR_CONTROLLER_OVERTEMP",
    "ALERT_TEMP_DCM",
    "ALERT_TEMP_CC",
    "ALERT_DOOR",
    "ALERT_VOLTAGE"
]

# dometic_cfx_wifi component config (host, id)
DOMETIC_CFX_WIFI_COMPONENT_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required("host"): cv.string,
    cv.Optional("port", default=13142): cv.port,
})

CONFIG_SCHEMA = cv.Schema({
    cv.Required("dometic_cfx_wifi"): cv.ensure_list(DOMETIC_CFX_WIFI_COMPONENT_SCHEMA),
})

# Platform schemas for sensor, binary_sensor, text_sensor
def validate_topic(value):
    value = cv.string_strict(value)
    if value not in TOPICS:
        raise cv.Invalid(f"type '{value}' is not a valid dometic_cfx_wifi topic. Valid: {TOPICS}")
    return value

def get_component_id(value):
    return cv.use_id(DometicCfxWifi)(value)

SENSOR_PLATFORM_SCHEMA = sensor.sensor_schema(
    DometicCfxWifi
).extend({
    cv.Required("dometic_cfx_wifi_id"): cv.use_id(DometicCfxWifi),
    cv.Required("type"): validate_topic,
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
})

BINARY_SENSOR_PLATFORM_SCHEMA = binary_sensor.binary_sensor_schema(
    DometicCfxWifi
).extend({
    cv.Required("dometic_cfx_wifi_id"): cv.use_id(DometicCfxWifi),
    cv.Required("type"): validate_topic,
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
})

TEXT_SENSOR_PLATFORM_SCHEMA = text_sensor.text_sensor_schema(
    DometicCfxWifi
).extend({
    cv.Required("dometic_cfx_wifi_id"): cv.use_id(DometicCfxWifi),
    cv.Required("type"): validate_topic,
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
})

async def to_code(config):
    # Register all dometic_cfx_wifi components
    for comp in config["dometic_cfx_wifi"]:
        var = cg.new_Pvariable(comp[CONF_ID])
        await cg.register_component(var, comp)
        cg.add(var.set_host(comp["host"]))
        cg.add(var.set_port(comp.get("port", 13142)))

# Sensor platform
@sensor.register_platform("dometic_cfx_wifi", SENSOR_PLATFORM_SCHEMA)
async def sensor_to_code(config):
    paren = await cg.get_variable(config["dometic_cfx_wifi_id"])
    var = await sensor.new_sensor(config)
    cg.add(paren.register_sensor(config["type"], var))

# Binary sensor platform
@binary_sensor.register_platform("dometic_cfx_wifi", BINARY_SENSOR_PLATFORM_SCHEMA)
async def binary_sensor_to_code(config):
    paren = await cg.get_variable(config["dometic_cfx_wifi_id"])
    var = await binary_sensor.new_binary_sensor(config)
    cg.add(paren.register_binary_sensor(config["type"], var))

# Text sensor platform
@text_sensor.register_platform("dometic_cfx_wifi", TEXT_SENSOR_PLATFORM_SCHEMA)
async def text_sensor_to_code(config):
    paren = await cg.get_variable(config["dometic_cfx_wifi_id"])
    var = await text_sensor.new_text_sensor(config)
    cg.add(paren.register_text_sensor(config["type"], var))
