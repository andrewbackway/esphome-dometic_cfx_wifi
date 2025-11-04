import json
import signal
import socket
import traceback
from threading import Event

import time
from prometheus_client import Gauge, Counter, start_http_server

gauge_compartment_0_temp = Gauge('cfx3_compartment_0_temp', 'Compartment 0 Temperature')
gauge_compartment_1_temp = Gauge('cfx3_compartment_1_temp', 'Compartment 1 Temperature')
gauge_compartment_0_open = Gauge('cfx3_compartment_0_open', 'Compartment 0 Open')
gauge_compartment_0_open_count = Gauge('cfx3_compartment_0_open_count', 'Compartment 0 Open Count')
gauge_compartment_1_open = Gauge('cfx3_compartment_1_open', 'Compartment 1 Open')
gauge_compartment_1_open_count = Gauge('cfx3_compartment_1_open_count', 'Compartment 1 Open Count')
gauge_dc_current = Gauge('cfx3_dc_current', 'DC Current')
gauge_dc_voltage = Gauge('cfx3_dc_voltage', 'DC Voltage')

created_socket_count = Counter('cfx3_exporter_socket_count', '')
socket_timeouts_count = Counter('cfx3_exporter_socket_timeouts_count', '')


def decode(value_type, value):
    if value_type == ddmDataTypes.INT16_DECIDEGREE_CELSIUS:
        if not len(value):
            return None
        return int16_degrees_celsius(value[0], value[1])
    if value_type == ddmDataTypes.INT8_BOOLEAN:
        return bool(value[0])

    if value_type == ddmDataTypes.HISTORY_DATA_ARRAY:
        temp1 = int16_degrees_celsius(value[0], value[1])  # most recent
        temp2 = int16_degrees_celsius(value[2], value[3])
        temp3 = int16_degrees_celsius(value[4], value[5])
        temp4 = int16_degrees_celsius(value[6], value[7])
        temp5 = int16_degrees_celsius(value[8], value[9])
        temp6 = int16_degrees_celsius(value[10], value[11])
        temp7 = int16_degrees_celsius(value[12], value[13])  # oldest value
        date = (value[14])  # 14th byte has the timestamp sent from the firmware.
        return [temp1, temp2, temp3, temp4, temp5, temp6, temp7, date]

    if value_type == ddmDataTypes.INT16_DECICURRENT_VOLT:
        return int16_degrees_celsius(value[0], value[1])

    print(f"Unknown value type {value_type=}")


def record(value, topic):
    if not topic or value is None:
        return
    field = getattr(ddmTopics, topic)
    if field is not None and isinstance(field, dict) and 'METRIC' in field:
        metric = field['METRIC']
        metric(value)


def encode(payload):
    return json.dumps({"ddmp": payload}).encode('utf-8') + b"\r"


def sub(s, topic):
    d = encode([ddmActions.SUBSCRIBE] + topic["PARAM"])
    print(f'sending {d}')
    s.sendall(d)


def recv(s):
    b = s.recv(1024)
    d = json.loads(b.decode().splitlines()[0])
    payload = d["ddmp"]
    print(f'received payload: {payload[0]}')
    return payload[0], payload[1:]


def ack(s):
    print("sending ack")
    d = encode([ddmActions.ACK])
    s.sendall(d)


def ping(s):
    d = encode([ddmActions.PING])
    s.sendall(d)


def int16_degrees_celsius(byte1, byte2):
    if byte2 >= 0b10000000:
        # negative number
        deci_degree = ((byte2 * 256) + byte1) - 65536
    else:
        # zero or positive number
        deci_degree = (byte2 << 8) | byte1
    return deci_degree / 10


def get_value_type(topic):
    for k, v in vars(ddmTopics).items():
        if isinstance(v, dict) and v["PARAM"] == topic:
            return v["TYPE"], k
    return None, None


class ddmDataTypes:
    INT16_DECIDEGREE_CELSIUS = 'INT16_DECIDEGREE_CELSIUS'
    INT8_BOOLEAN = 'INT8_BOOLEAN'
    INT8_NUMBER = 'INT8_NUMBER'
    INT16_DECICURRENT_VOLT = 'INT16_DECICURRENT_VOLT'
    UINT8_NUMBER = 'UINT8_NUMBER'
    UTF8_STRING = 'UTF8_STRING'
    HISTORY_DATA_ARRAY = 'HISTORY_DATA_ARRAY'
    INT16_ARRAY = 'INT16_ARRAY'
    EMPTY = 'EMPTY'


