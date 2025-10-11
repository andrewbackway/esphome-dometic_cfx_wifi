# sensor.py
# Sensor platform for dometic_cfx_wifi

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_ICON,
    UNIT_CELSIUS,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
)

from . import dometic_cfx_wifi_ns, CONF_DOMETIC_CFX_WIFI_ID, DometicCfxWifi, validate_topic

DEPENDENCIES = ["dometic_cfx_wifi"]

CONFIG_SCHEMA = sensor.sensor_schema(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(sensor.Sensor),
        cv.Required(CONF_DOMETIC_CFX_WIFI_ID): cv.use_id(DometicCfxWifi),
        cv.Required("type"): validate_topic,
    }).extend(cv.COMPONENT_SCHEMA),
    unit_of_measurement=UNIT_CELSIUS,
    device_class=DEVICE_CLASS_TEMPERATURE,
    state_class=STATE_CLASS_MEASUREMENT,
).extend({
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
})

async def to_code(config):
    var = await sensor.new_sensor(config)
    parent = await cg.get_variable(config[CONF_DOMETIC_CFX_WIFI_ID])
    cg.add(parent.register_sensor(config["type"], var))