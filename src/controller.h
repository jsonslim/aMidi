#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <AceButton.h>
#include <MIDI.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

#include <vector>

std::vector<String> scanWifiNetworks();

// class Controller
// {
// public:
//     Controller();
//     void begin();
//     void handleButton(uint8_t buttonId, bool pressed);
//     void loadConfiguration();
//     void saveConfiguration();
//     void handleWebRequest(WebServer &server);

// private:
//     struct ButtonConfig
//     {
//         String name;
//         String lcdText;
//         String action;
//         String state;
//         String type;
//         String colorOn;
//         String colorOff;
//         struct MidiCommand
//         {
//             String type;
//             uint8_t number;
//             uint8_t channel;
//             uint8_t value;
//         };
//         std::vector<MidiCommand> midiCommands;
//     };

//     std::vector<ButtonConfig> buttons;
//     void sendMidiCommand(const ButtonConfig &button, bool isPressed);
// };

// extern Controller controller;

#endif
