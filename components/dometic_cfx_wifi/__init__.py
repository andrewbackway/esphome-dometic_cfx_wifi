# __init__.py
# Main component for dometic_cfx_wifi

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_HOST, CONF_PORT, CONF_ICON, CONF_NAME

from esphome.components import sensor, binary_sensor, text_sensor

DEPENDENCIES = ["network", "json"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]

dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic_cfx_wifi")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometicCfxWifi", cg.Component)

# List of valid topic names (shared across platforms)
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

def validate_topic(value):
    value = cv.string_strict(value)
    if value not in TOPICS:
        raise cv.Invalid(f"type '{value}' is not a valid dometic_cfx_wifi topic. Valid topics: {', '.join(TOPICS)}")
    return value

CONF_DOMETIC_CFX_WIFI_ID = "dometic_cfx_wifi_id"

# Main component schema
DOMETIC_CFX_WIFI_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required(CONF_HOST): cv.string,
    cv.Optional(CONF_PORT, default=13142): cv.port,
})

CONFIG_SCHEMA = cv.Schema({
    cv.Required("dometic_cfx_wifi"): cv.ensure_list(DOMETIC_CFX_WIFI_SCHEMA),
})

async def to_code(config):
    for conf in config["dometic_cfx_wifi"]:
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        cg.add(var.set_host(conf[CONF_HOST]))
        cg.add(var.set_port(conf.get(CONF_PORT, 13142)))