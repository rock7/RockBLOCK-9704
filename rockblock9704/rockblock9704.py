import rockblock as _rb


class GpioConfig:

    def __init__(self, power_pin: int, iridium_pin: int, booted_pin: int, default_card: str = None, power_card: str = None, iridium_card: str = None,
                 booted_card: str = None):
        """
        A utility class for storing the GPIO pin config.
        :param power_pin: Power Enable pin ID
        :param iridium_pin: Iridium Enable pin ID
        :param booted_pin: Booted pin ID
        :param default_card: Default GPIO card (i.e. /dev/gpiochip0)
        :param power_card: GPIO card for Power Enable pin to override default
        :param iridium_card: GPIO card for Iridium Enable pin to override default
        :param booted_card: GPIO card for Booted pin to override default
        """
        self.default_card = default_card
        self.power_pin = power_pin
        self.iridium_pin = iridium_pin
        self.booted_pin = booted_pin
        self.power_card = power_card
        self.iridium_card = iridium_card
        self.booted_card = booted_card

    def to_dict(self) -> dict:
        """
        Generates a dictionary representing the GPIO config for use by the C Library
        :return:
        """
        if self.default_card is None and (self.power_card is None or self.iridium_card is None or self.booted_card is None):
            raise Exception("All GPIO card parameters must set if default card is not provided")

        return {
            "powerEnable": (self.power_card if self.power_card is not None else self.default_card, self.power_pin),
            "iridiumEnable": (self.iridium_card if self.iridium_card is not None else self.default_card, self.iridium_pin),
            "booted": (self.booted_card if self.booted_card is not None else self.booted_card, self.booted_pin)
        }


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

    def begin_gpio(self, port: str, gpio_config: GpioConfig, timeout: int) -> bool:
        """
        Drives user defined pin (power enable) low and user defined pin (iridium enable) high to 
        initialise the RB9704 PiHat. Initialise the serial connection in the 
        detected context (or user defined), if successful continue to set 
        the API, SIM & state of the modem in order to be ready for messaging.
        :param port: pointer to port name
        :param gpio_config: structure containing a valid chip & pin for powerEnable, IridiumEnable and booted
        :param timeout: in seconds
        :return: boolean indicating success
        """
        self.connected = _rb.begin_gpio(port, gpio_config.to_dict(), timeout)
        return self.connected

    def end_gpio(self, gpio_config: GpioConfig) -> bool:
        """
        Drives user defined pin (power enable) high and another
        user defined pin (iridium enable) low to deinitialise the RB9704
        PiHat. Uninitialises/closes the serial connection.
        :return: boolean indicating success
        """
        self.connected = not _rb.end_gpio(gpio_config.to_dict())
        return not self.connected

    def send_message(self, message: bytes, topic: int = None, timeout: int = 30) -> bool:
        """
        Sends a message from the RockBLOCK 9704
        :param message: bytes to send
        :param topic: optional topic to send to (defaults to raw topic)
        :param timeout: optional timeout in seconds (defaults to 30s)
        :return: boolean indicating success
        """
        if topic is None:
            return _rb.send_message(message, timeout)
        else:
            return _rb.send_message_any(topic, message, timeout)

    def receive_message(self, topic: int = None) -> bytes:
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
        :return: bool depicting SIM presence
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
