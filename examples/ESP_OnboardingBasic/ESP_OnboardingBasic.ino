#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP_Onboarding.h>
#include <ArduinoJson.h>
#include <FS.h>

ESP_Onboarding server;

void setup() {

  Serial.begin(115200);
  Serial.println("");

  server.begin();

  // do we have config
  if (server.loadWifiCreds()) {
    String ssid = server.getSSID();
    String pass = server.getPassword();

    // use config to connect
    WiFi.begin(ssid.c_str(), pass.c_str());

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return; // we are done
  }

  // fall back to AP mode to enable onboarding
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(myIP);
  Serial.print("Access Token: ");
  Serial.println(server.getToken());

  Serial.println("Loading onboarding server");
  server.startServer();
}

void loop() {
  server.handleClient();
}
