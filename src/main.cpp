#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "ServoData.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Esp32Servo.h>
#include <Relay.h>
#include <ControlCar.h>
using namespace std;
ControlCar *controlCar = new ControlCar();

///
unsigned long lastRequestTime = 0;
const unsigned long timeoutDuration = 100; // 1 giây timeout
bool hasNewRequest = false;                // Cờ kiểm tra request mới

// Khai báo biến cho các chân kết nối
int pinIN1 = 23;
int pinIN2 = 19;
int pinIN3 = 18;
int pinIN4 = 5;
int pinENA = 22; // PWM cho motor bên trái
int pinENB = 21; // PWM cho motor bên phải

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
void initCar();
void handelServo(JsonDocument doc);
void handelControll(JsonDocument doc);
void handelSwitch(JsonDocument doc);
void printServos();
String valueToJson(String key, string value);
String getStringInputHtml();
String getStringRelayHtml();
String getStringChangeSpeedHtml();
void initServos();
void initRelays();
void setUpServer();
void setUpSPIFFS();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void connectWifi();
void wifiAccessPoint();
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

void setup()
{
    initServos();
    initRelays();
    initCar();
    setUpSPIFFS();
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);
    // connectWifi();
    wifiAccessPoint();
    setUpServer();
}

void loop()
{
    webSocket.cleanupClients();
    if (millis() - lastRequestTime > timeoutDuration)
    {
        if (!hasNewRequest)
        {
            controlCar->stop();
            hasNewRequest = false;
        }
    }
    else
    {
        hasNewRequest = false;
    }
}

void wifiAccessPoint()
{

    WiFi.softAP(ssidAP, passwordAP);
    Serial.println("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void connectWifi()
{
    unsigned long startAttemptTime = millis();
    const unsigned long maxConnectTime = 5000;
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
    ServoData *servo6 = new ServoData("Cờ hiệu", 14, 180, 90);
    servo6->setCurrentAngle(180);
    servos.push_back(servo1);
    servos.push_back(servo2);
    servos.push_back(servo3);
    servos.push_back(servo4);
    servos.push_back(servo5);
    servos.push_back(servo6);
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
                String typeHandle = doc["type"];
                if (typeHandle == "servo")
                {

                    handelServo(doc);
                }
                else if (typeHandle == "control")
                {
                    handelControll(doc);
                }
                else if (typeHandle == "switch")
                {
                    handelSwitch(doc);
                }
            }
        }
    }
}

void handleChangeSpeed(AsyncWebServerRequest *request);

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
    server.on("/setSpeed", HTTP_POST, handleChangeSpeed);
    server.on("/getChangeSpeed", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/json", valueToJson("input", getStringChangeSpeedHtml().c_str())); });

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
    lastRequestTime = millis();
    hasNewRequest = true;
    if (action == "U")
    {
        controlCar->moveForward();
    }
    else if (action == "D")
    {
        controlCar->moveBackward();
    }
    else if (action == "L")
    {
        // controlCar->moveLeft();
        controlCar->moveRight();
    }
    else if (action == "R")
    {
        // controlCar->moveRight();
        controlCar->moveLeft();
    }
    else if (action == "UL")
    {
        // controlCar->turnLeft();
        controlCar->turnRight();
    }
    else if (action == "UR")
    {
        // controlCar->turnRight();
        controlCar->turnLeft();
    }
    else if (action == "DL")
    {
        // controlCar->turnLeftReverse();
        controlCar->turnRightReverse();
    }
    else if (action == "DR")
    {
        // controlCar->turnRightReverse();
        controlCar->turnLeftReverse();
    }
    else if (action == "S")
    {
        controlCar->stop();
    }

    else
    {
        controlCar->stop();
    }
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
        html += "<input type='range' class='range servo-range' id='range" + String(i) + "' min='0' max='" + String(servos[i]->getAngle()) + "' value='" + String(servos[i]->getCurrentAngle()) + "' data-pin-mode='" + String(servos[i]->getPin()) + "' data-standart-angle='" + String(servos[i]->getStandartAngle()) + "'>";
        html += "<input type='number' class='current-value' min='1' max='" + String(servos[i]->getAngle()) + "' value='" + String(servos[i]->getCurrentAngle()) + "' data-pin-mode='" + String(servos[i]->getPin()) + "'>";
        html += "<span class='status normal standard-angle' data-pin-mode='" + String(servos[i]->getPin()) + "' data-standart-angle='" + String(servos[i]->getStandartAngle()) + "'>Normal</span>";
        html += "</div>";
        html += "</div>";
        html += "</div>";
    }

    return html;
}
void handleChangeSpeed(AsyncWebServerRequest *request)
{
    int params = 0;
    if (request->hasParam("moveSpeed", true))
    {
        int moveSpeed = request->getParam("moveSpeed", true)->value().toInt();
        controlCar->setSpeedMove(moveSpeed);
        params++;
    }
    if (request->hasParam("turnSpeed", true))
    {
        int turnSpeed = request->getParam("turnSpeed", true)->value().toInt();
        controlCar->setSpeedTurn(turnSpeed);
        params++;
    }

    if (params > 0)
    {
        request->send(200, "text/plain", "Speed updated");
    }
    else
    {
        request->send(400, "text/plain", "No valid parameters provided");
    }
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

String getStringChangeSpeedHtml()
{
    String html;
    html += "<div class='range-container'>";
    html += "<div class='range-component'>";
    html += "<label for='moveSpeed'>Tốc độ di chuyển</label>";
    html += "<div class='input-wrapper'>";
    html += "<input type='range' class='range default-speed' id='moveSpeed' min='0' max='255' value='" + String(controlCar->getSpeedMove()) + "'>";
    html += "<input type='number' class='current-value default-speed-input' min='1' max='255' value='" + String(controlCar->getSpeedMove()) + "'>";
    html += "</div>";
    html += "</div>";
    html += "</div>";

    html += "<div class='range-container'>";
    html += "<div class='range-component'>";
    html += "<label for='turnSpeed'>Tốc độ quay</label>";
    html += "<div class='input-wrapper'>";
    html += "<input type='range' class='range turn-speed' id='turnSpeed' min='0' max='255' value='" + String(controlCar->getSpeedTurn()) + "'>";
    html += "<input type='number' class='current-value turn-speed-input' min='1' max='255' value='" + String(controlCar->getSpeedTurn()) + "'>";
    html += "</div>";
    html += "</div>";
    html += "</div>";

    return html;
}

void initCar()
{
  
    controlCar = new ControlCar(pinIN1, pinIN2, pinIN3, pinIN4, pinENA, pinENB);
}