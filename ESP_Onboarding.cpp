/*
ESP_Onboarding.h - Onboarding library for the ESP8266

  Copyright (c) 2014 Mark Wolfe. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 8 May 2015 by Hristo Gochkov (proper post and file upload handling)

*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

#include "ESP_Onboarding.h"

#define DEBUG_OUTPUT Serial

const char * AUTHORIZATION_HEADER = "Authorization";

ESP_Onboarding::ESP_Onboarding(IPAddress addr, int port)
: _server(addr, port)
{
}

ESP_Onboarding::ESP_Onboarding(int port)
: _server(port)
{
}

void ESP_Onboarding::handleClient() {
  _server.handleClient();
}

void ESP_Onboarding::begin() {

  // filesystem setup
  if (!SPIFFS.begin()) {

#ifdef DEBUG
    DEBUG_OUTPUT.println("Failed to mount file system");
#endif

  }

//   if (!SPIFFS.format()) {

// #ifdef DEBUG
//     DEBUG_OUTPUT.println("Failed to format file system");
// #endif

//   }

  _initToken();

}

void ESP_Onboarding::startServer() {

  _server.on("/wifiSetup", [this]() {
    _wifiSetup();
  });

  //ask server to track these headers which we need for authentication
  const char * headerkeys[] = {"Authorization"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  _server.collectHeaders(headerkeys, headerkeyssize);

  _server.begin();

}

ESP_Onboarding::~ESP_Onboarding() {
}

bool ESP_Onboarding::loadWifiCreds() {
  File configFile = SPIFFS.open("/config.json", "r");

  if (!configFile) {

#ifdef DEBUG
    DEBUG_OUTPUT.println("Failed to open config file");
#endif

    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    DEBUG_OUTPUT.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {

#ifdef DEBUG
    DEBUG_OUTPUT.println("Failed to parse config file");
#endif

    return false;
  }

  _ssid = String(static_cast<const char*>(json.get("ssid")));
  _pass = String(static_cast<const char*>(json.get("pass")));

  return true;
}

String ESP_Onboarding::getSSID() {
  return _ssid;
}

String ESP_Onboarding::getPassword() {
  return _pass;
}

String ESP_Onboarding::getToken() {
  return _token;
}

bool ESP_Onboarding::_saveWifiCreds() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = _ssid;
  json["pass"] = _pass;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {

#ifdef DEBUG
   DEBUG_OUTPUT.println("Failed to open config file for writing");
#endif

    return false;
  }

  json.printTo(configFile);
  return true;
}

void ESP_Onboarding::_initToken() {

  File tokenFile = SPIFFS.open("/token.txt", "r");

  if (tokenFile) {

#ifdef DEBUG
   DEBUG_OUTPUT.println("Token config exists.");
#endif

    _token = tokenFile.readString();

    return;
  }

  _token = "yiphtOobtoifIrt6";

  tokenFile = SPIFFS.open("/token.txt", "w");
  if (!tokenFile) {

    DEBUG_OUTPUT.println("WARN: Failed to open token file for writing");

    return;
  }

  DEBUG_OUTPUT.print("saving token: ");
  DEBUG_OUTPUT.println(_token);

  tokenFile.print(_token);
}


void ESP_Onboarding::_wifiSetup() {

  DEBUG_OUTPUT.print("Header Count: ");
  DEBUG_OUTPUT.println(_server.headers());

  if (!_authenticate()){
    _server.send(401, "text/plain", "Unauthorised");
    return;
  }

  if(_server.hasArg("ssid") && (_server.hasArg("pass"))) {
    _ssid = _server.arg("ssid");
    _pass = _server.arg("pass");

#ifdef DEBUG
    DEBUG_OUTPUT.print("configuring ssid: ");
    DEBUG_OUTPUT.println(_ssid);
#endif

    if (!_saveWifiCreds()) {
    _server.send(500, "text/plain", "Save configuration failed");
    }
    _server.send(200, "text/plain", "OK");

    // reboot
    ESP.restart();

    return;
  }

  _server.send(400, "text/plain", "");
}

bool ESP_Onboarding::_authenticate(){

  if(_server.hasHeader(AUTHORIZATION_HEADER)) {
    String authReq = _server.header(AUTHORIZATION_HEADER);

#ifdef DEBUG
    DEBUG_OUTPUT.print("authReq: ");
    DEBUG_OUTPUT.println(authReq);
#endif

    if(authReq.startsWith("Token")){
      authReq = authReq.substring(6);
      authReq.trim();

      if (authReq.equals(_token)){
        return true;
      }
    }
  }

  return false;
}
