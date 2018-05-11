#include <ESP8266HTTPClient.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include "json_parse.h"
#include <SoftwareSerial.h>


const char *ssid =  "Rafid-Hamiz";
const char *pass =  "32636869";
const char* server = "api.thingspeak.com";

String write_api_global;
String read_api_global;
int id_global;

HTTPClient http;    //Declare object of class HTTPClient

WiFiClient client;

int msg_from_arduino;

SoftwareSerial NodeMCU(D3, D2);

float temp;
int bpm;
int sys_bp;
int dias_bp;

void setup() {
  Serial.begin(9600);
  NodeMCU.begin(115200);

  pinMode(D3, INPUT);
  pinMode(D2, OUTPUT);

  WiFi.begin(ssid, pass);



  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());


  ThingSpeak.begin( client );
}

void loop() {
  // put your main code here, to run repeatedly:
  if (NodeMCU.available() >= 37) {
    Serial.println(NodeMCU.available());
    int availableByteCount = NodeMCU.available();
    String msg = "";
    int i;
    for ( i = 0; i < 38; i++)
    {
      char msgChar = NodeMCU.read();
      msg += msgChar;
      Serial.println(msg);
      Serial.println(NodeMCU.available());
    }
    msg.replace("\n", " ");
    msg.trim();
    Serial.println(msg);
    String command, id, key, value;
    int indexUnderScore = msg.indexOf("_");
    if (indexUnderScore != -1) {
      int lastUnderScore = msg.lastIndexOf("_");
      int colonIndex = msg.indexOf(":");
      command = msg.substring(0, indexUnderScore);
      Serial.println(command);
      id = msg.substring(indexUnderScore + 1, lastUnderScore);
      Serial.println(id);
      key = msg.substring(lastUnderScore + 1, colonIndex);
      Serial.println(key);
      value = msg.substring(colonIndex + 1);
      Serial.println(value);
    }
    else {
      command = msg;
    }
    if (command.compareTo("0") == 0) {
      open_channel();
      NodeMCU.println(String(id_global) + "_" + read_api_global + "_" + write_api_global);
      NodeMCU.flush();

      Serial.println(id_global);
      Serial.println(read_api_global);
      Serial.println(write_api_global);
    }
    else if (command.compareTo("1") == 0) {
      ThingSpeak.writeField(id.toInt(), 1, value, key.c_str());
    }
    else if (command.compareTo("2") == 0) {
      int indexSlash = value.indexOf("/");
      ThingSpeak.setField(2, value.substring(indexSlash + 1));
      ThingSpeak.setField(3, value.substring(0, indexSlash));
      ThingSpeak.writeFields(id.toInt(), key.c_str());
      //ThingSpeak.writeField(id.toInt(), 2, value.substring(indexSlash + 1), key.c_str());
      //delay(5000);
      //ThingSpeak.writeField(id.toInt(), 3, value.substring(0, indexSlash), key.c_str());
    }
  }
}


void open_channel() {
  http.begin("http://api.thingspeak.com/channels.json");      //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  String body = "api_key=9V8WTVQNBZJZR9UP&field1=temperature&field2=blood_pressure&field3=heart_rate";
  int httpCode = http.POST(body);
  String payload = http.getString();                  //Get the response payload

  //Serial.println(httpCode);   //Print HTTP return code
  //Serial.println(payload);    //Print request response payload

  http.end();  //Close connection

  id_global = get_id(payload).toInt();
  write_api_global = get_write_api(payload);
  read_api_global = get_read_api(payload);
}


