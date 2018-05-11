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
  while (NodeMCU.available() > 0) {
    String msg = NodeMCU.readString();
    msg.replace("\n", " ");
    msg.trim();
    if (msg.compareTo("0") == 0) {
      open_channel();
      NodeMCU.println(String(id_global) + "_" + read_api_global + "_" + write_api_global);
      NodeMCU.flush();

      Serial.println(id_global);
      Serial.println(read_api_global);
      Serial.println(write_api_global);
    }
  }
  /*while (NodeMCU.available() > 0) {
    msg_from_arduino = NodeMCU.parseInt();
    if (NodeMCU.read() == '\n') {
      if (msg_from_arduino == 0) {
        //clear_eeprom();
        open_channel();

        NodeMCU.println(id_global);
        //NodeMCU.println("\n");
        NodeMCU.flush();

        NodeMCU.println(read_api_global);
        //NodeMCU.println("\n");
        NodeMCU.flush();


        Serial.println(id_global);
        Serial.println(read_api_global);
        Serial.println(write_api_global);
      }
      else if (msg_from_arduino == 1) {
        temp = Serial.parseFloat();
        if (NodeMCU.read() == '\n') {
          if (temp != -1) {
            Serial.println(temp);
            ThingSpeak.writeField(id_global, 2, temp, write_api_global);
          }
        }
        bpm = NodeMCU.parseInt();
        if (NodeMCU.read() == '\n') {
          if (bpm != -1) {
            Serial.println(bpm);
            ThingSpeak.writeField(id_global, 3, bpm, write_api_global);
          }
        }

        sys_bp = Serial.parseInt();
        if (sys_bp != -1) {
          Serial.println(sys_bp);
          if (NodeMCU.read() == '\n') {
            dias_bp = Serial.parseInt();
            if (NodeMCU.read() == '\n') {
              String bp(sys_bp + "/" + dias_bp);
              ThingSpeak.writeField(id_global, 2, bp, write_api_global);
            }
          }
        }
      }
      else if (msg_from_arduino == 4) {
        //String id_to_send =  "" + id_global;

        NodeMCU.print(id_global);
        NodeMCU.println("\n");
        NodeMCU.flush();

        Serial.println(id_global);
        Serial.println(read_api_global);

        NodeMCU.print(read_api_global);
        NodeMCU.println("\n");
        NodeMCU.flush();
      }
    }
    }*/
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


