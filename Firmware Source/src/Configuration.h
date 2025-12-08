#pragma once

//Preferances modes
#define RO_MODE true
#define RW_MODE false

//Display type
#define DISP_E_INK


#ifdef DISP_E_INK

    //Display Pins
    #define DISP_BUSY_PIN   4
    #define DISP_RESET_PIN  16//sa 4 zamena
    #define DISP_DC_PIN     17
    #define DISP_CS_PIN     5
    //SCLK-18; SDI-23

    //Logo
    
#endif

//WiFi Settings Pins
#define WIFI_CFG_PIN 25
#define WIFI_RST_PIN 26

//IoT Settings
#define IOT_CFG_PIN 33
  
#define TOTAL_PRINTS_FIELD 1
#define SUCCESSFULL_PRINTS_FIELD 2
#define TOTAL_TIME_FIELD 3
#define FILAMENT_USED_FIELD 4


//Second Serial port
#define RXD2 2  // UART2 RX pin
#define TXD2 13  // UART2 TX pin

#define LED_PIN 27
