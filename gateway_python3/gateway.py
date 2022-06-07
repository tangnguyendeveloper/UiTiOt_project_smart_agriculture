import json
from lora import LoRa
from mqtt_client import MQTTClient, TopPic
import asyncio

lora = None
mqtt = None
topic_ls = TopPic()


def on_connect(client, userdata, flags, rc):
    print("MQTTClient Connected with resuit code ", str(rc))
    try:
        for topic in topic_ls.topic_subscribe:
            client.subscribe(topic)
            print(" - MQTTClient subscribe topic:", topic)
    except Exception as e:
        print("SUBSCRIBE ERROR", str(e))


def on_message(client, userdata, msg):
    print("topic", msg.topic, "payload", msg.payload)
    
    try:
        if msg.payload == b"1":
            lora.push_to_queue_send("motor|1\n")
            mqtt.publish(topic_ls.topic_publish["NOTIFY"], 1)
        if msg.payload == b"0":
            lora.push_to_queue_send("motor|0\n")
            mqtt.publish(topic_ls.topic_publish["NOTIFY"], 0)

    except Exception as e:
        print("MESSAGE ERROR", str(e))


def load_configure() -> dict:
    js_conf = None
    with open("gateway_config.json") as f_js:
        js_conf = json.load(f_js)
    return js_conf


def setup() -> None:
    global lora, mqtt, topic_ls

    js_conf = load_configure()

    lora = LoRa(js_conf["lora_port"], js_conf["lora_baudrate"])
    mqtt = MQTTClient(js_conf["mqtt_server"], js_conf["mqtt_port"])
    mqtt.set_callbacks(on_connect=on_connect, on_message=on_message)
    
    topic_ls.topic_publish = js_conf["publish_topic"]
    topic_ls.topic_subscribe = js_conf["subscribe_topic"]


async def publish_message(msg: list) -> None:

    topic = ""
    value = 0.0
    for item in msg:
        try:
            tmp = item.split(":")
            topic = topic_ls.topic_publish[tmp[0]]
            value = float(tmp[1])
            mqtt.publish(topic, value)
        except Exception as e:
            print("PUBLISH MESSAGE ERROR", str(e))


async def process_data() -> None:
    msg = ""

    try:
        while True:
            msg = lora.pop_from_queu_receive
            if msg != None:
                msg = msg.split("|")
                await publish_message(msg)
            await asyncio.sleep(0.1)

    except Exception as e:
        print("PROCESS DATA ERROR", str(e))


async def loop() -> None:
    task_lora_connect = asyncio.create_task(lora.loop_forever())
    task_mqtt_connect = asyncio.create_task(mqtt.connect_to_broker_forever())
    task_process_data = asyncio.create_task(process_data())

    await task_lora_connect
    await task_mqtt_connect
    await task_process_data


if __name__ == "__main__":
    
    loop = asyncio.get_event_loop()
    setup()
    loop.loop.run_until_complete(loop())
