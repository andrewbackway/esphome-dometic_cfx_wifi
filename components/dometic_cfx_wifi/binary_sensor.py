# binary_sensor.py
# Binary sensor platform for dometic_cfx_wifi

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_ICON,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import dometic_cfx_wifi_ns, CONF_DOMETIC_CFX_WIFI_ID, DometicCfxWifi, validate_topic

DEPENDENCIES = ["dometic_cfx_wifi"]

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
        cv.Required(CONF_DOMETIC_CFX_WIFI_ID): cv.use_id(DometicCfxWifi),
        cv.Required("type"): validate_topic,
    }).extend(cv.COMPONENT_SCHEMA),
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
).extend({
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_ICON): cv.icon,
})

async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    parent = await cg.get_variable(config[CONF_DOMETIC_CFX_WIFI_ID])
    cg.add(parent.register_binary_sensor(config["type"], var))