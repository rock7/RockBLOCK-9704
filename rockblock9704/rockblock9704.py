import rockblock as _rb


class RockBlock9704:

    RAW_TOPIC = 244
    PURPLE_TOPIC = 313
    PINK_TOPIC = 314
    RED_TOPIC = 315
    ORANGE_TOPIC = 316
    YELLOW_TOPIC = 317

    def __init__(self):
        self.connected = False

    def get_signal(self) -> int:
        """
        Checks the modem's current signal to the Iridium IMT constellation
        :return: 0 - 5 going from no signal to full
        """
        return _rb.get_signal()

    def begin(self, port: str) -> bool:
        """
        Initiates the serial connection to the RockBLOCK 9704
        :param port: address of the serial port
        :param baud: optional baud (recommend staying with default)
        :return: boolean indicating success
        """
        self.connected = _rb.begin(port)
        return self.connected

    def end(self) -> bool:
        """
        Terminates the serial connection to the RockBLOCK 9704
        :return: boolean indicating success
        """
        self.connected = not _rb.end()
        return not self.connected
    
    def beginHat(self, timeout: int) -> bool:
        """
        Drives pin 24 (power enable) low and pin 16
        (iridium enable) high to initialise the RB9704
        PiHat. Initialises the the serial connection in the detected context 
        (or user defined), if successful continue to set the API, SIM & 
        state of the modem in order to be ready for messaging.
        :param timeout: in seconds
        :return: boolean indicating success
        """
        self.connected = _rb.beginHat(timeout)
        return self.connected

    def endHat(self) -> bool:
        """
        Drives pin 24 (power enable) high and pin 16
        (iridium enable) low to deinitialise the RB9704
        PiHat. Uninitialises/closes the the serial connection.
        :return: boolean indicating success
        """
        self.connected = not _rb.endHat()
        return not self.connected
    
    def beginGpio(self, port: str, gpioDict: dict, timeout: int) -> bool:
        """
        Drives user defined pin (power enable) low and user defined pin (iridium enable) high to 
        initialise the RB9704 PiHat. Initialise the serial connection in the 
        detected context (or user defined), if successful continue to set 
        the API, SIM & state of the modem in order to be ready for messaging.
        :param port pointer to port name
        :param gpioInfo structure containing a valid chip & pin for powerEnable, IridiumEnable and booted
        :param timeout in seconds
        :return: boolean indicating success
        """
        self.connected = _rb.beginGpio(port, gpioDict, timeout)
        return self.connected

    def endGpio(self, gpioDict: dict) -> bool:
        """
        Drives user defined pin (power enable) high and another
        user defined pin (iridium enable) low to deinitialise the RB9704
        PiHat. Uninitialises/closes the the serial connection.
        :return: boolean indicating success
        """
        self.connected = not _rb.endGpio(gpioDict)
        return not self.connected

    def send_message(self, message: bytes, topic: int=None) -> bool:
        """
        Sends a message from the RockBLOCK 9704
        :param message: bytes to send
        :param topic: optional topic to send to (defaults to raw topic)
        :return: boolean indicating success
        """
        if topic is None:
            return _rb.send_message(message)
        else:
            return _rb.send_message_any(topic, message)

    def receive_message(self, topic: int=None) -> bytes:
        """
        Check for messages sent to the RockBLOCK 9704
        :param topic: optional to only get messages sent to this topic
        :return: byte string message
        """
        if topic is None:
            return _rb.receive_message()
        else:
            return _rb.receive_message_with_topic(topic)

    def get_hardware_version(self) -> str:
        """
        Get RockBLOCK 9704 hardware version
        :return: version string
        """
        return _rb.get_hardware_version()

    def get_serial_number(self) -> str:
        """
        Get RockBLOCK 9704 serial number
        :return: serial string
        """
        return _rb.get_serial_number()

    def get_imei(self) -> str:
        """
        Get modem IMEI
        :return: IMEI string
        """
        return _rb.get_imei()

    def get_board_temp(self) -> int:
        """
        Get temperature of the RockBLOCK 9704
        :return: Temperature in Degrees Celsius
        """
        return _rb.get_board_temp()

    def get_card_present(self) -> bool:
        """
        Checks if a SIM card is present
        "return: bool depicting SIM presence
        """
        return _rb.get_card_present()

    def get_sim_connected(self) -> bool:
        """
        Check if SIM card is present, communicating properly with, and has presented no errors in SIM transactions with the transceiver.
        :return: bool depicting SIM communicating correctly
        """
        return _rb.get_sim_connected()

    def get_iccid(self) -> str:
        """
        Get ICCID of SIM
        :return: ICCID string
        """
        return _rb.get_iccid()

    def get_firmware_version(self) -> str:
        """
        Get the Iridium modem firmware version as vX.Y.X with X being the major number, Y being the minor number and X being the patch number
        :return: firmware version string
        """
        return _rb.get_firmware_version()
