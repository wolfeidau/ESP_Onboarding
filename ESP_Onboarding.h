/*
ESP_Onboarding.cpp - Onboarding library for the ESP8266

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

#ifndef ESP_Onboarding_h
#define ESP_Onboarding_h

#include <functional>

//#define ESPO_DEBUG

class ESP_Onboarding
{
  public:
    ESP_Onboarding(ESP8266WebServer * server);
    ~ESP_Onboarding();
    void handleClient();
    void begin();
    void startServer(bool configured);
    String getSSID();
    String getPassword();
    String getToken();
    bool loadWifiCreds();
  protected:
    bool _saveWifiCreds();
    void _initToken();
    void _wifiSetup();
    void _wifiReset();
    bool _authenticate();
    String _ssid;
    String _pass;
    String _token;
    ESP8266WebServer * _server;
};

#endif //ESP_Onboarding_h
