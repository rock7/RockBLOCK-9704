# RockBLOCK 9704
The official RockBLOCK 9704 C and Python Library made by [Ground Control](https://www.groundcontrol.com) 

For more information about RockBLOCK 9704, see: [groundcontrol.com/product/rockblock-9704](https://www.groundcontrol.com/product/rockblock-9704/)

## Contents

### Code
- [**Simple send/receive**](#simple-sendreceive)
  - Script
  - Result
- [**Examples**](#examples)
  - C
  - Python
### Building
- [**Python**](#python)
  - Dependencies
  - Build steps
- [**Linux**](#linux)
  - Dependencies
  - Build steps
- [**MacOS**](#macos)
  - Dependencies
  - Build steps
- [**Windows**](#windows)
  - Dependencies
  - Build steps
- [**Raspberry Pi**](#raspberry-pi)
  - Dependencies
  - Build steps
  - Hardware setup
- [**Arduino**](#arduino)
  - Build steps
  - Compatible models
- [**Python**](#python)
  - Dependencies
  - Build steps

# Code

## Simple send/receive:

### Script

```c
#include "rockblock_9704.h"

int main()
{
    const char *message = "Hello World!";
    char * mtBuffer = NULL;

    //Begin serial connection and initialise the modem
    if(rbBegin("/dev/ttyUSB2"))
    {
        //Queue and send MO
        if(sendMessageAny(80, message, strlen(message), 600))
        {
            printf("Sent message: %s\r\n", message);
            //Start listening for MT
            while(true)
            {
                const size_t mtLength = receiveMessage(&mtBuffer);
                if ((mtLength > 0) && (mtBuffer != NULL))
                {
                    printf("Received message: %s\r\n", mtBuffer);
                    break; //Break out of loop if MT is found
                }
                usleep(100000);
            }
            //End serial connection
            if(rbEnd())
            {
                printf("Ended connection successfully\r\n");
            }
        }
        else
        {
            printf("Failed to queue message\r\n");
        }
    }
    else
    {
        printf("Failed to begin the serial connection\r\n");
    }
}
```

### Result

```bash
linux@laptop:~/Documents/RockBLOCK-9704/build$ ./simpleSendReceive
Sent message: Hello World!
Received message: Hello World!
Ended connection successfully
```

## Examples:

### C

#### Run executable

```c
    ./<examplename> <arguments>

    eg ./reflected -d <serial device>
    eg ./customFileMessage -d <serial device> -t 244 -f "/path/to/file.txt"
```
***Notes:***
- Replace `<serial device>` with the serial instance of your RockBLOCK 9704
- When running these examples on windows make sure to use the configured COM port of the RockBLOCK 9704 as your ```-d``` argument.

### Python

#### Run Scripts

```python
    python <examplename> <arguments>

    eg python receive_message.py --device <serial device> 
    eg python send_message.py --device <serial device> 
```
***Notes:***
- Replace `<serial device>` with the serial instance of your RockBLOCK 9704

# Building

## Python 🐍

The Python library can be installed from [PyPI](https://pypi.org/project/rockblock9704/), building from source is not required in most situations.

```bash
pip install rockblock9704
```

Build instructions are included below for those not using PyPI.

### Dependencies
  - Python  >= 3.8
  - CMake  >= 3.16
 ### Build steps
  ```bash
  python -m pip install -r requirements.txt
  python -m pip install .
  ```

## Linux:

### Dependencies

- CMake >= 3.16
- make
- git
- build-essentials

***Notes*** : ```sudo apt install <dependancy>```

### Build steps

#### Run cmake and compile project

```bash
    mkdir -p build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

## MacOS:

### Dependencies

- CMake >= 3.16
- make
- git

***Notes:*** ```brew install <dependancy>```

### Build steps

#### Run cmake and compile project

```bash
    mkdir -p build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

## Windows:

### Dependencies

- Download and install [Visual Studios](https://visualstudio.microsoft.com/). If using the Python library install [Python Native Development Tools](https://learn.microsoft.com/en-us/visualstudio/python/installing-python-support-in-visual-studio?view=vs-2022) with Visual Studios as well.
- Download and install [CMake](https://cmake.org/download/).
- [git](https://git-scm.com/downloads/win)
- Downloaded and install the [FTDI VCP drivers](https://ftdichip.com/drivers/).

### Build steps


To ensure you are using the right environment, open the Developer Command Prompt for Visual Studio 2022. You can find this in your Start menu under `Visual Studio 2022 > x64 Native Tools Command Prompt for VS 2022` or `x86 Native Tools Command Prompt for VS 2022` is building for 32bit. Alternatively you can run the bat in `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat` or  `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat`.

```bash
mkdir -p build
cmake -G "Visual Studio 17 2022" -A <ARCH> -S . -B build
msbuild build\ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=<ARCH>
```
- Replace `<ARCH>` with x64, Win32, ARM64, etc., depending on your architecture.
- You can set /p:Configuration=Release for a release build.

The build outputs will be in `build\Debug\` or `build\Release\` depending on the build configuration used.

## Raspberry Pi:

### Dependencies

- CMake >= 3.16
- make
- git
- build-essentials
- libgpiod >= 3.1.1 ***note: This may need to be installed from source to get the appropriate version, steps to do this are outlined below. Only required if you need pin manipulation.*** 

***Notes:*** ```sudo apt install <dependancy>```

### Build steps

#### Install libgpiod *Optional

```bash
    git clone https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git 
    cd libgpiod
    ./autogen.sh
    ./configure --enable-shared=yes
    make
    sudo make install
```

#### Run cmake and compile project

```bash
    mkdir -p build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

### Hardware setup
- **USB-C**

  - Plug in USB-C cable.
  - Select correct USB port.
- **Pi-Hat**

    ```bash
    sudo raspi-config
    ```
  - Select "Interface Options".
  - Select "Serial Port".
  - Select **No** to shell over serial.
  - Select **Yes** to serial port hardware enabled.
  - Use "**/dev/ttyS0**" as your port.

    ***Notes:***
    - When using the RB9704 on a raspberry pi via **GPIO** you will need to use ```rbBeginGpio(char * port, const rbGpioTable_t * gpioInfo, const int timeout)```.
    - The minimum following connections will need to be made from the RB9704 to the Pi:
      - GND -> GND
      - VIN -> VIN
      - TX (13) -> RX (14)
      - RX (14) -> TX (15)
      - P_EN (6) -> Any free GPIO (Recommended: 24)
      - I_EN (3) -> Any free GPIO (Recommended: 16)
      - I_BTD (7) -> Any free GPIO (Recommended: 23)
    - Here's a rbGpioTable_t structure example if the recommended pins above were used:
    ```c
    #define CHIP_NAME "/dev/gpiochip0"
    #define POWER_ENABLE_PIN 24U
    #define IRIDIUM_ENABLE_PIN 16U
    #define IRIDIUM_BOOTED_PIN 23U

    const rbGpioTable_t customGpioTable = 
    {
        { CHIP_NAME, POWER_ENABLE_PIN},
        { CHIP_NAME, IRIDIUM_ENABLE_PIN},
        { CHIP_NAME, IRIDIUM_BOOTED_PIN}
    };
    ```
    - Use ```rbEndGpio(const rbGpioTable_t * gpioInfo)``` with the same structure used to begin to end the serial connection and shutdown the RB9704.

## Arduino 

### Build steps

Download Arduino IDE from: https://www.arduino.cc/en/software/

#### Include library in Arduino IDE

- Copy the library into /ArduinoInstallationDirectory/Arduino/libraries
- At the top of your sketch include: ```#include "rockblock_9704.h"```

***Notes:***
- Message size both outgoing and incoming limited to 2-5kb (depending on model) due to RAM memory constraints.
- Message buffers can be increased manually if sufficient memory exists, these can be increased in **imt_queue.h** under the **IMT_PAYLOAD_SIZE** definition.

### Compatible models
***Notes:***
- This library has been designed to work with most Arduino models, the list below shows models that have been **tested** and proven to work. Therefore other **untested** models may or may not be currently compatible.

**List of tested working models:**
- MKR 1010 (**Note:** Message size limit ~5kb)
- UNO R4 (**Note:** Message size limit ~2kb)
