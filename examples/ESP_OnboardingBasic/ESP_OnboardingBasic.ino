#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP_Onboarding.h>
#include <ArduinoJson.h>
#include <FS.h>

// enable debugging for the onboarding library
#define ESPO_DEBUG

// Listen on :9000
ESP8266WebServer webserver(9000);

ESP_Onboarding server(&webserver);

void setup() {

  Serial.begin(115200);
  Serial.println("");

  server.begin();

  bool configured = server.loadWifiCreds();

  Serial.println("Loading onboarding server");
  server.startServer(configured);

  // do we have config
  if (configured) {
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

}

void loop() {
  server.handleClient();
}
