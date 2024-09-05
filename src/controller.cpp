#include "controller.h"

#define MAX_NETWORKS 20


std::vector<String> scanWifiNetworks()
{
    std::vector<String> networks;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    
    Serial.println("Scan done");
    if (n == 0)
    {
        Serial.println("No networks found");
    }
    else
    {
        for (int i = 0; i < n; ++i)
        {
            networks.push_back(WiFi.SSID(i));
        }
    }
    return networks;
}