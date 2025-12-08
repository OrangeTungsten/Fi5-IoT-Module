#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "IoT_Module.h"
#include "Connection_Module/WiFi_Module.h"
#include "Configuration.h"


HTTPClient HTTP;
WiFiClientSecure clientSecure;
uint8_t httpResponseCode;


//Make POST request
void httpPOST(String endpointURL, String tP, String fP, String pT, String fU){

    HTTP.setReuse(false);
    clientSecure.setInsecure();
    
    HTTP.begin(endpointURL); //full URL
    HTTP.addHeader("Content-Type", "application/json");
    
    // Body (JSON format)
    String jsonData = "{\"colB\":" + tP + ",\"colC\":" + fP + ",\"colD\":" + pT + ",\"colE\":" + fU + "}";

    //POST and test
    uint8_t httpResponseCode = HTTP.POST(jsonData);

    if (httpResponseCode > 0){
        digitalWrite(LED_PIN, 1);
        Serial.println("Data uploaded.");
    }else{
        Serial.println("Upload error!");
        digitalWrite(LED_PIN, 0);
    }

    HTTP.end();
    clientSecure.stop();

    delay(50);

}

