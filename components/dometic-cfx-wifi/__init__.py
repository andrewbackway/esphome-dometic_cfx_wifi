import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_HOST, CONF_PORT
from esphome.components import sensor, binary_sensor, text_sensor

DEPENDENCIES = []
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]

dometic_cfx_wifi_ns = cg.esphome_ns.namespace("dometic-cfx-wifi")
DometicCfxWifi = dometic_cfx_wifi_ns.class_("DometicCfxWifi", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicCfxWifi),
    cv.Required(CONF_HOST): cv.string,
    cv.Required(CONF_PORT): cv.port,
    cv.Optional("sensor_comp0_temp"): sensor.sensor_schema(),
    cv.Optional("sensor_comp1_temp"): sensor.sensor_schema(),
    cv.Optional("sensor_comp0_set_temp"): sensor.sensor_schema(),
    cv.Optional("sensor_comp1_set_temp"): sensor.sensor_schema(),
    cv.Optional("sensor_comp0_open_count"): sensor.sensor_schema(),
    cv.Optional("sensor_comp1_open_count"): sensor.sensor_schema(),
    cv.Optional("binary_comp0_open"): binary_sensor.binary_sensor_schema(),
    cv.Optional("binary_comp1_open"): binary_sensor.binary_sensor_schema(),
    cv.Optional("sensor_dc_voltage"): sensor.sensor_schema(),
    cv.Optional("sensor_dc_current"): sensor.sensor_schema(),
    cv.Optional("text_serial"): text_sensor.text_sensor_schema(),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_host(config[CONF_HOST]))
    cg.add(var.set_port(config[CONF_PORT]))

    if "sensor_comp0_temp" in config:
        sens = await sensor.new_sensor(config["sensor_comp0_temp"])
        cg.add(var.set_sensor_comp0_temp(sens))

    if "sensor_comp1_temp" in config:
        sens = await sensor.new_sensor(config["sensor_comp1_temp"])
        cg.add(var.set_sensor_comp1_temp(sens))

    if "sensor_comp0_set_temp" in config:
        sens = await sensor.new_sensor(config["sensor_comp0_set_temp"])
        cg.add(var.set_sensor_comp0_set_temp(sens))

    if "sensor_comp1_set_temp" in config:
        sens = await sensor.new_sensor(config["sensor_comp1_set_temp"])
        cg.add(var.set_sensor_comp1_set_temp(sens))

    if "sensor_comp0_open_count" in config:
        sens = await sensor.new_sensor(config["sensor_comp0_open_count"])
        cg.add(var.set_sensor_comp0_open_count(sens))

    if "sensor_comp1_open_count" in config:
        sens = await sensor.new_sensor(config["sensor_comp1_open_count"])
        cg.add(var.set_sensor_comp1_open_count(sens))

    if "binary_comp0_open" in config:
        bs = await binary_sensor.new_binary_sensor(config["binary_comp0_open"])
        cg.add(var.set_binary_comp0_open(bs))

    if "binary_comp1_open" in config:
        bs = await binary_sensor.new_binary_sensor(config["binary_comp1_open"])
        cg.add(var.set_binary_comp1_open(bs))

    if "sensor_dc_voltage" in config:
        sens = await sensor.new_sensor(config["sensor_dc_voltage"])
        cg.add(var.set_sensor_dc_voltage(sens))

    if "sensor_dc_current" in config:
        sens = await sensor.new_sensor(config["sensor_dc_current"])
        cg.add(var.set_sensor_dc_current(sens))

    if "text_serial" in config:
        ts = await text_sensor.new_text_sensor(config["text_serial"])
        cg.add(var.set_text_serial(ts))
