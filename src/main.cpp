#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "ServoData.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Esp32Servo.h>
using namespace std;
const char *ssid = "P424-2";
const char *password = "0947900523";
vector<ServoData *> servos;
JsonDocument doc;
void handelServo(JsonDocument doc);
void handelControll(JsonDocument doc);
void printServos();
String valueToJson(String key, string value);
String getStringInputHtml();
void initServos()
{
    ServoData *servo1 = new ServoData("Khung", 27, 180);
    ServoData *servo2 = new ServoData("Vai", 26, 180);
    ServoData *servo3 = new ServoData("Khuỷu tay", 25, 180);
    ServoData *servo4 = new ServoData("Bộ kẹp", 33, 180);
    ServoData *servo5 = new ServoData("Laser", 32, 180);

    servos.push_back(servo1);
    servos.push_back(servo2);
    servos.push_back(servo3);
    servos.push_back(servo4);
    servos.push_back(servo5);
}

AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

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
void setup()
{

    initServos();
    setUpSPIFFS();
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); // Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while (WiFi.status() != WL_CONNECTED)
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
    if (action == "U")
    {
        servos[0]->setCurrentAngle(servos[0]->getCurrentAngle() + 10);
    }
    else if (action == "D")
    {
        servos[0]->setCurrentAngle(servos[0]->getCurrentAngle() - 10);
    }
    else if (action == "L")
    {
        servos[1]->setCurrentAngle(servos[1]->getCurrentAngle() - 10);
    }
    else if (action == "R")
    {
        servos[1]->setCurrentAngle(servos[1]->getCurrentAngle() + 10);
    }
    else if (action == "S")
    {
        servos[0]->setCurrentAngle(0);
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "HL")
    {
        servos[0]->setCurrentAngle(0);
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "HR")
    {
        servos[0]->setCurrentAngle(180);
        servos[1]->setCurrentAngle(180);
    }
    else if (action == "TR")
    {
        servos[0]->setCurrentAngle(180);
    }
    else if (action == "TL")
    {
        servos[0]->setCurrentAngle(0);
    }
    else if (action == "BR")
    {
        servos[1]->setCurrentAngle(180);
    }
    else if (action == "BL")
    {
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "C")
    {
        servos[0]->setCurrentAngle(90);
        servos[1]->setCurrentAngle(90);
    }
    else if (action == "Z")
    {
        servos[0]->setCurrentAngle(0);
        servos[1]->setCurrentAngle(180);
    }
    else if (action == "X")
    {
        servos[0]->setCurrentAngle(180);
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "V")
    {
        servos[0]->setCurrentAngle(180);
        servos[1]->setCurrentAngle(180);
    }
    else if (action == "B")
    {
        servos[0]->setCurrentAngle(0);
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "N")
    {
        servos[0]->setCurrentAngle(90);
        servos[1]->setCurrentAngle(0);
    }
    else if (action == "M")
    {
        servos[0]->setCurrentAngle(0);
    }
}

String getStringInputHtml()
{
    String html;
    for (int i = 0; i < servos.size(); i++)
    {
        html += "<div class='input-group'>";
        html += "<label for='range" + String(i) + "'>" + servos[i]->getName() + "</label>";
        html += "<input class='input-range' type='range' id='range" + String(i) + "' name='range" + String(i) + "' min='0' max='" + String(servos[i]->getAngle()) + "' value='" + String(servos[i]->getCurrentAngle()) + "' data-servo-pin='" + String(servos[i]->getPin()) + "' oninput='this.nextElementSibling.value = this.value'>";
        html += "<output for='range" + String(i) + "'>" + String(servos[i]->getCurrentAngle()) + "</output>";
        html += "</div>";
    }
    return html;
}

String valueToJson(String key, string value)
{
    // Thêm giá trị vào document
    doc[key] = value;

    // Tạo một chuỗi JSON
    String jsonStr;
    serializeJson(doc, jsonStr);

    // Trả về chuỗi JSON
    return jsonStr;
}