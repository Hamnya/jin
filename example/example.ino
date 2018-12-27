#include <SoftwareSerial.h>
 
#include "ESP8266.h"
 
#define SSID "oysterYS"
#define PASS "52005200ys"
#define DST_IP "184.106.153.149" //baidu.com
 
SoftwareSerial esp8266Serial = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(esp8266Serial);
 
void setup()
{
    Serial.begin(9600);
 
    // ESP8266
    esp8266Serial.begin(9600);
    wifi.begin();
    wifi.setTimeout(1000);
 
    /****************************************/
    /******       Basic commands       ******/
    /****************************************/
    // test
    Serial.print("test: ");
    Serial.println(getStatus(wifi.test()));
 
    // restart
    Serial.print("restart: ");
    Serial.println(getStatus(wifi.restart()));
 
    // getVersion
    char version[16] = {};
    Serial.print("getVersion: ");
    Serial.print(getStatus(wifi.getVersion(version, 16)));
    Serial.print(" : ");
    Serial.println(version);
 
 
    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
    // joinAP
    Serial.print("joinAP: ");
    Serial.println(getStatus(wifi.joinAP(SSID, PASS)));
 
 
    /****************************************/
    /******       TCP/IP commands      ******/
    /****************************************/
    // connect
    Serial.print("connect: ");
    Serial.println(getStatus(wifi.connect(ESP8266_PROTOCOL_TCP, DST_IP, 80)));
 
    // send
    Serial.print("send: ");
    Serial.println(getStatus(wifi.send("GET /update?api_key=JY3ND9YVXCJNHHU4&field1=55 HTTP/1.0\r\n\r\n")));
 
}
 
void loop()
{
    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
 
    // read data
    unsigned int id;
    int length;
    int totalRead;
    char buffer[300] = {};
 
    if ((length = wifi.available()) > 0) {
      id = wifi.getId();
      totalRead = wifi.read(buffer, 300);
 
      if (length > 0) {
        Serial.print("Received ");
        Serial.print(totalRead);
        Serial.print("/");
        Serial.print(length);
        Serial.print(" bytes from client ");
        //Serial.print("from client ");
        Serial.print(id);
        Serial.print(": ");
        Serial.println((char*)buffer);
      }
    }
}
 
String getStatus(bool status)
{
    if (status)
        return "OK";
 
    return "KO";
}
 
String getStatus(ESP8266CommandStatus status)
{
    switch (status) {
    case ESP8266_COMMAND_INVALID:
        return "INVALID";
        break;
 
    case ESP8266_COMMAND_TIMEOUT:
        return "TIMEOUT";
        break;
 
    case ESP8266_COMMAND_OK:
        return "OK";
        break;
 
    case ESP8266_COMMAND_NO_CHANGE:
        return "NO CHANGE";
        break;
 
    case ESP8266_COMMAND_ERROR:
        return "ERROR";
        break;
 
    case ESP8266_COMMAND_NO_LINK:
        return "NO LINK";
        break;
 
    case ESP8266_COMMAND_TOO_LONG:
        return "TOO LONG";
        break;
 
    case ESP8266_COMMAND_FAIL:
        return "FAIL";
        break;
 
    default:
        return "UNKNOWN COMMAND STATUS";
        break;
    }
}
 
String getRole(ESP8266Role role)
{
    switch (role) {
    case ESP8266_ROLE_CLIENT:
        return "CLIENT";
        break;
 
    case ESP8266_ROLE_SERVER:
        return "SERVER";
        break;
 
    default:
        return "UNKNOWN ROLE";
        break;
    }
}
 
String getProtocol(ESP8266Protocol protocol)
{
    switch (protocol) {
    case ESP8266_PROTOCOL_TCP:
        return "TCP";
        break;
 
    case ESP8266_PROTOCOL_UDP:
        return "UDP";
        break;
 
    default:
        return "UNKNOWN PROTOCOL";
        break;
    }
}
