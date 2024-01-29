<div align="center">
<h1 align="center">
<br>Bidirectional USART-SPI-Gateway on FreeRTOS</h1>
<h3>◦ ► Bidirectional FreeRTOS based USART-SPI-Gateway</h3>
<h3>◦ Developed with the software and tools below.</h3>

<p align="center">
<img src="https://img.shields.io/badge/YAML-CB171E.svg?style=flat-square&logo=YAML&logoColor=white" alt="YAML" />
<img src="https://img.shields.io/badge/C-A8B9CC.svg?style=flat-square&logo=C&logoColor=black" alt="C" />
</p>
<img src="https://img.shields.io/github/license/dtmutsvanga/usart_spi_gateway?style=flat-square&color=5D6D7E" alt="GitHub license" />
<img src="https://img.shields.io/github/last-commit/dtmutsvanga/usart_spi_gateway?style=flat-square&color=5D6D7E" alt="git-last-commit" />
<img src="https://img.shields.io/github/commit-activity/m/dtmutsvanga/usart_spi_gateway?style=flat-square&color=5D6D7E" alt="GitHub commit activity" />
<img src="https://img.shields.io/github/languages/top/dtmutsvanga/usart_spi_gateway?style=flat-square&color=5D6D7E" alt="GitHub top language" />
</div>

---

