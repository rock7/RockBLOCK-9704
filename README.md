# 📡 RockBLOCK 9704

[![Build](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/rock7/rockblock-9704/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![PyPI](https://img.shields.io/pypi/v/rockblock9704)](https://pypi.org/project/rockblock9704/)
[![arduino-library-badge](https://www.ardu-badge.com/badge/RockBLOCK-9704.svg)](https://docs.arduino.cc/libraries/rockblock-9704/)

Official C and Python libraries for the [RockBLOCK 9704](https://www.groundcontrol.com/product/rockblock-9704/) satellite modem, maintained by [Ground Control](https://www.groundcontrol.com).

Further documentation for hardware and specifications can be found on our document site [here](https://docs.groundcontrol.com/iot/rockblock-9704).


---

## 📋 Table of Contents

- [📖 About](#-about)
- [🚀 Quick Start](#-quick-start)
  - [📜 High Level API Calls](#-high-level-api-calls)
  - [Simple Send/Receive Example](#simple-sendreceive-example)
- [💡 Examples](#-examples)
- [🛠️ Building from Source](#%EF%B8%8F-building-from-source)
  - [🐍 Python](#-python)
  - [🐧 Linux](#-linux)
  - [🍏 macOS](#-macos)
  - [🖥️ Windows](#%EF%B8%8F-windows)
  - [🥧 Raspberry Pi](#-raspberry-pi)
  - [🤖 Arduino](#-arduino)
- [🔌 Hardware Setup](#-hardware-setup)
- [📖 Kick-start your own project](#-kick-start-your-own-project)
  - [🪜 Kick-start step-by-step guide](#-kick-start-step-by-step-guide)
  - [⚙️ Example `CMakeLists.txt`](#%EF%B8%8F-example-cmakeliststxt)
- [📚 Using the Library](#-using-the-library)
  - [📡 Antenna Placement and Signal Quality](#-antenna-placement-and-signal-quality)
  - [⚠️ Blocking Behavior](#%EF%B8%8F-blocking-behavior)
  - [⬆️ Sending Mobile-Originated (MO) Messages](#%EF%B8%8F-sending-mobile-originated-mo-messages)
  - [⬇️ Receiving Mobile-Terminated (MT) Messages](#%EF%B8%8F-receiving-mobile-terminated-mt-messages)
- [❓ Frequently Imagined Questions (FIQ)](#-frequently-imagined-questions-fiq)
- [⚖️ License](#%EF%B8%8F-license)

---

## 📖 About

This project provides easy-to-use C and Python libraries to send and receive data over the Iridium satellite network using the RockBLOCK 9704.

- **Official library maintained by Ground Control**
- **Supports** Linux, macOS, Windows, Raspberry Pi, Arduino
- **Available on PyPI** for easy Python installation: [rockblock9704](https://pypi.org/project/rockblock9704/)

---

### 📜 High Level API Calls

The high level API calls can be seen in the generated Doxygen documentation [here](https://rock7.github.io/RockBLOCK-9704/) or the `rockblock_9704.h` header file.

## 🚀 Quick Start

### Simple Send/Receive Example

***Note:*** In this example we send a message to the cloudloop RAW topic then use cloudloop to send a message back to the RB9704.

**Requirements for below example:**
- Your RB9704 needs to be provisioned for the cloudloop RAW topic (244).
- Have a good view of the sky so that signal can be obtained.

```c
#include <stdio.h>
#include "rockblock_9704.h"

int main(void)
{
    const char *message = "Ground Control to Major Tom...";
    char * mtBuffer = NULL;

    if (rbBegin("/dev/ttyUSB2"))
    {
        if (rbSendMessage(message, strlen(message), 600))
        {
            printf("Sent message: %s\r\n", message);
            while (true)
            {
                size_t mtLength = rbReceiveMessage(&mtBuffer);
                if ((mtLength > 0) && (mtBuffer != NULL))
                {
                    printf("Received message: %s\r\n", mtBuffer);
                    break;
                }
                usleep(100000);
            }
            rbEnd();
        }
        else
        {
            printf("Failed to queue message\r\n");
        }
    }
    else
    {
        printf("Failed to begin serial connection\r\n");
    }

    return 0;
}
```

#### Example Output

```bash
linux@laptop:~/Documents/RockBLOCK-9704/build$ ./simpleSendReceive
Sent message: Ground Control to Major Tom...
Received message: Ground Control to Major Tom...
Ended connection successfully
```

---

## 💡 Examples

### C Examples

```bash
./<example_name> <arguments>

e.g., ./reflected -d <serial device>
e.g., ./customFileMessage -d <serial device> -t 244 -f "/path/to/file.txt"
```

### Python Examples

```bash
python <example_name>.py --device <serial device>

e.g., python receive_message.py --device /dev/ttyUSB2
e.g., python send_message.py --device /dev/ttyUSB2
```

---

## 🛠️ Building from Source

> **Tip:** If you're using Python, install directly from PyPI:
>
> ```bash
> pip install rockblock9704
> ```

---

### 🐍 Python

**Dependencies:**

- Download and install [Python](https://www.python.org/downloads/)  >= 3.8.
- Download and install [CMake](https://cmake.org/download/) >= 3.16.

**Build:**

```bash
python -m pip install -r requirements.txt
python -m pip install .
```

---

### 🐧 Linux

**Dependencies:**

- Download and install [CMake](https://cmake.org/download/) >= 3.16.
- [git](https://git-scm.com/downloads/linux).
- build-essentials.

Install dependencies:

```bash
sudo apt install cmake git build-essential
```

**Build Steps:**

```bash
mkdir build
cd build
cmake ..
make
```

*(For debug builds use **`cmake -DDEBUG=ON ..`**)*

---

### 🍏 macOS

**Dependencies:**

- Download and install [CMake](https://cmake.org/download/) >= 3.16.
- make
- [git](https://git-scm.com/downloads/mac).

Install dependencies:

```bash
brew install cmake make git
```

**Build Steps:**

```bash
mkdir build
cd build
cmake ..
make
```

---

### 🖥️ Windows

**Dependencies:**

- [Visual Studio](https://visualstudio.microsoft.com/) (C++ and Python Development Tools)
- [CMake](https://cmake.org/download/)
- [Git for Windows](https://git-scm.com/downloads/win)
- [FTDI VCP drivers](https://ftdichip.com/drivers/)

**Build Steps:**

```bash
mkdir build
cmake -G "Visual Studio 17 2022" -A <ARCH> -S . -B build
msbuild build\ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=<ARCH>
```

Replace `<ARCH>` with `x64`, `Win32`, `ARM64`, etc.

---

### 🥧 Raspberry Pi

**Dependencies:**

- Download and install [CMake](https://cmake.org/download/) >= 3.16.
- [git](https://git-scm.com/downloads/linux).
- build-essentials.
- (Optional) [libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git) >= 3.1.1 for GPIO pin control.

**Install libgpiod:**

```bash
git clone https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git
cd libgpiod
./autogen.sh
./configure --enable-shared=yes
make
sudo make install
```

**Build Steps:**

```bash
mkdir build
cd build
cmake ..
make
```

---

### 🤖 Arduino

**Getting Started:**

- Download [Arduino IDE](https://www.arduino.cc/en/software/)
- Install `RockBLOCK-9704` from the IDE library manager

```cpp
#include "rockblock_9704.h"
```

When using the Arduino send/receive [example](examples/ArduinoBasicSendandReceive/ArduinoBasicSendandReceive.ino) you may need to specify the GPIO pins to 
be used by `Serial1`.

e.g. if using pins 11 (Rx) and 12 (Tx)

```cpp
Serial1.begin(230400, SERIAL_8N1, D11, D12);
```

**Notes:**

- Message size: ~2–5 KB depending on Arduino model.
- Buffers adjustable in `imt_queue.h` via `IMT_PAYLOAD_SIZE`.

**Tested Boards:**

- MKR 1010 (~5KB limit).
- UNO R4 (~2KB limit).
- NANO ESP32 (~5KB limit).
---

## 🔌 Hardware Setup

### Raspberry Pi — GPIO Mode

**Minimal Wiring:**

| RockBLOCK Pin | Raspberry Pi Pin |
| ------------- | ---------------- |
| GND           | GND              |
| VIN           | VIN              |
| TX (13)       | RX (14)          |
| RX (14)       | TX (15)          |
| P_EN (6)      | GPIO 24          |
| I_EN (3)      | GPIO 16          |
| I_BTD (7)     | GPIO 23          |

**Enable Serial Port:**

```bash
sudo raspi-config
# Interface Options -> Serial Port -> Disable shell, enable serial hardware
```

**Example GPIO config in C:**

```c
#define CHIP_NAME "/dev/gpiochip0"
#define POWER_ENABLE_PIN 24U
#define IRIDIUM_ENABLE_PIN 16U
#define IRIDIUM_BOOTED_PIN 23U

const rbGpioTable_t customGpioTable =
{
    { CHIP_NAME, POWER_ENABLE_PIN },
    { CHIP_NAME, IRIDIUM_ENABLE_PIN },
    { CHIP_NAME, IRIDIUM_BOOTED_PIN }
};
```

---

## 📖 Kick-start your own project

**Dependencies:**

For dependencies refer to your specific target under the [🛠️ Building from Source](#%EF%B8%8F-building-from-source) section.

**Pre-requisites**

These steps will assume you have created a blank repository using git.

### 🪜 Kick-start step-by-step guide
1. In your blank repository, add this library as a submodule:
```
git submodule add https://github.com/rock7/RockBLOCK-9704.git
```

2. Change into the `RockBLOCK-9704` directory and (optionally) checkout the latest version or a specific branch using standard Git commands. In this guide, we'll use the latest tip of the default branch.

3. Create a new `CMakeLists.txt` for your project like this:
```
cmake_minimum_required(VERSION 3.16)
project(MyAwesomeProject)

# This will provide  RB9704_LIB for linking and RB9704_INCLUDES for including
add_subdirectory("RockBLOCK-9704")
```
The key line here is `add_subdirectory("RockBLOCK-9704")`, which brings in the required sources so your project can link against them.

4. Create your project source files. To get started, you can use the [Simple Send/Receive Example](#simple-sendreceive-example) above. In this guide we will assume you have one source file called `main.c`.

5. Now define your executable in `CMakeLists.txt`:
```
add_executable(${CMAKE_PROJECT_NAME} main.c)
```
To build the project, you will also need to link the library and include the necessary headers. Add the following two lines:
```
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${RB9704_INCLUDES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${RB9704_LIB})
```

6. To build follow the steps for your specific target under the [🛠️ Building from Source](#%EF%B8%8F-building-from-source) section.

### ⚙️ Example `CMakeLists.txt`
```
cmake_minimum_required(VERSION 3.16)
project(MyAwesomeProject)

# This will provide  RB9704_LIB for linking and RB9704_INCLUDES for including
add_subdirectory("RockBLOCK-9704")

add_executable(${CMAKE_PROJECT_NAME} main.c)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${RB9704_INCLUDES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${RB9704_LIB})

```

---

## 📚 Using the Library

This library provides a simple blocking API for communicating with the RockBLOCK 9704 modem over serial. Here are some important considerations when integrating it into your application:

### 📡 Antenna Placement and Signal Quality

#### **Clear Sky Visibility Required**  
  The RockBLOCK 9704 modem requires a **clear view of the sky** to reliably connect to Iridium satellites.

#### **Before Communication**  
  Always **check signal strength** before attempting to send or receive data. Poor placement (e.g., indoors, under cover, or near obstructions) can cause timeouts or message failures.

#### **Recommended Practice**  
  Ensure the antenna has **an unobstructed view of the sky** during operation to improve signal strength and reduce message latency.

### ⚠️ Blocking Behavior

#### **Synchronous Operations**  
  The library is **not currently asynchronous**. All functions block until the modem responds or a timeout occurs.

#### **Most Operations**  
  Most API calls involve sending and receiving small command sequences over serial at **230400 baud**, and generally return quickly. Exceptions for sending and receiving messages explained below.

### ⬆️ Sending Mobile-Originated (MO) Messages

#### **Blocking Transmit**
  Sending an full payload MO message (up to 100 kB + 2 B CRC) may take **around 2 minutes** under good conditions. In areas with **poor signal**, this could take longer. **Use timeouts of several minutes**, not seconds.

  Only transmit when:
  - You have good signal and a clear view of the sky
  - You handle it in a way blocking does not impact your the main application flow.

### ⬇️ Receiving Mobile-Terminated (MT) Messages

#### **Non-Blocking Receive**
  Calling the listen function will not block until a message is received, it can timeout listening for unsolicited MT message, so it is advised to wrap this call in receive loop/thread, refer to our examples showing this.

  Only call listen when:
  - You have good signal and a clear view of the sky
  - You handle it in a way you keep calling it until you have a message.

---

## ❓ Frequently Imagined Questions (FIQ)

> **Q:** Can I send memes to space?
> 
> **A:** Technically yes, if you compress them enough.
>
> **Q:** How fast is it?
>
> **A:** About as fast as a satellite flying over your head at Mach 20.
>
> **Q:** Can it survive being strapped to a rocket?
>
> **A:** The modem, yes. Your nerves, maybe not.

---

## ⚖️ License

This project is maintained by [Ground Control](https://www.groundcontrol.com) and is open-source under an MIT-style license.

---
