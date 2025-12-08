#pragma once

struct WiFiParameters{

  //Variables
  String WiFiSSID;
  String WiFiPassword;

  //This is checking if Config went ok.
  bool Valid;
  bool Connected;

};

WiFiParameters WiFiConfig();
void WiFiConnect(String ssid, String password);
void WiFiDisconnect();