## 📖 Table of Contents
- [📖 Table of Contents](#-table-of-contents)
- [📍 Overview](#-overview)
- [📦 Features](#-features)
- [⚙️ How it works](#-how-it-works)
- [📂 repository Structure](#-repository-structure)
- [🚀 Getting Started](#-getting-started)
    - [🔧 Prerequisites](#-Prerequisites)
    - [🔧 Installation](#-installation)
- [🛣 Roadmap](#-roadmap)
- [📄 License](#-license)
- [👏 Acknowledgments](#-acknowledgments)

---


## 📍 Overview

► This is a bidirectional USART SPI gateway implemented for the STM32F407 using FreeRTOS. The gateway uses USART2 and SPI2 peripherals in master mode. It is assumed that the slave is always selected i.e nCS is always active-low. <br>
► The gateway allows communication between a USART device and a SPI device by transferring messages between them via stream buffers and queues. <br>
► Messages on USART and SPI can be sent asynchronously  - the SPI slave device can transmit a message at any time. <br>
► The system is capable of handling dynamic length frames. The frame format is strings ending with NULL. It is assumed that the SPI slave device there are no internal registers and other addressing.

---

## 📦 Features

► Asynchronous data transfer<br>
► Dnamic length frames<br>
► Data rate `[---kbps]`<br>
► Portable code. All that needs be done to port the code is to edit the `usart.c` and `spi.c` modules.

---

## ⚙️ How it works 
► All the files that directly concern the gateway are in the folder [usart_spi_gateway](Core\Src\usart_spi_gateway). <br>
► The main processing of the gateway is done in [app_usart_spi_gateway.c](Core\Src\usart_spi_gateway\app_usart_spi_gateway.c), which has platofom independent code. <br>
► Platform dependant code related to spi and usart is located in `usart.c` and `spi.c`, which handles `SPI` and `USART` driver and gpio initializations etc. <br>

###  [📃 app_usart_spi_gateway.c](Core\Src\usart_spi_gateway\app_usart_spi_gateway.c)
► The `app_usart_spi_gateway` module has two main tasks: `vTask_SPI` and `vTask_USART`:
   * `vTask_SPI`: 
      * This task handles data transfer from the SPI to the USART task `vTask_USART`.
      * This task polls the SPI for messages from the slave by calling `spi_receive()`. 
      * Internally, `spi_receive()` calls `HAL_SPI_Receive()` which in turn calls `HAL_SPI_TransmitReceive()`. `HAL_SPI_TransmitReceive()` which sends dummy-bytes over the TX line, thus generating clock signals for the slave to transmit. (see ln 956, 959 in `stm32f4xx_hal_spi.c`)
      * If the message received from the SPI slave does not begin with a `\0`, it posts the received frame to a FreeRTOS stream buffer that is then processed in the USART task `vTask_USART`. <br>
      * A stream buffer was chosen so as to handle dynamic length frames.
      * Posting to the stream buffer was chosen over directly writing to the USART from the `vTask_SPI` because we could miss a message from the slave whilst trasnferring to USART directly.
   * `vTask_USART`: 
      * This tasks handles data transfer from the `USART` to directly to the `SPI` peripheral, and also handles data transfer from the `vTask_SPI` to the `USART` peripheral.
      * When the `USART` peripheral is initalized via `usart_init()`, a the callback function `usart_rx_callback()` is registered. <br>
      * The callback function `usart_rx_callback()` is called when a character is received from the USART. <br>
      * The callback function `usart_rx_callback()` posts the received character to a queue that is then processed in the `vTask_USART` and saved to an internal buffer. When a `\0` is received, or when the internal buffer in the `vTask_USART` is full, the `vTask_USART` posts the message to the SPI peripheral directly. Why directly to the `SPI`? Well, because if the `USART` queue is large enough, we wont miss any messages from the `USART`.
      * This task also receives messages posted by the `vTask_SPI` to the message buffer, and writes them to the `USART` peripheral. 

###  [📃 usart.c](Core\Src\usart_spi_gateway\usart.c), [📃 spi.c](Core\Src\usart_spi_gateway\spi.c)
► The `usart.c` and `spi.c` modules have platform dependant code <br>
► The code is related to spi and usart gpio initialization, interupts, and peripheral intialization etc.<br>
► In order to port the code to another platform running FreeRTOS, we need to edit the `usart.c` and `spi.c` files.<br>





---
---
## 📂 Repository Structure

```sh
└── usart_spi_gateway/
    ├── .clang-format
    ├── .mxproject
    ├── Core/
    │   ├── Inc/
    │   │   ├── FreeRTOSConfig.h
    │   │   ├── main.h
    │   │   ├── stm32f4xx_hal_conf.h
    │   │   └── stm32f4xx_it.h
    │   └── Src/
    │       ├── app_usart_spi_gateway/          
    |       |   ├── app_usart_spi_gateway.h     
    |       |   ├── app_usart_spi_gateway.c
    |       |   ├── spi.c
    |       |   ├── spi.h
    |       |   ├── usart.c
    |       |   ├── usart.h
    │       ├── freertos.c
    │       ├── main.c
    │       ├── stm32f4xx_hal_msp.c
    │       ├── stm32f4xx_hal_timebase_tim.c
    │       ├── stm32f4xx_it.c
    │       ├── sys.c
    │       └── system_stm32f4xx.c
    ├── Drivers/
    │   ├── CMSIS/
    │   │   ├── Core/
    │   │   ├── Core_A/
    │   │   ├── DAP/
    │   │   ├── Device/
    │   │   ├── Documentation/
    │   │   ├── DSP/
    │   │   ├── Include/
    │   │   ├── LICENSE.txt
    │   │   ├── NN/
    │   │   ├── RTOS/
    │   │   └── RTOS2/
    │   └── STM32F4xx_HAL_Driver/
    │       ├── Inc/
    │       ├── LICENSE.txt
    │       └── Src/
    ├── Makefile
    ├── Middlewares/
    │   └── Third_Party/
    │       └── FreeRTOS/
    ├── startup_stm32f407xx.s
    ├── STM32-for-VSCode.config.yaml
    ├── STM32F407VGTx_FLASH.ld
    ├── STM32Make.make
    └── USART_SPI_Gateway.ioc

```

---



---

## 🚀 Getting Started

To use this project, you need to have the following:

### Prerequisites
- A `STM32F407` board with `USART2` and `SPI2` pins connected to a USART device and a SPI device respectively.
- Visual Studio Code with the STM32 VS Code Extension and the Embedded Tools extension installed. You can install the extensions from the [Visual Studio Marketplace](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) or follow the instructions on the [GitHub repository](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension). The ectension will guide you on how to install the necessary dependencies in vscode.
- STM32CubeCLT, a toolset for third-party integrated development environment (IDE) providers, allowing the use of STMicroelectronics proprietary tools within their own IDE frameworks. It is required for basic build and debug functionality. You can download it from the [ST website](https://github.com/bmd-studio/stm32-for-vscode).
- GNU Arm Embedded Toolchain, a cross-platform toolchain for compiling and debugging ARM Cortex-M and Cortex-R embedded applications. You can download it from the [ARM website](https://learn.arm.com/install-guides/stm32_vs/).
- Make, a utility for managing the compilation of multiple source files. You can download it from the [GNU website] or install it using your platform's package manager.
- OpenOCD, an on-chip debugger that provides programming and debugging support for various embedded devices. You can download it from the [OpenOCD website] or install it using your platform's package manager.
- PuTTY, a terminal program to communicate with the USART device. You can download it from the [PuTTY website].
Please ensure you have the following dependencies installed on your system:


### 🔧 Installation
To install the project, follow these steps:
-  Clone the usart_spi_gateway repository:
   ```sh
   git clone https://github.com/dtmutsvanga/usart_spi_gateway
   ```
- Open the project from the root folder in `VSCode`.
- Click the extension icon for `STM32-for-VSCode`, and select `Install Build Tools`. This will install the necessary dependencies.
- Follow the STM32 for vscode on how to open and configure a project.
- press `Ctrl+Shift+B` to build the project. Select `Build STM` in the dialog box.
- press `Ctrl+Shift+B` to flash the project. Select `Flash STM` in the dialog box.
- NB: You also need to configure edit the `launch.json` file for your debug or flashing interface. FOr example, if using jlink for debuging: 
    ```
            "type": "cortex-debug",
            "servertype": "jlink",
            "serverpath": "C:/path/to/JLink/JLinkGDBServerCL.exe",
            "svdFile": "C:/path/to/STM32F407.svd"
    ```

## 🛣 Project Roadmap

> - [✅] `ℹ️  Task 1: Implement modules for USART and SPI peripheral initialization (type of a HAL)`
> - [✅] `ℹ️  Task 2: Implement corwe functions for USART-SPI-gateway`
> - [❌] `ℹ️ Task 3: Test and debug a hardware platform. TODO: Look / buy a hardware platform`
> - [❌] `ℹ️ Task 4: Shift SPI and USART transfers to interrupts`
> - [✔️] `ℹ️ Task 4: Write project Readme`
---
---

## 📄 License


This project is protected under the [GNU General Public License v3.0](https://choosealicense.com/licenses/gpl-3.0/) License. For more details, refer to the [LICENSE](https://choosealicense.com/licenses/gpl-3.0/) file.

---

## 👏 Acknowledgments

- Thanks to the team at (********) for the interesting task

[**Return**](#Top)

---

