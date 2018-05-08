#include <ESP8266HTTPClient.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include "json_parse.h"
#include <EEPROM.h>


const char *ssid =  "Rafid-Hamiz";
const char *pass =  "32636869";
const char* server = "api.thingspeak.com";

char write_api_global[17];
char read_api_global[17];
int id_global;

WiFiClient client;

int eepromAddr = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, pass);

  EEPROM.begin(512);


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

  /*for (int i = 0 ; i < 512; i++) {
    EEPROM.put(i, 0);
    EEPROM.commit();
  }*/

  char c1, c2;
  EEPROM.get(eepromAddr, c1);
  EEPROM.get(eepromAddr + sizeof(char), c2);

  if(c1 != 'n' || c2 != 'e') {
  
    HTTPClient http;    //Declare object of class HTTPClient
  
    http.begin("http://api.thingspeak.com/channels.json");      //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    String body = "api_key=ZXPNI4ECJRXG21TI&field1=temperature&field2=blood_pressure&field3=heart_rate";
    int httpCode = http.POST(body); 
    String payload = http.getString();                  //Get the response payload
  
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
  
    http.end();  //Close connection
  
    Serial.println(get_id(payload));
    Serial.println(get_write_api(payload));
    Serial.println(get_read_api(payload));
  
    EEPROM.put(eepromAddr, "ne"); //ne = not empty
    EEPROM.commit();
    eepromAddr += 2;
  
    String id = get_id(payload);
    EEPROM.put(eepromAddr, id.toInt());
    EEPROM.commit();
    id_global = id.toInt();
    eepromAddr += sizeof(int);
  
  
    
    String write_api = get_write_api(payload);
    memcpy(write_api_global, write_api.c_str(), write_api.length() + 1);
    for(int i = 0; i < 17; ++i) { 
      EEPROM.put(eepromAddr + i, write_api_global[i]);
      EEPROM.commit();
    }
    eepromAddr += write_api.length() + 1;
  
  
    String read_api = get_read_api(payload);
    memcpy(read_api_global, read_api.c_str(), read_api.length() + 1);
    for(int i = 0; i < 17; ++i) { 
      EEPROM.put(eepromAddr + i, read_api_global[i]);
      EEPROM.commit();
    }
    eepromAddr += read_api.length() + 1;
  }

  else {
    eepromAddr += 2;
    EEPROM.get(eepromAddr, id_global);
    eepromAddr += sizeof(int);
    
    for(int i = 0; i < 16; ++i) {
        EEPROM.get(eepromAddr + i, write_api_global[i]);
    }
    write_api_global[16] = '\0';
    eepromAddr += (sizeof(char)*17);

    for(int i = 0; i < 16; ++i) {
        EEPROM.get(eepromAddr + i, read_api_global[i]);
    }
    read_api_global[16] = '\0';
    eepromAddr += (sizeof(char)*17);

    Serial.println(id_global);
    Serial.println(write_api_global);
    Serial.println(read_api_global);
  }
 

 //write2TSData( 490416, 1, 10, 2, 20, 3, 30);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

int write2TSData( long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, long field2Data, unsigned int TSField3, long field3Data ){

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, "G5UH5TQDJKGIRTDX");
  return writeSuccess;
}





