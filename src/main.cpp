#include <Arduino.h>

#include <AceButton.h>
#include <MIDI.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "controller.h"

WebServer server(80);
bool isAccessPointMode = false;

//////////////// HANDLERS ////////////////
void handleSSID()
{
  std::vector<String> networks;

  server.send(200, "text/plain", WiFi.SSID());
}

void handleNormalRoot()
{
  // Your existing root handler for normal operation
}

void handleData()
{
  // Your existing handleData() function goes here
}

void handleRoot()
{
  if (SPIFFS.exists("/wifiSettings.html"))
  {
    File file = SPIFFS.open("/wifiSettings.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  }
  else
  {
    server.send(404, "text/plain", "WiFi settings page not found");
  }
}

void handleSaveWiFi()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  DynamicJsonDocument doc(1024);
  doc[ssid] = password;

  File file = SPIFFS.open("/wifi.json", "w");
  if (file)
  {
    serializeJson(doc, file);
    file.close();
    server.send(200, "text/plain", "WiFi settings saved. Rebooting...");
    delay(1000);
    ESP.restart();
  }
  else
  {
    server.send(500, "text/plain", "Failed to save WiFi settings");
  }
}

//////////////// ACCESS POINT MODE ////////////////
void startAccessPointMode()
{
  isAccessPointMode = true;
  WiFi.softAP("MIDI Controller");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/saveWiFi", HTTP_POST, handleSaveWiFi);
  server.on("/ssid", HTTP_GET, handleSSID);
  server.begin();
}

/**
 * @brief Starts the ESP32 in Access Point mode for configuration
 *
 * This function sets up the ESP32 as a WiFi Access Point, allowing users
 * to connect to it for initial configuration. It sets the SSID to "ESP32-Config",
 * initializes the web server, and sets up routes for the configuration page
 * and WiFi settings submission.
 *
 * The function does the following:
 * 1. Sets the isAccessPointMode flag to true
 * 2. Starts the SoftAP
 * 3. Prints the AP's IP address to Serial
 * 4. Sets up server routes for root ("/") and WiFi settings submission ("/saveWiFi")
 * 5. Starts the web server
 *
 * @note This function is typically called when no valid WiFi credentials are found
 *       or when the device fails to connect to any known networks.
 */
void loadInitialWifi()
{
  if (SPIFFS.begin(true))
  {
    if (SPIFFS.exists("/wifi.json"))
    {
      File file = SPIFFS.open("/wifi.json", "r");
      if (file)
      {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (!error)
        {
          for (JsonPair kv : doc.as<JsonObject>())
          {
            const char *ssid = kv.key().c_str();
            const char *password = kv.value().as<const char *>();

            WiFi.begin(ssid, password);
            unsigned long startAttemptTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000)
            {
              delay(100);
              Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
              Serial.println("\nConnected to WiFi");
              Serial.print("IP address: ");
              Serial.println(WiFi.localIP());
              return;
            }
            else
            {
              Serial.println("\nFailed to connect. Trying next network...");
            }
          }
        }
      }
    }
  }

  // on the first run //
  Serial.println("No valid WiFi credentials found. Starting Access Point mode.");
  std::vector<String> networkList = scanWifiNetworks();
  Serial.println("Scan done");

  Serial.println("Found networks:");
  for (const auto &network : networkList)
  {
    Serial.println(network);
  }
  
  startAccessPointMode();
}

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS initialization failed");
    return;
  }

  loadInitialWifi();

  if (!isAccessPointMode)
  {
    // Normal web server routes
    server.on("/", handleNormalRoot);
    server.on("/data", handleData);

    server.enableCORS(true);
    server.begin();
    Serial.println("Web server started");
  }
}

void loop()
{
  server.handleClient();
}