#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "ServoData.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Esp32Servo.h>
#include <Relay.h>
using namespace std;

// Config wifi Station mode
const char *ssid = "P424-2";
const char *password = "0947900523";

// Config wifi AP mode
const char *ssidAP = "Nhan Du Du";
const char *passwordAP = "12345678";

// init vector to store servo data
vector<ServoData *> servos;
vector<Relay *> relays;
JsonDocument doc;

// declare function
void handelServo(JsonDocument doc);
void handelControll(JsonDocument doc);
void handelSwitch(JsonDocument doc);
void printServos();
String valueToJson(String key, string value);
String getStringInputHtml();
String getStringRelayHtml();
void initServos();
void initRelays();
void setUpServer();
void setUpSPIFFS();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

void setup()
{
    unsigned long startAttemptTime = millis();
    const unsigned long maxConnectTime = 5000;
    initServos();
    initRelays();
    setUpSPIFFS();
    Serial.begin(115200);
    delay(1000);
    WiFi.softAP(ssidAP, passwordAP);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    Serial.println("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    WiFi.mode(WIFI_MODE_APSTA); // Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < maxConnectTime)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    setUpServer();
}

void loop()
{
    webSocket.cleanupClients();
}

void initRelays()
{
    Relay *relay1 = new Relay("Đèn Laser", 13);
    Relay *relay2 = new Relay("Cờ hiệu", 12);
    relays.push_back(relay1);
    relays.push_back(relay2);
}

void initServos()
{
    ServoData *servo1 = new ServoData("Khung", 27, 180, 90);
    ServoData *servo2 = new ServoData("Vai", 26, 180, 90);
    ServoData *servo3 = new ServoData("Khuỷu tay", 25, 130, 0);
    servo3->setCurrentAngle(0);
    ServoData *servo4 = new ServoData("Bộ kẹp", 33, 180, 0);
    servo4->setCurrentAngle(0);
    ServoData *servo5 = new ServoData("Laser", 32, 180, 90);
    servos.push_back(servo1);
    servos.push_back(servo2);
    servos.push_back(servo3);
    servos.push_back(servo4);
    servos.push_back(servo5);
}
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.println("Websocket client connection received");
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.println("Client disconnected");
    }
    if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info;
        info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            char *msg = new char[len + 1];
            strncpy(msg, (char *)data, len);
            msg[len] = '\0';

            DeserializationError error = deserializeJson(doc, msg);
            delete[] msg;
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }
            else
            {
                String type = doc["type"];
                if (type == "servo")
                {

                    handelServo(doc);
                }
                else if (type == "control")
                {
                    handelControll(doc);
                }
                else if (type == "switch")
                {
                    handelSwitch(doc);
                }
            }
        }
    }
}

void setUpServer()
{
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style.css", "text/css"); });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/getInput", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/json", valueToJson("input", getStringInputHtml().c_str())); });
    server.on("/getSwitch", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/json", valueToJson("input", getStringRelayHtml().c_str())); });
    webSocket.onEvent(onWsEvent);
    server.addHandler(&webSocket);
    server.begin();
}

void setUpSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
    Serial.println("SPIFFS mounted successfully");
}

void handelServo(JsonDocument doc)
{
    int servoPin = doc["servoPin"];
    int angle = doc["angle"];

    for (int i = 0; i < servos.size(); i++)
    {
        if (servos[i]->getPin() == servoPin && servos[i]->getAngle() != angle)
        {
            servos[i]->setCurrentAngle(angle);
            break;
        }
    }
}

void handelSwitch(JsonDocument doc)
{
    int pinMode = doc["pinMode"];

    bool state = doc["state"];
    Serial.println(state);
    Serial.println("-----------------");
    for (int i = 0; i < relays.size(); i++)
    {
        if (relays[i]->getPin() == pinMode)
        {
            relays[i]->setState(state);
            break;
        }
    }
}

void printServos()
{
    for (int i = 0; i < servos.size(); i++)
    {
        Serial.println(servos[i]->getName());
        Serial.println(servos[i]->getPin());
        Serial.println(servos[i]->getAngle());
        Serial.println(servos[i]->getCurrentAngle());
    }
}
void handelControll(JsonDocument doc)
{
    
    String action = doc["action"];
    Serial.println(action);
}

String getStringInputHtml()
{
    String html;

    for (int i = 0; i < servos.size(); i++)
    {
        html += "<div class='range-container'>";
        html += "<div class='range-component'>";
        html += "<label for='range" + String(i) + "'>" + servos[i]->getName() + "</label>";
        html += "<div class='input-wrapper'>";
        html += "<input type='range' class='range' id='range" + String(i) + "' min='0' max='" + String(servos[i]->getAngle()) + "' value='" + String(servos[i]->getCurrentAngle()) + "' data-pin-mode='" + String(servos[i]->getPin()) + "' data-standart-angle='" + String(servos[i]->getStandartAngle()) + "'>";
        html += "<input type='number' class='current-value' min='1' max='" + String(servos[i]->getAngle()) + "' value='" + String(servos[i]->getCurrentAngle()) + "' data-pin-mode='" + String(servos[i]->getPin()) + "'>";
        html += "<span class='status normal standard-angle' data-pin-mode='" + String(servos[i]->getPin()) + "' data-standart-angle='" + String(servos[i]->getStandartAngle()) + "'>Normal</span>";
        html += "</div>";
        html += "</div>";
        html += "</div>";
    }

    return html;
}

String valueToJson(String key, string value)
{
    doc[key] = value;
    String jsonStr;
    serializeJson(doc, jsonStr);
    return jsonStr;
}
String getStringRelayHtml()
{
    String html;
    for (int i = 0; i < relays.size(); i++)
    {
        String state = relays[i]->getState() ? "checked" : "";
        html += "<div class='switch-container-wrapper'>";
        html += "<div class='switch-container'>";
        html += "<label for='toggle-switch" + String(i) + "' class='switch-label'>" + relays[i]->getName() + "</label>";
        html += "<label class='switch'>";
        html += "<input type='checkbox' id='toggle-switch" + String(i) + "' data-pin-mode='" + String(relays[i]->getPin()) + "' " + state + ">";
        html += "<span class='slider round'></span>";
        html += "</label>";
        html += "</div>";
        html += "</div>";
    }
    return html;
}
