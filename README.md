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
  - [⚠️ Blocking Behavior (Sync)](#%EF%B8%8F-blocking-behavior-sync)
    - [⬆️ Sending Mobile-Originated (MO) Messages (Sync)](#%EF%B8%8F-sending-mobile-originated-mo-messages-sync)
    - [⬇️ Receiving Mobile-Terminated (MT) Messages (Sync)](#%EF%B8%8F-receiving-mobile-terminated-mt-messages-sync)
  - [⚠️ Non-blocking Approach (Async)](#%EF%B8%8F-non-blocking-approach-async)
    - [📞 Callbacks](#-callbacks)
    - [⬆️ Sending Mobile-Originated (MO) Messages (Async)](#%EF%B8%8F-sending-mobile-originated-mo-messages-async)
    - [⬇️ Receiving Mobile-Terminated (MT) Messages (Async)](#%EF%B8%8F-receiving-mobile-terminated-mt-messages-async)
  - [↗️ Adjusting library size (Queue & Payload)](#%EF%B8%8F-adjusting-library-size)
- [❓ Frequently Imagined Questions (FIQ)](#-frequently-imagined-questions-fiq)
- [❓ Frequently Asked Questions (FAQ)](#-frequently-asked-questions-faq)
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

***Note:*** In this example we send a message to the Cloudloop RAW topic then use Cloudloop to send a message back to the RB9704.

**Requirements for below example:**
- Your RB9704 needs to be active on a plan and have picked up its provisioning information. 

ℹ️ Note that the initial provisioning or a change requires the RB9704 being powered with a good view of the sky for 10 - 30 minutes. This will allow it to collect its provisioning information including the available IMT topics.
 
- Your RB9704 needs to be provisioned for the Cloudloop RAW topic (244) as this example uses `rbSendMessage` to send to the default topic.
- Have a good view of the sky so that a stable signal can be obtained.

```c
#include <stdio.h>
#include "rockblock_9704.h"

int main(void)
{
    const char *message = "Ground Control to Major Tom...";
    char * mtBuffer = NULL;

    if (rbBegin("/dev/ttyUSB2"))
    {
        usleep(100000);
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
                usleep(10000);
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

e.g., ./cloudloopRaw -d <serial device>
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

**⚠️IMPORTANT⚠️**

This library requires sufficient RAM memory (~230kB) which a lot of Arduino models lack on board. Please find below a table of popular Arduino models with our recommendations. Refer to our [↗️ Adjusting library size (Queue & Payload)](#%EF%B8%8F-adjusting-library-size) section to see how you can reduce this memory limit (affecting functionality) if need be. Our recommendation for low-memory systems is to reduce the value of `IMT_PAYLOAD_SIZE` to whatever you expect your largest message size to be eg. Adjust value to `100U` if you're not planning on sending / receiving a message bigger than 100 Bytes, for this configuration it should reduce memory usage down to ~20kB (size of library + 2x 100 Byte buffers).

Please also note that by default all Arduino models compile with `5000U` as a default for `IMT_PAYLOAD_SIZE`, meaning that unless changed, the maximum message size will be 5000 Bytes, this is to accommodate a large number of models with low RAM capacity. As already stated, if you wish to decreased or increase this limit from 0 - 100kB, follow the instructions linked above.

**Getting Started:**

- Download [Arduino IDE](https://www.arduino.cc/en/software/)
- Install `RockBLOCK-9704` from the IDE library manager

```cpp
#include "rockblock_9704.h"
```

When using the Arduino send/receive [example](examples/ArduinoBasicSendandReceive/ArduinoBasicSendandReceive.ino) you may need to specify the GPIO pins to be used by `Serial1`.

e.g. if using pins 11 (Rx) and 12 (Tx)

```cpp
Serial1.begin(230400, SERIAL_8N1, D11, D12);
```

**Notes:**

- Message size: ~2–100 kB depending on Arduino model.
- Buffers adjustable in `imt_queue.h` via `IMT_PAYLOAD_SIZE`. (Refer to [↗️ Adjusting library size (Queue & Payload)](#%EF%B8%8F-adjusting-library-size))

**Reviewed Boards:**

| Arduino                     | RAM (kB)         | Extra RAM recommended? | ~ Max `IMT_PAYLOAD_SIZE` Size (kB) | Recommended? | Tested? |
| --------------------------- | ---------------- | ---------------------- | ---------------------------------- |  ----------- | ------- |
|Arduino Uno R3               |2                 |-                       |-                                   |NO            |YES      |
|Arduino Leonardo             |2.5               |-                       |-                                   |NO            |YES      |
|Arduino Nano                 |2                 |-                       |-                                   |NO            |YES      |
|Arduino Nano Every           |6                 |-                       |-                                   |NO            |YES      |
|Arduino Mega 2560 R3         |8                 |-                       |-                                   |NO            |YES      |
|Arduino Giga R1 Wifi         |1024              |NO                      |100                                 |YES           |YES      |
|Arduino Zero                 |32                |YES                     |~7                                  |YES           |YES      |
|Arduino MKR Wifi 1010 / Zero*|32                |YES                     |~0.5                                |YES           |YES      |
|Arduino Uno R4               |32                |YES                     |~2                                  |YES           |YES      |
|Arduino Portenta C33         |512               |NO                      |100                                 |YES           |YES      |
|Arduino Portenta H7          |1024 + 8192       |NO                      |100                                 |YES           |YES      |
|Nano ESP32                   |512               |YES                     |~5                                  |YES           |YES      |
|Raspberry Pi Pico            |264               |NO                      |100                                 |YES           |YES      |
|Raspberry Pi Pico 2          |520               |NO                      |100                                 |YES           |YES      |

**⚠️IMPORTANT⚠️**

The tested devices listed above have all only been checked at compilation level via the Arduino IDE, please note that the max `IMT_PAYLOAD_SIZE` size may vary depending on user code and as the library is updated in the future. If at any point your Arduino code behaves unexpectedly (crashes, freezing etc) we recommend adjusting `IMT_PAYLOAD_SIZE` to a small figure (eg. 50U, 100U etc) and working your way up to find your current message size limit.

To prevent this all together we **highly recommend** using devices with a 200kB or higher RAM supply.

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

### ⚠️ Blocking Behavior (Sync)

#### **Synchronous Operations**  
  All default send / receive functions block until the modem responds or a timeout occurs.

#### **Most Operations**  
  Most API calls involve sending and receiving small command sequences over serial at **230400 baud**, and generally return quickly. Exceptions for sending and receiving messages explained below.

### ⬆️ Sending Mobile-Originated (MO) Messages (Sync)

#### **Blocking Transmit**
  Sending a full payload MO message (up to 100 kB + 2 B CRC) may take **around 2 minutes** under good conditions. In areas with **poor signal**, this could take longer. **Use timeouts of several minutes**, not seconds.

  Only transmit when:
  - You have good signal and a clear view of the sky
  - You handle it in a way blocking does not impact your the main application flow.

### ⬇️ Receiving Mobile-Terminated (MT) Messages (Sync)

#### **Non-Blocking Receive**
  Calling the receive function will not block until a message is received, it can timeout listening for unsolicited MT message, so it is advised to wrap this call in receive loop/thread, refer to our examples showing this. It's important to note that due to the non-blocking nature of the receive function it needs to called very frequently, in our examples 10ms is sufficient, failing to call this function frequently will result in missed messages.

  **NOTE:** When using other library functions in a receive loop, **MAKE SURE** you execute the receiving function at the top of the loop, before executing any other library functions. Not following this will cause **MISSING MESSAGES.**

  Only call receive when:
  - You have good signal and a clear view of the sky
  - You handle it in a way you keep calling it until you have a message.

### ⚠️ Non-blocking Approach (Async)

#### **Asynchronous Operations**  
  To use the library asynchronously and in a non-blocking manner use `rbPoll()` alongside any functions ending in `Async`, the details regarding these functions are explained below.

#### **rbPoll()**  
  This function is responsible for all the messaging communication to and from the modem which normally blocks in the default functions. It needs to be called **very frequently**, at most every **50ms**, for reliability we recommend keeping that number as low as you can get it.

#### **Warnings**
  - Don't call any functions that aren't labeled with **Async** while `rbPoll()` is running.
  - Any functions labeled with **Async** require `rbPoll()` to be called very frequently to function correctly.

### ↗️ Adjusting Library Size
  The fully compiled library is ~130kB, however that's only if you choose to include everything, otherwise the size varies depending on what is linked to your project. For example an average Arduino sketch will usually be ~30-40kB for a basic send and receive script.
  
  The library uses static messaging buffers for both incoming and outgoing data, these, by default are 100kB in size and will therefore require ~200kB in dynamic memory making the total requirement for the library ~230kB. Further to this queueing is available if sufficient memory exists on your device, however by default both incoming and outgoing queues are set to 1. Below are the steps to increase or decrease the queue & payload sizes to reduce the dynamic memory requirement.

#### **Changing payload size**
  - Adjust manually in `imt_queue.h` via `IMT_PAYLOAD_SIZE`.

    OR

  - When compiling use the `-DIMT_PAYLOAD_SIZE=*size*U` eg, `-DIMT_PAYLOAD_SIZE=10000U` to set the maximum payload size to 10k Bytes.

#### **Changing queue size**
  - Adjust manually in `imt_queue.h` via `IMT_QUEUE_SIZE`.

    OR

  - When compiling use the `-DIMT_QUEUE_SIZE=*size*U` eg, `-DIMT_QUEUE_SIZE=5U` to set the queue size to 5.

### 📞 Callbacks

#### **Overview**
  User defined callbacks are called internally by the library, if set, to provide you with important messaging information outlined below.

#### **Structure**
```c
typedef struct {
    /**
     * @brief Callback for message provisioning info once its been obtained.
     * 
     * @param messageProvisioning Pointer to the provisioning info structure.
     */
    void (*messageProvisioning)(const jsprMessageProvisioning_t *messageProvisioning);

    /**
     * @brief Callback for when a mobile-originated (MO) message has finished processing 
     * and been sent successfully.
     * 
     * @param id Unique Identifier of the message.
     * @param status Integer indicating result of processing (-1 for failure & 1 for success).
     */
    void (*moMessageComplete)(const unsigned int id, const int status);

    /**
     * @brief Callback for when a mobile-terminated (MT) message has finished processing 
     * and been received successfully.
     * 
     * @param id Unique Identifier of the message.
     * @param status Integer indicating result of processing (-1 for failure & 1 for success).
     */
    void (*mtMessageComplete)(const unsigned int id, const int status);

    /**
     * @brief Callback for the constellationState (signal) has been updated.
     * 
     * @param state Pointer to the updated constellation state structure.
     */
    void (*constellationState)(const jsprConstellationState_t *state);
} rbCallbacks_t;
```

#### **messageProvisioning**
This callback will run only one time when you send your first message and will return all your message provisioning information. Below we check if provisioning has been set then print every topic number and name.
```c
void onMessageProvisioning(const jsprMessageProvisioning_t *messageProvisioning)
{
    if(messageProvisioning->provisioningSet == true)
    {
        printf("Device is provisioned for %d topics\r\n", messageProvisioning->topicCount);
        printf("Provisioned topics:\r\n");
        for(int i = 0; i < messageProvisioning->topicCount; i++)
        {
            printf("Topic name: %s Topic number: %d\r\n", 
            messageProvisioning->provisioning[i].topicName, messageProvisioning->provisioning[i].topicId);
        }
    }
}
```

#### **moMessageComplete**
This callback will run every time an MO message has either sent successfully or failed. Below we print the ID and status of the message as well as keep track of how many messages we managed to send so far.
```c
int messagesSent = 0;

void onMoComplete(const unsigned int id, const int status)
{
    printf("MO Complete: ID = %u, Status = %d\r\n", id, status);
    if(status == 1)
    {
        messagesSent += 1;
    }
}
```

#### **mtMessageComplete**
This callback will run every time an MT message has either fully arrived successfully or failed. Below we print the ID and status of the message as well as update a bool to let our script know when to call `rbReceiveMessageAsync(...)`.
```c
bool receivedNewMessage = false;

void onMtComplete(const unsigned int id, const int status)
{
    printf("MT Complete: ID = %u, Status = %d\r\n", id, status);
    if(status == 1)
    {
        receivedNewMessage = true;
    }
}
```

#### **constellationState**
This callback will run every time an unsolicited constellation state (signal) message is received from the modem, this happens every time there is a change in the signal bars or signal level which will be very often. Below we print the signal bars every time they change.
```c
int currentSignal = 0;

void onConstellationState(const jsprConstellationState_t *state)
{
    if(state->signalBars != currentSignal)
    {
        printf("\033[1;34mCurrent Signal: %d\033[0m\r\n", state->signalBars);
        currentSignal = state->signalBars;
    }
}
```

#### **Register callbacks**
Finally at the start of our script we assign and register our callbacks with the library.
```c
//Assign callbacks
rbCallbacks_t myCallbacks =
{
.messageProvisioning = onMessageProvisioning,
.moMessageComplete = onMoComplete,
.mtMessageComplete = onMtComplete,
.constellationState = onConstellationState
};
//Register Callbacks
rbRegisterCallbacks(&myCallbacks);
```

### ⬆️ Sending Mobile-Originated (MO) Messages (Async)

#### **Queuing MO Messages**
  Using the Async send function will put your message in a queue, you can queue up as many messages as your MO queue size, which is set in `imt_queue.h` as `#define IMT_QUEUE_SIZE 1U`. This is kept at 1 by default.
  Queued messages will send one after the other, you will not be able to queue another message unless there is space in the queue. `rbPoll()` is responsible for handling these messages to the modem so as stated previously make sure you call it **very frequently**.

  - If your queue is full, by default trying to add another message will fail. If you want to prevent this functionality call `rbSendUnlockAsync()`, this will instead accept any new messages if your queue is full by clearing the oldest message. `rbSendLockAsync()` can be called to undo this.

#### **Non-blocking Transmit**
  Simply call `rbSendMessageAsync(...)` to queue your message, then continue with your code, making sure that the interval between calling `rbPoll()` is at most **50ms** (`rbPoll()` needs to be called at least every **50ms**).

### ⬇️ Receiving Mobile-Terminated (MT) Messages (Async)

#### **Queuing MT Messages**
  As messages arrive `rbPoll()` will place them in the MT queue (1 by default) the size of which can be adjusted in `imt_queue.h` by changing `#define IMT_QUEUE_SIZE 1U`. For most applications a queue size of 1 should be sufficient if your application can acknowledge the message as soon as it arrives to prevent it being overwritten, otherwise follow the steps below.

  - Increase queue size to > 1.
  - As a message comes in, if you received it using `rbReceiveMessageAsync(...)` you should then use `rbAcknowledgeReceiveHeadAsync()` to clear that message from the head of the queue and shift any messages up. If this isn't done, an incoming message will be placed at the tail of the queue.
  - If your queue is full, by default the oldest message will be erased to make space. If you want to prevent this functionality call `rbReceiveLockAsync()`, this will instead not accept any new messages if your queue is full. `rbReceiveUnlockAsync()` can be called to undo this.

#### **Non-Blocking Receive**
  Calling `rbReceiveMessageAsync(...)` will check the head of the MT queue, if a valid message exists it will return successfully. It is advised to wait for a positive callback from `mtMessageComplete` before attempting to receive a message, that way you can ensure one exists.

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

## ❓ **Frequently Asked Questions (FAQ)**

> **Q:** What are topics?
>
> **A:** Topics are named channels used to send/receive messages to/from.
>
> **Q:** How can I provision a topic?
>
> **A:** Topics are provisioned by the plan provider ([Cloudloop Topics](https://rock7.github.io/RockBLOCK-9704/rockblock__9704_8h.html#abba1a824edb46c8c944f5bf7b53ac09c))
>
> **Q:** What do the names of the topics mean?
>
> **A:** Our topic names have no special meaning - they are simply chosen to make it easier to distinguish between them
>
> **Q:** Do I have to use topics?
>
> **A:** Yes, all IMT traffic has to pass through a topic
>
> **Q:** Why would I use topics?
>
> **A:** Topics can be utilised if needed, otherwise the default topic can be used. <br/> For example, the Red Topic may be used to send alerts from the application, the Green Topic may be used for routine status messages. <br/> If all messages used a single topic, it may be harder to identify different message types. <br/> Using topics could also help messages handled differently on the Cloud side (**hint**, you can filter by topic/category in Cloudloop Data)
>
> **Q:** Why is the send message function failing the first time I call it after rebooting the device? (418 Error in Debug).
>
> **A:** When we call `rbBegin()` we send a couple commands to the 9704 modem to set it up, like the simConfig and operationalState, the modem needs some time (100ms or more) after changing these, otherwise it may not be ready to send a message, these only reset if changed manually or the RockBLOCK is rebooted, which is why your code might fail the first time but works every time after that until rebooted. To fix this simply put a 100ms or bigger delay after calling `rbBegin()`.

---

## ⚖️ License

This project is maintained by [Ground Control](https://www.groundcontrol.com) and is open-source under an MIT-style license.

---
