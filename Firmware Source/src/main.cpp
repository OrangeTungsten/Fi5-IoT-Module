#include <Arduino.h>
#include <nvs_flash.h>
#include <Preferences.h>
#include "Configuration.h"
#include "Connection_Module/WiFi_Module.h"
#include "Display_Module/Display_Module.h"
#include "IoT_Module/IoT_Module.h"


Preferences EEPROM;

volatile bool WIFI_CFG = 0;
volatile bool WIFI_RST = 0;
volatile bool URL_CFG  = 0;

String WriteAPI, dateTime;
String totalPrints, failedPrints, printTime, filamentUsed;

extern WiFiParameters Parameters;


void IRAM_ATTR WiFiCfgISR();
void IRAM_ATTR WiFiRstISR();
void IRAM_ATTR IoTCfgISR();

void EstablishNewConnection();
void EstablishOldConnection();
void WiFiReset();
void SaveNewParameters();
void DefineEndpointURL();
void getCurParams();

void RecordMessage(HardwareSerial &serialPort, char separator);
void UploadData(String tP, String fP, String pT, String fU);

void SerialPrintEndSession(char sign);



void setup() {

    //Initialize NVS (EEPROM)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }  

    EEPROM.begin("WiFi Parameters", RW_MODE);
    EEPROM.begin("API", RW_MODE);

    //Initialize display
    InitializeDisplay();
    
    //Initialize both Serial ports.
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
        
    //Pin Mode
    pinMode(WIFI_CFG_PIN, INPUT_PULLUP);
    pinMode(WIFI_RST_PIN, INPUT_PULLUP);
    pinMode(IOT_CFG_PIN,  INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    attachInterrupt(WIFI_CFG_PIN, WiFiCfgISR, FALLING);
    attachInterrupt(WIFI_RST_PIN, WiFiRstISR, FALLING);
    attachInterrupt(IOT_CFG_PIN,  IoTCfgISR,  FALLING);


    digitalWrite(LED_PIN, 0);
    SerialPrintEndSession('-');


    Serial.println("Checking for default parameters...");
    
    //Initial connection to existing WiFi
    //EEPROM.begin("WiFi Parameters", RO_MODE); 
    if(EEPROM.isKey("SSID") && EEPROM.isKey("Password")){
        WiFiConnect(EEPROM.getString("SSID"), EEPROM.getString("Password"));
        //EEPROM.end();
    }else{
        Serial.println("There are no default parameters.");
        //EEPROM.end();
        EstablishNewConnection();
    }

    //Check if API is defined
    //EEPROM.begin("API", RO_MODE);
    if(EEPROM.isKey("APIEndpoint")){
        Serial.printf("Write API URL is: %s\n", EEPROM.getString("APIEndpoint").c_str());
    }else{
        Serial.println("Write API URL is not defined. Run setup by pressing IoT_Cfg button and typing '1'.");
    }


    SerialPrintEndSession('-');

    Serial.flush();
    Serial2.flush();
    
    
}

void loop() {
    
    //Until the message arrives
    while(Serial2.available() == 0){

        //Wi-Fi Configure Interrupt
        if(WIFI_CFG == 1){

            EstablishNewConnection();
            SerialPrintEndSession('=');
            WIFI_CFG = 0;

        }

        //Wi-Fi Reset Interrupt
        if(WIFI_RST == 1){

            WiFiReset();
            SerialPrintEndSession('=');
            WIFI_RST = 0;

        }

        //URL Configure Interrupt
        if(URL_CFG == 1){

            DefineEndpointURL ();
            SerialPrintEndSession('=');
            URL_CFG = 0;
        
        }
        
        //Test pin
        if(digitalRead(12)==0){

            digitalWrite(LED_PIN, 1);
            while(digitalRead(12)==0) {}
            
            getCurParams();

            // UploadData("1", "2", "3", "4");
            // UpdateDisplay("","","","", dateTime);

            digitalWrite(LED_PIN, 0);

        }
        
        delay(10);

    }
    

    RecordMessage(Serial2, ',');
    UploadData(totalPrints, failedPrints, printTime, filamentUsed);
    UpdateDisplay(totalPrints, failedPrints, printTime, filamentUsed, "");


    Serial.flush();
    Serial2.flush();

    delay(10);

}

//Flagging button presses
void IRAM_ATTR WiFiCfgISR() {  
    WIFI_CFG = 1;
}

void IRAM_ATTR WiFiRstISR(){
    WIFI_RST = 1;
}

void IRAM_ATTR IoTCfgISR(){
    URL_CFG = 1;
}


////Connect to the new WiFi network
void EstablishNewConnection(){

    //Set the parameters
    Parameters = WiFiConfig();
    
    //Try to connect only if all input parameters are valid
    if(Parameters.Valid == true){

        WiFiConnect(Parameters.WiFiSSID, Parameters.WiFiPassword);
        
        Serial.println();

        //If connection is established, save current parameters as defaults
        if(Parameters.Connected == true){
            Serial.println("New connection is established.");
            SaveNewParameters();
        //If error, try to connect to with prevoiusly saved parameters
        }else{
            Serial.println("New connection is not established.");
            EstablishOldConnection();
        }

    }else{
        Serial.println("New connection is not established.");
    }

    return;
    
}

//Connect to the previous WiFi network
void EstablishOldConnection(){

    //EEPROM.begin("WiFi Parameters", RO_MODE);

    //Connect to old network only if Default parameters in NVM exist
    if(EEPROM.isKey("SSID") && EEPROM.isKey("Password")){
        
        Serial.println();
        Serial.println("Trying to connect to previous network:");
        
        WiFiConnect(EEPROM.getString("SSID"), EEPROM.getString("Password"));

        if(Parameters.Connected == true){
            Serial.println("Old onnection is retrived.");
        }else{
            Serial.println("Error. Old onnection is not retrived either.");
        }

    }else{
        Serial.println("Default parameters doesn't exist. Not connected.");
    }

    //EEPROM.end();
   
}


//Disconnect from the current network and earse default parameters 
void WiFiReset(){

    WiFiDisconnect();

    delay(750);

    //Delete default parameters from NVM?
    Serial.println();
    Serial.println("Press WIFI_RST_BTN again for earsing default parameters.");
    
    unsigned long timer = millis();

    while(millis()-timer < 3000) {

        if(digitalRead(WIFI_RST_PIN) == 0){ 
            while(digitalRead(WIFI_RST_PIN)==0) {}
            //EEPROM.begin("WiFi Parameters", RW_MODE);
            EEPROM.remove("SSID");
            EEPROM.remove("Password");
            //EEPROM.end();
            Serial.println("-> Default parameters are earsed");
            return;
        }
        delay(50);
    }

    Serial.println("-> Default parameters are not changed.");

}

//Store new WiFi parameters to the EEPROM (NVS)
void SaveNewParameters(){

    //EEPROM.begin("WiFi Parameters", RW_MODE);

    //Update SSID to NVM
    if(EEPROM.isKey("SSID") != 1 || Parameters.WiFiSSID != EEPROM.getString("SSID")){
        EEPROM.putString("SSID", Parameters.WiFiSSID);
        Serial.println("Default SSID is updated.");
    }else{
        Serial.println("Default SSID not changed.");
    }

    //Update Password to NVM
    if(EEPROM.isKey("Password") != 1 || Parameters.WiFiPassword != EEPROM.getString("Password")){
        EEPROM.putString("Password", Parameters.WiFiPassword);
        Serial.println("Default password is updated.");
    }else{
        Serial.println("Default password not changed.");
    }

    //EEPROM.end();

}

//Store API endpoint URL to the EEPROM (NVS)
void DefineEndpointURL(){

    //EEPROM.begin("API", RW_MODE);
    
    Serial.println("Define full URL of the API endpoint: ");
    while(Serial.available() == 0){}
    
    //Obtain variable from the input
    String APIEndpoint = Serial.readStringUntil('\n');
    Serial.println(APIEndpoint);

    //Store new API Endpoint URL to NVM
    if(EEPROM.isKey("APIEndpoint") != 1 || APIEndpoint != EEPROM.getString("APIEndpoint")){
    //     EEPROM.remove("APIEndpoint");
        EEPROM.putString("APIEndpoint", APIEndpoint);
        Serial.println("Default API endpoint URL is updated.");
    }else{
        Serial.println("Default API endpoint URL is not changed.");
    }


    //EEPROM.end();

}


//Assign arrived data from printer to the four variables
void RecordMessage(HardwareSerial &serialPort, char separator){

    digitalWrite(LED_PIN, 1);

    String message = serialPort.readStringUntil('\n');
    message.trim();

    //Parsing to four String variables
    totalPrints = message.substring(0, message.indexOf(separator));
    
    failedPrints = message.substring(message.indexOf(separator) + 1, message.indexOf(separator, message.indexOf(separator) + 1));
    
    printTime = message.substring(message.lastIndexOf(separator, message.lastIndexOf(separator) - 1) + 1, message.lastIndexOf(separator));
    const char* printTimeChar = printTime.c_str();
    int printTimeInt = atoi(printTimeChar) / 60; //min
    printTime = String(printTimeInt);
    
    filamentUsed = message.substring(message.lastIndexOf(separator) + 1);
    const char* filamentUsedChar = filamentUsed.c_str();
    int filamentUsedInt = atoi(filamentUsedChar) / 100; //cm
    filamentUsed = String(filamentUsedInt);

    //Debug
    Serial.println("Received data:");
    Serial.println(totalPrints);
    Serial.println(failedPrints);
    Serial.println(printTime);
    Serial.println(filamentUsed);
    SerialPrintEndSession('.');

    digitalWrite(LED_PIN, 0);
}

//Upload desired data to the corresponding fields
void UploadData(String tP, String fP, String pT, String fU){

    if(Parameters.Connected == 1){

        //EEPROM.begin("API", RO_MODE);

        if(EEPROM.isKey("APIEndpoint")){

            String endpointURL =  EEPROM.getString("APIEndpoint");
            httpPOST(endpointURL, tP, fP, pT, fU);
            
        }else{
            Serial.println("API Endpoint URL is not defined. Run setup by pressing IoT_Cfg button.");
        }
        
        //EEPROM.end();

    }else{
        Serial.println("Can't upload. Internet connection doesn't exist.");
    }

}


void getCurParams(){

    //EEPROM.begin("WiFi Parameters", RO_MODE);
    Serial.println(EEPROM.getString("SSID"));
    Serial.println(EEPROM.getString("Password"));
    //EEPROM.end();

    //EEPROM.begin("API", RO_MODE);
    Serial.println(EEPROM.getString("SSID"));
    Serial.println(EEPROM.getString("APIEndpoint"));
    //EEPROM.end();

}

//Print 28 times sign on Serial0
void SerialPrintEndSession(char sign){

    for(uint8_t i=0; i<28; i++){
        Serial.print(sign);
    }

    Serial.println();
}