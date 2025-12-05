<img width="1612" height="318" alt="Baner" src="https://github.com/user-attachments/assets/5594c74e-c346-47bb-8f1e-d2ea98397629" />

# Introduction

Fi5 IoT module is an ESP-32S-based device designed primarily as a data-logging unit, enabling the collection and upload of 3D printing statistic values to a cloud server, as well as displaying them locally on a dedicated display. The project also involves extending the Marlin project by adding a custom G-code command at the firmware level.

This version of the device is implemented on a [Flutur i5](https://github.com/OrangeTungsten/Flutur-i5) FDM 3D printer, with [MKS Robin Nano v1.2](https://makerbase3d.com/product/mks-robin-nano-v1-2/?srsltid=AfmBOooX1eGkBQStCqhv-IZ1dArsi8UtKZWTpPTg1CoTkC5dbtrfiOg2) controller board running on a [Marlin v2.1.2.5](https://github.com/MarlinFirmware/Marlin/tree/2.1.2.5) firmware, but it can be connected to any other device as long as the system is capable of exchanging data over an available UART port. An MH-ET 2.13 in E-ink display is used for local output.

This project provides an infrastructure for communication with external devices, enabling the forwarding of selected data to a cloud server, forming an easily operable IoT platform that It can be used as a flexible platform for creating logging solutions of various types. Project is suitable for further development in multiple directions. The system can be expanded to support two-way communication, allowing remote-control functionality of connected device (3D printer). Although the device is currently implemented for one type of application – logging data, it can be upgraded to support different types of peripherals, including operation with multiple devices simultaneously. This infrastructure provides flexibility and openness for any type of future upgrade.

**Project provides:**
- Complete documentation for reproducing the device: Bill of materials; Electronics schematics; PCB layouts (copper, soldermask, and silkscreen layers); PCB build parameters and guidance; CAD models of 3D printable enclosure; Guidance for configuration;

- Additional code and guidance for modifying 3D printer firmware in order to define a new, UART communication port;

- Additional code and guidance for expanding Marlin firmware with a new g-code machine command;

- Additional code and guidance for creating cloud platform on Google Drive for storing data;

- Device firmware with guidance.


## Stages of realization

Project consists of five main stages of realization:

1. Establishing serial communication between devices;
2. Creating a custom G-code M command for sending data over the predefined UART port;
3. Extracting desired data on firmware level;
4. Creating a cloud platform;
5. Creating a device firmware.

------------------------------------------------------

# 1. Establishing serial communication between devices

A 3D printer motherboard can exchange data with other devices through communication interfaces such as UART, I²C, SPI, CAN, or USB. Each of these interfaces is a built-in hardware feature of the microcontroller, and they follow specific communication protocols (rules for how data is sent and received). Depending on the hardware and firmware configuration, these interfaces can be assigned to certain pins on the board. An interface, once mapped to specific physical pins, constitutes a communication port. Through these ports, the control board can reliably talk to external devices (modules). A lot of microcontrollers (therefore motherboards) have multiple ports, used for different purposes.
All modern 3D printer motherboards include a dedicated communication port for connecting to a host computer. At the microcontroller level, this port is typically implemented as a UART interface, which is then bridged through a USB-to-UART converter (such as CH340, CP2102, or FT232) and exposed to the computer as a standard USB serial port. This port serves as the primary channel for host communication, allowing direct control, firmware configuration, and interactive G-code exchange (control via Pronterface i.e.).
The idea is to create a device that communicates with the microcontroller of the 3D printer motherboard over an another available UART port, independent of the port used for communication with the computer. The device will wait for messages, process them and forward desired data to the cloud – a predefined internet service.

## UART
Universal Asynchronous Receiver/Transmitter (UART) is a simple serial communication interface used to send data between two digital devices using only one transmit and one receive line (TX and RX). Unlike parallel communication where several bits are transferred at once, UART sends data bit-by-bit over a single wire, making it cost-effective, low-pin and easy to implement. Data is framed with a start bit, optional parity, and stop bit, so both sides understand where each byte begins and ends. Because UART is asynchronous, both devices must use the same baud rate (bit speed) to correctly interpret the data, but no shared clock line is needed.
##Port on motherboard
MKS Robin Nano v1.2 is running on [STM32F103VET6](https://www.st.com/en/microcontrollers-microprocessors/stm32f103ve.html) microcontroller. Official documentation shows that this microcontroller features up to five UART interfaces. USART ports also support synchronous mode, which is not needed for this application.

Table 1. shows the default pinout:

| Peripheral | TX   | RX   |
| ---------- | ---- | ---- |
| USART1     | PA9  | PA10 |
| USART2     | PA2  | PA3  |
| USART3     | PB10 | PB11 |
| UART4      | PC10 | PC11 |
| UART5      | PC12 | PD2  |

On STM32F1 devices, the USART/UART peripherals can be reassigned to alternative pin sets using the AFIO remap functionality, but in this project only the default (reset state) pin assignments are considered because the 3DP motherboard already has the remaining pins allocated to other functions and features.
By reviewing the MKS Robin Nano V1.2 motherboard documentation and the Marlin 2.1.2.5 firmware, it was observed that all default UART pins are already used by existing system features. The only UART interface that can be utilized without compromising system integrity or requiring additional remapping is USART1 (TX/RX: PA9/PA10), which is assigned to the optional wireless module port on the motherboard.

The following code is done on firmware of 3D printer’s microcontroller.

## Defining available port on 3DP firmware level

Following files should be edited/created:

edit: Marlin/Configuration.h

```cpp
#define IOT_MODULE

#if ENABLED(IOT_MODULE)
  //IoT module could be enabled only if MKS_WIFI_MODULE is disabled
  #if ENABLED(MKS_WIFI_MODULE)
    #error "Can't use IOT_MODULE module if MKS_WIFI_MODULE is enabled."
  #else
    #define IOT_MODULE_PORT 1 //Which UART port custom ESP IoT module will use. i.e. 1 = UART1(Tx-PA9;Rx-PA10)
    #define IOT_MODULE_BAUDRATE 115200
  #endif
#endif
```

create: Marlin/src/core/serial_extension.h
```cpp
#include "serial.h"

//Define Universal Serial call
#define _SERIAL_PORT_IOT(N)  Serial##N
#define  SERIAL_PORT_IOT(N)  _SERIAL_PORT_IOT(N) 

//Define Serial println function for new port
#define SERIAL_IOT_PRINT(P)   SERIAL_PORT_IOT(IOT_MODULE_PORT).print(P)
#define SERIAL_IOT_PRINTLN(P) SERIAL_PORT_IOT(IOT_MODULE_PORT).println(P)
```


edit: Marlin/src/MarlinCore.cpp

```cpp
#if ENABLED(IOT_MODULE)
  #include "core/serial_extension.h"
#endif
```

edit: Marlin/src/MarlinCore.cpp > setup()
```cpp
SERIAL_PORT_IOT(IOT_MODULE_PORT).begin(IOT_MODULE_BAUDRATE);
```
Now, a new, dedicated, serial port called SERIAL_PORT_IOT is defined. Marlin has a built-in pin allocation functionality, so it is sufficient to define new port by its numerical identifier (1), rather than by  specifying  the physical pins names or addresses. The remaining configuration is already handled in the Wserial.h and HardwareSerial.h files.

----------------------------------------------------------------------------------

## 2. Creating a custom g-code M command for sending data over the SERIAL_PORT_IOT

New command will be executed via M1600 call. A new file called M1600.cpp should be created, and existing gcode.h and gcode.cpp files should be edited:

edit: Marlin/src/gcode/gcode.h > GcodeSuite

```cpp
  #if ENABLED(IOT_MODULE)
    static void M1600();
  #endif
```

edit: Marlin/src/gcode/gcode.cpp > process_parsed_command(const bool) > switch (parser.command_letter) > case 'M': switch (parser.codenum)

```cpp
  #if ENABLED(IOT_MODULE)
    case 1600: M1600(); break;
  #endif
```

create: Marlin/src/gcode/IoT/M1600.cpp

```cpp
#include "../../MarlinCore.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(IOT_MODULE)
    
    #include "../gcode.h"
    #include "../../core/serial_extension.h"
    
    void GcodeSuite::M1600() {

        SERIAL_IOT_PRINT(/*Here goes String type message*/);

    }

#endif
```

--------------------------------------------

#3. Extracting desired data on firmware level

Marlin firmware has a built-in software module called "printcounter", which should be enabled in the *configuration.h* file 

edit: Marlin/Configuration.h

```cpp
#define PRINTCOUNTER
#if ENABLED(PRINTCOUNTER)
  #define PRINTCOUNTER_SAVE_INTERVAL 0 // (minutes) EEPROM save interval during print. A value of 0 will save stats at end of print.
#endif
```

Given module records data like: total prints, runtime, failed prints and filament used and stores it into separate variables. The task is to forward this data to the new SERIAL_PORT_IOT serial port, where it will be received by the IoT module.

Marlin/src/module/printcounter.cpp has variables:

```cpp
struct printStatistics {    // 16 bytes
  //const uint8_t magic;    // Magic header, it will always be 0x16
  uint16_t totalPrints;     // Number of prints
  uint16_t finishedPrints;  // Number of complete prints
  uint32_t printTime;       // Accumulated printing time
  uint32_t longestPrint;
...
```

edit: Marlin/src/gcode/IoT/M1600.cpp

```cpp
#include "../../MarlinCore.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(IOT_MODULE)
    
    #include "../gcode.h"
    #include "../../core/serial_extension.h"
    #include "../../module/printcounter.h"

    printStatistics localStats;
    
    void GcodeSuite::M1600() {

        PrintCounter::init();

        localStats = PrintCounter::getStats();

        SERIAL_IOT_PRINT(localStats.totalPrints);
        SERIAL_IOT_PRINT(",");
        SERIAL_IOT_PRINT(localStats.finishedPrints);
        SERIAL_IOT_PRINT(",");
        SERIAL_IOT_PRINT(localStats.printTime );
        SERIAL_IOT_PRINT(",");
        SERIAL_IOT_PRINTLN(localStats.filamentUsed);

    }

#endif
```

Every time when M1600 command is called, message from 3DP motherboard will be sent over its UART1 port, in the form of single string line, where variables are separated with a separators (commas). In order to activate this system after every print, line M1600 should be placed at the end of every G-code (print). That should be done in the slicer configuration.
Later, firmware of the ESP32 (IoT Module) will be programmed to parse these values between separators to the separate variables.

----------------------------

# 4. Creating a cloud backend

In this setup, Google Drive, together with Google Spreadsheets and Apps Script, serves as a cloud backend for IoT data. It functions as a centralized repository where statistics from a 3D printer are periodically uploaded, and made available for further processing or download.
Functionality of this approach reflects in following: IoT module (device) sends HTTP POST request, triggering Google Apps Script webhook to append new rows to Google Sheet. Google Spreadsheet acts as a centralized cloud data store. Apps Script could be expanded to validate data, compute summaries, generate alerts, or create and update visualizations. 

## Procedure

The following procedure explains how to turn a Google Spreadsheet into a REST endpoint that accepts incoming HTTP requests from an ESP32 (or any HTTP client) and logs values into the sheet:

**1. Create a Google Spreadsheet**

Create a new Google Spreadsheet in your Google Drive. Rename current sheet into “Stats”. Add columns representing the parameters/data you plan to submit (optional). In this case columns: Total prints; Successful prints, Runtime; Filament used will be used.

**2. Create a Google Apps Script**

Open the Google Sheet → Extensions > Apps Script. Write a following script that will accept HTTP requests (POST or GET) and append rows into the sheet.

Code.gs

```js
function doPost(e) {

  //Catch data from POST request (JSON format)
  var data = JSON.parse(e.postData.contents);
  
  // Open Sheet (current file)
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Stats");
  
  //Make type with given format
  var row = [
    new Date(),         
    data.tP || "",    
    data.sP || "",   
    data.rT || "",
    data.fU || ""     
  ];
  
  //Append new row
  sheet.appendRow(row);
  

  //Return answer
  return ContentService
    .createTextOutput(JSON.stringify({status: "success", message: "Data stored"}))
    .setMimeType(ContentService.MimeType.JSON);
}

function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Stats");
  var lastRow = sheet.getLastRow();
  var lastTimestamp = sheet.getRange(lastRow, 1).getValue(); // 1 = first column

  //Return JSON answer
  return ContentService.createTextOutput(JSON.stringify({
    timestamp: lastTimestamp
  })).setMimeType(ContentService.MimeType.JSON);
}
```

**3. Deploy as Web App**

In the Apps Script editor, go to Deploy > New deployment, set “Who has access” to Anyone, then authorize and deploy the script. The generated URL becomes a write endpoint (REST API). By using this URL, any internet-connected device can read and write data to the spreadsheet (according to the defined Code.gs script).

**4. Test the endpoint**

You can test the endpoint using Postman / Hoppscotch / ReqBin:

Method: POST | Content-Type: application/json

Body example:
```
{
"tP" : 6, 
"sP" : 5,
"rT" : 36,
"fU" : 30
}
```

You should receive this response:

```
{
    "status": "success",
    "message": "Data stored"
}
```

And a new row should be added in the Stats sheet.

<img width="918" height="203" alt="Snimak ekrana 2025-11-07 210756" src="https://github.com/user-attachments/assets/f0ecfd28-a479-48f1-8748-9a894fb7c924" />

# Device firmware
The device’s main task is to read message from the UART Serial2 port, parse the message into separate variables, process the values and forward them to the cloud service via POST request. The device also includes a user-defined settings like Wi-Fi and Endpoint URL configuration. These parameters can be set via the UART Serial0 port. Since the device uses a standalone ESP-32S module, a USB to UART bridge must be used for programming and later communication with computer. ESP32 programmer is recommended.

Access to settings and MCU reset function is provided via physical pushbuttons: RESET; IOT_CFG; WIFI_CFG; WIFI_RST. Settings are available all the times except except during message reading, cloud upload, and display update operations. The pushbuttons trigger interrupt events on the ESP-32S module, flagging input event during the run.

Firmware development was carried out using the PlatformIO ecosystem, with Arduino framework, acting as a hardware abstraction layer (HAL).

This firmware uses GxEPD2_3C with Adafruit BusIO and Adafruit GFX libraries for E-ink display control and ArduinoJson library for manipulating JSON data.

Firmware follows a simplified algorithms: 

<img width="500" height="660" alt="main" src="https://github.com/user-attachments/assets/976fbc6c-c834-480c-88e0-2f69c419891e" />
<img width="464" height="660" alt="wifi_cfg" src="https://github.com/user-attachments/assets/7836ba0c-a41a-452b-a50f-f8ebdfc87ee8" /> 
<img width="491" height="600" alt="wifi_rst" src="https://github.com/user-attachments/assets/aff285c3-9971-45df-bdfb-ed5a18797aec" />
<img width="468" height="400" alt="url_cfg" src="https://github.com/user-attachments/assets/7fc5849b-ace3-4f5e-a1cd-a8efa5e17ffd" />

 










