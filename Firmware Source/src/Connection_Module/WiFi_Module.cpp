#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include "WiFi_Module.h"
#include "Configuration.h"


WiFiParameters Parameters;


WiFiParameters WiFiConfig(){

    
    Parameters.Valid = false;
    uint16_t numOfNetworksFound = 0;

    //Delete previous scan results
    WiFi.scanDelete();

    Serial.println();

    //Scanning for WiFi networks
    Serial.println("Scanning for WiFi networks...");
    numOfNetworksFound = WiFi.scanNetworks();

    Serial.println("Scan complete.");
    Serial.println();

    if(numOfNetworksFound == 0){
        Serial.println("0 networks found.");
        WiFi.scanDelete();
        return Parameters;
    }else{
        Serial.print(numOfNetworksFound);
        Serial.println(" networks found:");
        for(uint16_t i=0; i<numOfNetworksFound; i++){
            Serial.printf("%2u",i+1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.println((WiFi.encryptionType(i)==0)? " - OPEN":"");
            delay(30);
        }

        delay(10); 
    }
    Serial.println();
    

    //Chooing SSID
    Serial.println("Choose WiFi SSID by number: "); 
    while(Serial.available() == 0) {}

    String inputS = Serial.readStringUntil('\n');
    Serial.print(inputS);
    uint8_t input = inputS.toInt();

    if(input < 1 || input > numOfNetworksFound){
        Serial.println(" - Wrong input!");
        WiFi.scanDelete();
        return Parameters;
    }else{

        //Storing WiFi SSID to struct
        Parameters.WiFiSSID = WiFi.SSID(input-1).c_str();
        Serial.print(" - ");
        Serial.println(Parameters.WiFiSSID);  
        //Request Password if not open
        if(WiFi.encryptionType(input-1) != WIFI_AUTH_OPEN){ 
            Serial.print("Input WiFi Password: ");
            while(Serial.available() == 0) {}
            //Storing WiFi password to struct
            Parameters.WiFiPassword = Serial.readStringUntil('\n');
            Serial.println(Parameters.WiFiPassword);    
        }else{
            Parameters.WiFiPassword = "";
            Serial.println("-");
        }
        
        Parameters.Valid = true;
    }

    WiFi.scanDelete();
    return Parameters;


}

void WiFiConnect(String ssid, String password){

    

    WiFi.disconnect(true);
    delay(100);

    Parameters.Connected = false;

    WiFi.begin(ssid, password);

    Serial.printf("Connecting to %s", ssid.c_str());

    unsigned long WiFiTimeout = millis();

    //Wait to establish connection
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(600);

        //Timeout
        if(millis() - WiFiTimeout >= 15000){
            //Parameters.Connected = false;
            Serial.println("Error: Timeout!");
            WiFi.disconnect(true);
            return;
        }
    }


    delay(50);
    
    //Connection established:
    if(WiFi.status() == WL_CONNECTED){
        Parameters.Connected = true;
        Serial.println();
        Serial.printf("Successfuly connected to: %s network.", WiFi.SSID().c_str());
        Serial.println();
        
        //LED
        digitalWrite(LED_PIN, 1);
        delay(120);
        digitalWrite(LED_PIN, 0);
        delay(120);
        digitalWrite(LED_PIN, 1);
        delay(120);
        digitalWrite(LED_PIN, 0);
        delay(120);
    
    }


}

void WiFiDisconnect(){

    WiFi.disconnect(true);

    Serial.println();
    Serial.print("Disconnecting..");

    unsigned long timer = millis();

    while(WiFi.status() != WL_DISCONNECTED){

        Serial.print(".");

        //Timeout
        if(millis() - timer > 10000){
            Serial.println("Disconnection unsuccessful. Still connected.");
            return;
        }

        delay(200);
    }

    Serial.println();
    Serial.println("Disconnected from network.");   
    
}