def on_door_0_open(is_open):
    gauge_compartment_0_open.set(1 if is_open else 0)
    if is_open:
        gauge_compartment_0_open_count.inc()


def on_door_1_open(is_open):
    gauge_compartment_1_open.set(1 if is_open else 0)
    if is_open:
        gauge_compartment_1_open_count.inc()


class ddmTopics:
    # Subscribe All SZ topics
    SUBSCRIBE_APP_SZ = {
        "PARAM": [1, 0, 0, 129],
        "TYPE": ddmDataTypes.EMPTY,
    }
    # Subscribe All SZ with icemaker topics
    SUBSCRIBE_APP_SZI = {
        "PARAM": [2, 0, 0, 129],
        "TYPE": ddmDataTypes.EMPTY,
    }
    # Subscribe All DZ
    SUBSCRIBE_APP_DZ = {
        "PARAM": [3, 0, 0, 129],
        "TYPE": ddmDataTypes.EMPTY,
    }
    # Product Information
    PRODUCT_SERIAL_NUMBER = {
        "PARAM": [0, 193, 0, 0],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    # # Properties
    COMPARTMENT_COUNT = {
        "PARAM": [0, 128, 0, 1],
        "TYPE": ddmDataTypes.INT8_NUMBER,
    }
    ICEMAKER_COUNT = {
        "PARAM": [0, 129, 0, 1],
        "TYPE": ddmDataTypes.INT8_NUMBER,
    }
    # # Compartment
    COMPARTMENT_0_POWER = {
        "PARAM": [0, 0, 1, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    COMPARTMENT_1_POWER = {
        "PARAM": [16, 0, 1, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    COMPARTMENT_0_MEASURED_TEMPERATURE = {
        "PARAM": [0, 1, 1, 1],
        "TYPE": ddmDataTypes.INT16_DECIDEGREE_CELSIUS,
        "METRIC": lambda v: gauge_compartment_0_temp.set(v)
    }
    COMPARTMENT_1_MEASURED_TEMPERATURE = {
        "PARAM": [16, 1, 1, 1],
        "TYPE": ddmDataTypes.INT16_DECIDEGREE_CELSIUS,
        "METRIC": lambda v: gauge_compartment_1_temp.set(v)
    }
    COMPARTMENT_0_DOOR_OPEN = {
        "PARAM": [0, 8, 1, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
        "METRIC": on_door_0_open
    }
    COMPARTMENT_1_DOOR_OPEN = {
        "PARAM": [16, 8, 1, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
        "METRIC": on_door_1_open
    }
    COMPARTMENT_0_SET_TEMPERATURE = {
        "PARAM": [0, 2, 1, 1],
        "TYPE": ddmDataTypes.INT16_DECIDEGREE_CELSIUS,
    }
    COMPARTMENT_1_SET_TEMPERATURE = {
        "PARAM": [16, 2, 1, 1],
        "TYPE": ddmDataTypes.INT16_DECIDEGREE_CELSIUS,
    }
    COMPARTMENT_0_RECOMMENDED_RANGE = {
        "PARAM": [0, 129, 1, 1],
        "TYPE": ddmDataTypes.INT16_ARRAY,
    }
    COMPARTMENT_1_RECOMMENDED_RANGE = {
        "PARAM": [16, 129, 1, 1],
        "TYPE": ddmDataTypes.INT16_ARRAY,
    }
    # Presented Temperature Unit
    PRESENTED_TEMPERATURE_UNIT = {
        "PARAM": [0, 0, 2, 1],
        "TYPE": ddmDataTypes.INT8_NUMBER,
    }
    COMPARTMENT_0_TEMPERATURE_RANGE = {
        "PARAM": [0, 128, 1, 1],
        "TYPE": ddmDataTypes.INT16_ARRAY,
    }
    COMPARTMENT_1_TEMPERATURE_RANGE = {
        "PARAM": [16, 128, 1, 1],
        "TYPE": ddmDataTypes.INT16_ARRAY,
    }
    # Power
    COOLER_POWER = {
        "PARAM": [0, 0, 3, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    BATTERY_VOLTAGE_LEVEL = {
        "PARAM": [0, 1, 3, 1],
        "TYPE": ddmDataTypes.INT16_DECICURRENT_VOLT,
        "METRIC": lambda v: gauge_dc_voltage.set(v)
    }
    BATTERY_PROTECTION_LEVEL = {
        "PARAM": [0, 2, 3, 1],
        "TYPE": ddmDataTypes.UINT8_NUMBER,
    }
    POWER_SOURCE = {
        "PARAM": [0, 5, 3, 1],
        "TYPE": ddmDataTypes.INT8_NUMBER,
    }
    ICEMAKER_POWER = {
        "PARAM": [0, 6, 3, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    # Errors
    COMMUNICATION_ALARM = {
        "PARAM": [0, 3, 4, 1],
        # Type is not specified in documentation. Using INT8_BOOLEAN for now
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    NTC_OPEN_LARGE_ERROR = {
        "PARAM": [0, 1, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    NTC_SHORT_LARGE_ERROR = {
        "PARAM": [0, 2, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    SOLENOID_VALVE_ERROR = {
        "PARAM": [0, 9, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    NTC_OPEN_SMALL_ERROR = {
        # Temporary params for mocked broker. Params should be "PARAM": [0, 11, 4, 1],
        "PARAM": [0, 17, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    NTC_SHORT_SMALL_ERROR = {
        # Temporary params for mocked broker. Params should be "PARAM": [0, 12, 4, 1],
        "PARAM": [0, 18, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    FAN_OVERVOLTAGE_ERROR = {
        # Temporary params for mocked broker, Params should be "PARAM": [0, 32, 4, 1]
        "PARAM": [0, 50, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    COMPRESSOR_START_FAIL_ERROR = {
        # Temporary params for mocked broker. Params should be "PARAM": [0, 33, 4, 1],
        "PARAM": [0, 51, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    COMPRESSOR_SPEED_ERROR = {
        # Temporary params for mocked broker. Params should be "PARAM": [0, 34, 4, 1],
        "PARAM": [0, 52, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    CONTROLLER_OVER_TEMPERATURE = {
        # Temporary params for mocked broker. Params should be "PARAM": [0, 35, 4, 1],
        "PARAM": [0, 53, 4, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    # Alerts
    TEMPERATURE_ALERT_DCM = {
        "PARAM": [0, 3, 5, 1],  # used for DCM as source
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    TEMPERATURE_ALERT_CC = {
        "PARAM": [0, 0, 5, 1],  # used for CC as source
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }

    DOOR_ALERT = {
        "PARAM": [0, 1, 5, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    VOLTAGE_ALERT = {
        "PARAM": [0, 2, 5, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    # Communication
    DEVICE_NAME = {
        "PARAM": [0, 0, 6, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    WIFI_MODE = {
        "PARAM": [0, 1, 6, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    BLUETOOTH_MODE = {
        "PARAM": [0, 3, 6, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    WIFI_AP_CONNECTED = {
        "PARAM": [0, 8, 6, 1],
        "TYPE": ddmDataTypes.INT8_BOOLEAN,
    }
    # WiFi Settings
    STATION_SSID_0 = {
        "PARAM": [0, 0, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_SSID_1 = {
        "PARAM": [1, 0, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_SSID_2 = {
        "PARAM": [2, 0, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_PASSWORD_0 = {
        "PARAM": [0, 1, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_PASSWORD_1 = {
        "PARAM": [1, 1, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_PASSWORD_2 = {
        "PARAM": [2, 1, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_PASSWORD_3 = {
        "PARAM": [3, 1, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    STATION_PASSWORD_4 = {
        "PARAM": [4, 1, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    CFX_DIRECT_PASSWORD_0 = {
        "PARAM": [0, 2, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    CFX_DIRECT_PASSWORD_1 = {
        "PARAM": [1, 2, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    CFX_DIRECT_PASSWORD_2 = {
        "PARAM": [2, 2, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    CFX_DIRECT_PASSWORD_3 = {
        "PARAM": [3, 2, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    CFX_DIRECT_PASSWORD_4 = {
        "PARAM": [4, 2, 7, 1],
        "TYPE": ddmDataTypes.UTF8_STRING,
    }
    COMPARTMENT_0_TEMPERATURE_HISTORY_HOUR = {
        "PARAM": [0, 64, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    COMPARTMENT_1_TEMPERATURE_HISTORY_HOUR = {
        "PARAM": [16, 64, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    COMPARTMENT_0_TEMPERATURE_HISTORY_DAY = {
        "PARAM": [0, 65, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    COMPARTMENT_1_TEMPERATURE_HISTORY_DAY = {
        "PARAM": [16, 65, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    COMPARTMENT_0_TEMPERATURE_HISTORY_WEEK = {
        "PARAM": [0, 66, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    COMPARTMENT_1_TEMPERATURE_HISTORY_WEEK = {
        "PARAM": [16, 66, 1, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    DC_CURRENT_HISTORY_HOUR = {
        "PARAM": [0, 64, 3, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
        "METRIC": lambda v: gauge_dc_current.set(v[0])
    }
    DC_CURRENT_HISTORY_DAY = {
        "PARAM": [0, 65, 3, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }
    DC_CURRENT_HISTORY_WEEK = {
        "PARAM": [0, 66, 3, 1],
        "TYPE": ddmDataTypes.HISTORY_DATA_ARRAY,
    }


class ddmActions:
    PUBLISH = 0
    SUBSCRIBE = 1
    PING = 2
    HELLO = 3
    ACK = 4
    NAK = 5
    NOP = 6


def main():
    print('starting')
    event = Event()

    def exit_gracefully(signum, frame):
        print('exit requested')
        event.set()
        return None

    signal.signal(signal.SIGINT, exit_gracefully)
    signal.signal(signal.SIGTERM, exit_gracefully)
    start_http_server(port=9000)

    host = "10.0.1.35"
    port = 13142

    while not event.is_set():
        print('creating socket')
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(30)
        created_socket_count.inc()
        try:
            print('connecting socket')
            s.connect((host, port))

            print("waiting for first message")
            code, _ = recv(s)
            if code != ddmActions.NOP:
                print('expected NOP packet first')
                exit(1)

            print('connected... sending ping')
            ping(s)
            print('waiting for ack')
            code, _ = recv(s)
            if code != ddmActions.ACK:
                print('expected ack back')
                exit(1)
            print('received ack... subscribing')

            sub(s, ddmTopics.SUBSCRIBE_APP_DZ)
            sub(s, ddmTopics.BATTERY_VOLTAGE_LEVEL)
            sub(s, ddmTopics.PRODUCT_SERIAL_NUMBER)
            sub(s, ddmTopics.COMPARTMENT_0_MEASURED_TEMPERATURE)
            sub(s, ddmTopics.COMPARTMENT_1_MEASURED_TEMPERATURE)
            sub(s, ddmTopics.COMPARTMENT_0_DOOR_OPEN)
            sub(s, ddmTopics.COMPARTMENT_1_DOOR_OPEN)
            sub(s, ddmTopics.COMPARTMENT_0_SET_TEMPERATURE)
            sub(s, ddmTopics.COMPARTMENT_1_SET_TEMPERATURE)
            sub(s, ddmTopics.DC_CURRENT_HISTORY_HOUR)

            print("subscribed to topics")

            connected_at = time.time()

            while not event.is_set():
                now = time.time()
                connected_for = now - connected_at
                print(f"has been connected for {connected_for} seconds")
                one_hour = 60 * 60
                if connected_for > one_hour:
                    print("connected for over 1hr; forcing reconnect")
                    break

                code, payload = recv(s)

                if code == ddmActions.PING:
                    ack(s)

                if code == ddmActions.PUBLISH:
                    ack(s)

                    topic = payload[0:4]
                    value = payload[4:]

                    value_type, topic_name = get_value_type(topic)
                    decoded_value = decode(value_type, value)
                    record(decoded_value, topic_name)

                    print(f"{topic_name=} {value_type=} {decoded_value=}")
                    if not topic_name:
                        print(f"unknown topic {topic=} {value=}")
        except TimeoutError as e:
            print("caught timeout error, will retry")
            traceback.print_exc()
            socket_timeouts_count.inc()
        finally:
            print('closing socket')
            s.close()
            # don't delay if we're been asked to exit
            if not event.is_set():
                delay = 5
                print(f'sleeping for {delay} seconds before reconnection')
                time.sleep(delay)


if __name__ == "__main__":
    main()
