#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <iostream>
#include "SPIFFS.h"
#include <ArduinoJson.h>
using namespace std;

const char *ssid = "Nam077"; // Thay đổi tên và mật khẩu WiFi cho phù hợp

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

struct ServoInfo
{
  Servo servo;
  int pin;
  string name;
  int maxAngle;
  int currentAngle = 0; // Lưu trữ góc độ hiện tại của servo
};
vector<ServoInfo> servos;

void setUpServos();
void setUpWebServer();
string getStringInputHtml();
String valueToJson(String key, string value);
void changeServoAngle(int servoIndex, int angle);
ServoInfo *findServoByPin(int pin);
void handelAction(string action);
void setup()
{
  Serial.begin(115200);
  setUpServos();
  setUpWebServer();
}

void loop()
{
}

void setUpServos()
{
  ServoInfo servo1;
  servo1.pin = 27;
  servo1.name = "Servo 1";
  servo1.maxAngle = 360;
  servos.push_back(servo1);

  ServoInfo servo2;
  servo2.pin = 13;
  servo2.name = "Servo 2";
  servo2.maxAngle = 360;
  servos.push_back(servo2);

  // 4 con nữa chân 25,26 ,27 ,33
  ServoInfo servo3;
  servo3.pin = 25;
  servo3.name = "Servo 3";
  servo3.maxAngle = 360;
  servos.push_back(servo3);

  ServoInfo servo4;
  servo4.pin = 26;
  servo4.name = "Servo 4";
  servo4.maxAngle = 360;
  servos.push_back(servo4);
  
  ServoInfo servo5;
  servo5.pin = 33;
  servo5.name = "Servo 5";
  servo5.maxAngle = 360;
  servos.push_back(servo5);



  for (int i = 0; i < servos.size(); i++)
  {
    servos[i].servo.attach(servos[i].pin);
  }
}

void setUpWebServer()
{
  // Khởi động SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully");
  WiFi.softAP(ssid);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.on("/getInput", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", valueToJson("input", getStringInputHtml())); });
  server.on("/angle", HTTP_POST, [](AsyncWebServerRequest *request)
            {
  if (request->hasParam("pin", true) && request->hasParam("angle", true)) {
    AsyncWebParameter* pPin = request->getParam("pin", true);
    AsyncWebParameter* pAngle = request->getParam("angle", true);

    int pin = pPin->value().toInt();
    int angle = pAngle->value().toInt();

    ServoInfo* servoInfo = findServoByPin(pin);
    if (servoInfo != nullptr) {
      servoInfo->servo.write(angle);
      servoInfo->currentAngle = angle;
      Serial.println("Updated servo on pin " + String(pin) + " to angle " + String(angle));
    } else {
      Serial.println("Servo with pin " + String(pin) + " not found.");
    }
  } else {
    Serial.println("Pin or angle parameter not found.");
  }
  request->send(200, "text/html", getStringInputHtml().c_str()); });

  server.on("/action", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("action", true))
              {
                AsyncWebParameter *pAction = request->getParam("action", true);
                string action = pAction->value().c_str();
                handelAction(action);
              }
              
              request->send(200, "application/json", valueToJson("status", "ok")); });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.begin();
}

string generateInputRange()
{
  string html;

  return html;
}

void changeServoAngle(int servoIndex, int angle)
{
  servos[servoIndex].servo.write(angle);
  servos[servoIndex].currentAngle = angle;
}

ServoInfo *findServoByPin(int pin)
{
  for (auto &servoInfo : servos)
  { // Giả định servos là một std::vector<ServoInfo>
    if (servoInfo.pin == pin)
    {
      return &servoInfo;
    }
  }
  return nullptr;
}

string getStringInputHtml()
{
  string html;
  for (int i = 0; i < servos.size(); i++)
  {
    html += "<div class='input-group'>";
    html += "<label for='range" + to_string(i) + "'>" + servos[i].name + "</label>";
    html += "<input class='input-range' type='range'" + to_string(i) + "' name='range" + to_string(i) + "' min='0' max='" + to_string(servos[i].maxAngle) + "' value='" + to_string(servos[i].currentAngle) + "' data-servo-pin='" + to_string(servos[i].pin) + "'oninput='this.nextElementSibling.value = this.value' '>";
    html += "<output for='range" + to_string(i) + "'>" + to_string(servos[i].currentAngle) + "</output>";
    html += "</div>";
    html += "</div>";
  }
  return html;
}

String valueToJson(String key, string value)
{
  JsonDocument doc;

  // Thêm giá trị vào document
  doc[key] = value;

  // Tạo một chuỗi JSON
  String jsonStr;
  serializeJson(doc, jsonStr);

  // Trả về chuỗi JSON
  return jsonStr;
}

void handelAction(string action)
{
  //  <button data-action="HL" class="dpad-button">↖</button> <!-- Top-left -->
  //               <button data-action="U" class="dpad-button main-button">&#x25B2</button> <!-- Up -->
  //               <button data-action="TR" class="dpad-button">↗</button> <!-- Top-right -->
  //               <button data-action="L" class="dpad-button main-button">&#x25C0</button> <!-- Left -->
  //               <button data-action="S" class="dpad-button main-button">&#9724</button> <!-- Center -->
  //               <button data-action="R" class="dpad-button main-button">&#x25B6</button> <!-- Right -->
  //               <button data-action="BL" class="dpad-button">↙</button> <!-- Bottom-left -->
  //               <button data-action="D" class="dpad-button main-button">&#x25BC</button> <!-- Down -->
  //               <button data-action="BR" class="dpad-button">↘</button> <!-- Bottom-right -->

  if (action == "HL")
  {
    changeServoAngle(1, 0);
  }
  else if (action == "U")
  {
    changeServoAngle(1, 90);
  }
  else if (action == "TR")
  {
    changeServoAngle(1, 180);
  }
  else if (action == "L")
  {
    changeServoAngle(1, 0);
  }
  else if (action == "S")
  {
    changeServoAngle(1, 90);
  }
  else if (action == "R")
  {
    changeServoAngle(1, 180);
  }
  else if (action == "BL")
  {
    changeServoAngle(1, 0);
  }
  else if (action == "D")
  {
    changeServoAngle(1, 90);
  }
  else if (action == "BR")
  {
    changeServoAngle(1, 180);
  }
  else
  {
    Serial.println("Action not found");
  }
}