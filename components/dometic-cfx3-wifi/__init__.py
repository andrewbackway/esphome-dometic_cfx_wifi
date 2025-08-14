import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import sensor, binary_sensor, text_sensor

DEPENDENCIES = []
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]

dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic-cfx3-wifi")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometDometicCFXComponenticCfxWifi", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required("host"): cv.string,
    cv.Optional("port"): cv.port
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_host(config["host"]))
    cg.add(var.set_port(config["port"]))
