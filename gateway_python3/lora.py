import platform
import serial
import asyncio

class LoRa:
    def __init__(self, uart_port: str, baudrate: int = 115200):
        self.__serial = serial.Serial(
            port=uart_port,
            baudrate=baudrate, timeout=1
        )

        self.__queue_send = []
        self.__queu_receive = []


    def __del__(self):
        self.__serial.close()
        self.__queu_receive.clear()
        self.__queue_send.clear()


    async def send_message(self, msg: str) -> int:
        return self.__serial.write(msg.encode("unicode_escape"))


    async def receive_message(self) -> str:
        return self.__serial.readline().decode("unicode_escape")


    @property
    def pop_from_queu_receive(self) -> str:
        if not self.__queu_receive:
            return ""
        return self.__queu_receive.pop(0)


    def push_to_queue_send(self, msg: str):
        if msg != "":
            self.__queue_send.append(msg)


    async def loop_forever(self):

        print("---| LoRa Gateway run on |---")
        print(platform.system(), platform.architecture())
        print(platform.processor())
        print("LoRa port:", self.__serial.port)
        print("LoRa baudrate:", self.__serial.baudrate)
        print("------------------------------")

        msg = ""
        nbyte = 0   

        try:
            
            while True:
                
                msg = (await self.receive_message())
                if msg != "":
                    self.__queu_receive.append(msg)
                    print("LoRa receive a message: ", msg)
                
                if not not self.__queue_send:
                    msg = self.__queue_send.pop(0)
                    nbyte = (await self.send_message(msg))
                    print("LoRa sent a message: ", msg, f"{nbyte}bytes")

                await asyncio.sleep(0.2)

        except Exception as e:
            print("LoRa ERROR! loop_forever")
            print(str(e))
