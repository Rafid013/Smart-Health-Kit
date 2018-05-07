#include <ESP8266HTTPClient.h>

#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

const char *ssid =  "Rafid-Hamiz";
const char *pass =  "32636869";
const char* server = "api.thingspeak.com";


WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println();

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

  /*while(!client.connect(server, 80)) {
    Serial.println("Trying to connect to ThingSpeak");
  }
  Serial.println("Connected to ThingSpeak");*/

  ThingSpeak.begin( client );

   /*HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://api.thingspeak.com/channels.json");      //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   String body = "api_key=ZXPNI4ECJRXG21TI&field1=temperature&field2=blood_pressure&field3=heart_rate&public_flag=true";
   int httpCode = http.POST(body); 
   String payload = http.getString();                  //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection*/

   write2TSData( 490416, 1, 10, 2, 20, 3, 30);
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





