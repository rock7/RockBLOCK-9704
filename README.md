# RockBLOCK 9704 Library
RockBLOCK 9704 C and Python Library

# Contents

### Building
- [**Linux**](#linux)
  - Dependencies
  - Build steps
  - Examples
- [**MacOS**](#macos)
  - Dependencies
  - Build steps
  - Examples
- [**Windows**](#windows)
  - Dependencies
  - Build steps
  - Examples
- [**Raspberry Pi**](#raspberry-pi)
  - Dependencies
  - Build steps
  - Examples
  - Hardware setup
- [**Arduino**](#arduino)
  - Build steps
- [**Python**](#python)
  - Dependencies
  - Build steps
  - Examples

# Building

## Linux:

### Dependencies

- CMake >= 3.16
- make
- git
- build-essentials

***Notes*** : ```sudo apt install <dependancy>```

### Build steps

#### Run cmake and compile project

```
    mkdir build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

### Examples (`./examples`)

#### Run executable

```
    ./<examplename> <arguments>

    eg ./reflected -d /dev/ttyUSB0
    eg ./customFileMessage -d /dev/ttyUSB0 -t 244 -f "/path/to/file.txt"
```

## MacOS:

### Dependencies

- CMake >= 3.16
- make
- git

***Notes:*** ```brew install <dependancy>```

### Build steps

#### Run cmake and compile project

```
    mkdir build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

### Examples (`./examples`)

#### Run executable

```
    ./<examplename> <arguments>

    eg ./reflected -d /dev/ttyUSB0
    eg ./customFileMessage -d /dev/ttyUSB0 -t 244 -f "/path/to/file.txt"
```

## Windows:

### Dependencies

- Download and install [Visual Studios](https://visualstudio.microsoft.com/). If using the Python library install [Python Native Development Tools](https://learn.microsoft.com/en-us/visualstudio/python/installing-python-support-in-visual-studio?view=vs-2022) with Visual Studios as well.
- Download and install [CMake](https://cmake.org/download/).
- [git](https://git-scm.com/downloads/win)
- Downloaded and install the [FTDI VCP drivers](https://ftdichip.com/drivers/).

### Build steps


To ensure you are using the right environment, open the Developer Command Prompt for Visual Studio 2022. You can find this in your Start menu under `Visual Studio 2022 > x64 Native Tools Command Prompt for VS 2022` or `x86 Native Tools Command Prompt for VS 2022` is building for 32bit. Alternatively you can run the bat in `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat` or  `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat`.

```
mkdir build
cmake -G "Visual Studio 17 2022" -A <ARCH> -S . -B build
msbuild build\ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=<ARCH>
```
- Replace `<ARCH>` with x64, Win32, ARM64, etc., depending on your architecture.
- You can set /p:Configuration=Release for a release build.

The build outputs will be in `build\Debug\` or `build\Release\` depending on the build configuration used.

### Examples (`./examples`)

#### Run executable

```
    ./<examplename> <arguments>

    eg ./reflected -d <COM PORT>
    eg ./customFileMessage -d <COM PORT> -t 244 -f "path\to\file.txt"
```
Replace `<COM PORT>` for the configured COM port of the RockBLOCK 9704.

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

```
    git clone https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git 
    cd libgpiod
    ./autogen.sh
    ./configure --enable-shared=yes
    make
    sudo make install
```

#### Run cmake and compile project

```
    mkdir build
    cd build
    cmake ..
    (For debug: cmake -DDEBUG=ON ..)
    make
```

### Examples (`./examples`)

#### Run executable

```
    ./<examplename> <arguments>

    eg ./reflected -d /dev/ttyUSB0
    eg ./customFileMessage -d /dev/ttyUSB0 -t 244 -f "/path/to/file.txt"
```

### Hardware setup
- **USB-C**

  - Plug in USB-C cable.
  - Select correct USB port.
- **Pi-Hat**

    ```
    sudo raspi-config
    ```
  - Select "Interface Options".
  - Select "Serial Port".
  - Select **No** to shell over serial.
  - Select **Yes** to serial port hardware enabled.
  - Select "**/dev/ttyS0**" as the USB port.

    ***Notes:***
    - When using the RB9704 PiHat you will need run ```gpioEnablePiHat()``` before doing anything else, this will configure the necessary pins on the Raspberry Pi to enable communication with the modem.
    - After the above has ran successfully the modem might take anywhere between ~1s-40s to boot, if you want your application to check for this use ```gpioListenIridBooted(<timeout>)``` which will block until the modem has booted or the timeout is reached.
    - Alternatively for manual pin manipulation follow these instructions:
        - Drive BCM pin **24 LOW**.
        - Drive BCM pin **16 HIGH**.
        - Monitor BCM pin **23** for **HIGH** transition.

## Arduino 

### Build steps

Download Arduino IDE from: https://www.arduino.cc/en/software/

#### Include library in Arduino IDE

- Copy the library into /<ArduinoInstallationDirectory>/Arduino/libraries
- At the top of your sketch include: ```#include "rockblock_9704.h"```

***Notes:***
- Message size both outgoing and incoming capped at 5kB due to memory constraints.
- Message buffers can be increased manually if sufficient memory exists, these can be increased in **imt_queue.h** under the **IMT_PAYLOAD_SIZE** definition.

## Python
### Dependencies
  - Python  >= 3.8
  - CMake  >= 3.16
 ### Build steps
  ```
  python -m pip install -r requirements.txt
  python -m pip install .
  ```
### Examples (`./examples`)

#### Run Scripts

```
    python <examplename> <arguments>

    eg python receive_message.py --device <serial device> 
    eg python send_message.py --device <serial device> 
```
**Notes:**
- Replace `<serial device>` with the serial instance of your RockBLOCK 9704