#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "PPG.h"



// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //lcd pin (4, 6, 11, 12, 13 ,14)



const int switchInput3 = 13;
const int switchInput2 = 11;
const int switchInput1 = 10;

unsigned long bp_loop = millis();


volatile const int switchInterrupt = 19;
volatile unsigned long lastInterrupt = 0;

bool channel_created = false;
bool api_saved = false;

bool data_saved = false;

bool firstTempReading = true;

float temperatures[10];

String id_global;
String read_api_global;
String write_api_global;


volatile int work_mode; //0 = temp, 1 = heart and blood, 2 = clear_eep, 3 = save, 4 = show api
volatile int prev_work_mode = -1;

/******************************************************************/
//Temperature related variables

const int tempSensor = A0; // Assigning analog pin A1 to variable 'sensor'

float tempc;  //variable to store temperature in degree Celsius
float tempf;  //variable to store temperature in Fahreinheit
float voutTemp;  //temporary variable to hold temperature sensor reading
/******************************************************************/




void setup() {
  pinMode(switchInput1, INPUT);
  pinMode(switchInput2, INPUT);
  pinMode(switchInput3, INPUT);
  attachInterrupt(digitalPinToInterrupt(switchInterrupt), switch_data_arrival_ISR, CHANGE);

  //EEPROM.begin(512);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial3.begin(115200);

  pinMode(tempSensor, INPUT); // Configuring pin A1 as input

  ppg_setup();

  work_mode = 4;
  Serial.println("SHOW API KEY");
  initFromEEPROM();
}

void loop() {
  if (work_mode == 0) {
    calculate_temp();
  }
  else if (work_mode == 1) {
    if (millis() - bp_loop < 30000) {
      ppg_operation();
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BPM: " + String((int)newBPMValue));
      lcd.setCursor(0, 1);
      String bp = "SBP/DBP: " + String(SBP) + "/" + String(DBP);
      lcd.print(bp);
      delay(1000);
    }
  }
  else if (work_mode == 2) {
    if (!channel_created) {
      String msg_to_send = "0";
      while (msg_to_send.length() != 37) {
        msg_to_send += " ";
      }
      Serial3.println(msg_to_send);
      Serial3.flush();

      while (Serial3.available() <= 0);
      String ret_msg = Serial3.readString();
      ret_msg.replace("\n", " ");
      ret_msg.trim();

      int index1 = ret_msg.indexOf("_");
      int index2 = ret_msg.lastIndexOf("_");

      id_global = ret_msg.substring(0, index1);
      read_api_global = ret_msg.substring(index1 + 1, index2);
      write_api_global = ret_msg.substring(index2 + 1);
      Serial.println(id_global);
      Serial.println(read_api_global);
      Serial.println(write_api_global);

      saveInEEPROM();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(id_global);
      lcd.setCursor(0, 1);
      lcd.print(read_api_global);
      channel_created = true;
    }
  }
  else if (work_mode == 3) {
    if (!data_saved && api_saved) {
      if (prev_work_mode == 0) {
        float avgTemp = 0;
        for (int i = 0; i < 10; ++i) {
          avgTemp += temperatures[i];
        }
        avgTemp /= 10;
        String msg_to_send = "1_" + id_global + "_" + write_api_global + ":" + avgTemp;
        while (msg_to_send.length() != 37) {
          msg_to_send += " ";
        }
        Serial3.println(msg_to_send);
        Serial3.flush();
      }
      else if (prev_work_mode == 1) {
        String msg_to_send = "2_" + id_global + "_" + write_api_global + ":" + (int)newBPMValue + "/" + SBP + "/" + DBP;
        while (msg_to_send.length() != 37) {
          msg_to_send += " ";
        }
        Serial3.println(msg_to_send);
        Serial3.flush();
      }
      data_saved = true;
    }
  }
  else {
    if (api_saved) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(id_global);
      lcd.setCursor(0, 1);
      lcd.print(read_api_global);
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CREATE A NEW");
      lcd.setCursor(0, 1);
      lcd.print("CHANNEL");

      Serial.println("CREATE A NEW CHANNEL");
    }
    delay(1000);
  }
}




void switch_data_arrival_ISR() {
  //noInterrupts();
  //EIFR = 0xFF;
  //detachInterrupt(digitalPinToInterrupt(switchInterrupt));
  if (millis() - lastInterrupt > 500) {
    prev_work_mode = work_mode;
    TIMSK2 = 0x00; //turn off timer INT for heart ratexxxx
    if (digitalRead(switchInput3) == 1) {
      work_mode = 4;
      Serial.println("SHOW API KEY");
    }
    else if (digitalRead(switchInput2) == 0 && digitalRead(switchInput1) == 0) {
      work_mode = 0;
      Serial.println("TEMPERATURE");
    }
    else if (digitalRead(switchInput2) == 0 && digitalRead(switchInput1) == 1) {
      work_mode = 1;
      Serial.println("HEART RATE AND BLOOD PRESSURE");
      Serial.flush();
      firstPTT = false;
      firstBPM = false;
      bp_loop = millis();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calculating...");
    }
    else if (digitalRead(switchInput2) == 1 && digitalRead(switchInput1) == 0) {
      work_mode = 2;
      Serial.println("CREATE CHANNEL");
      channel_created = false;
    }
    else if (digitalRead(switchInput2) == 1 && digitalRead(switchInput1) == 1) {
      work_mode = 3;
      Serial.println("SAVE");
      Serial.flush();
      data_saved = false;
    }
    lastInterrupt = millis();
    //attachInterrupt(digitalPinToInterrupt(switchInterrupt), switch_data_arrival_ISR, RISING);
    //interrupts();
  }
}



void calculate_temp() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);

  voutTemp = analogRead(tempSensor);
  voutTemp = (voutTemp * 5000) / (1024 * 9.2);
  tempc = voutTemp; // Storing value in Degree Celsius
  tempf = (voutTemp * 1.8) + 32; // Converting to Fahrenheit

  if (firstTempReading) {
    for (int i = 0; i < 10; ++i) {
      temperatures[i] = tempc;
    }
    firstTempReading = false;
  }
  else {
    for (int i = 0; i < 9; ++i) {
      temperatures[i] = temperatures[i + 1];
    }
    temperatures[9] = tempc;
  }

  lcd.clear();
  lcd.print("Temp = ");
  lcd.print(tempc);
  delay(500); //Delay of 1 second for ease of viewing
}






void saveInEEPROM() {
  int eepromAddr = 0;

  EEPROM.put(eepromAddr, "ne"); //ne = not empty
  eepromAddr += sizeof(char) * 2;

  char *id = new char[id_global.length() + 1];
  memcpy(id, id_global.c_str(), id_global.length() + 1);
  for (int i = 0; i < id_global.length() + 1; ++i) {
    EEPROM.put(eepromAddr + i, id[i]);
  }
  eepromAddr += id_global.length() + 1;

  char *read_api = new char[read_api_global.length() + 1];
  memcpy(read_api, read_api_global.c_str(), read_api_global.length() + 1);
  for (int i = 0; i < read_api_global.length() + 1; ++i) {
    EEPROM.put(eepromAddr + i, read_api[i]);
  }
  eepromAddr += read_api_global.length() + 1;

  char *write_api = new char[write_api_global.length() + 1];
  memcpy(write_api, write_api_global.c_str(), write_api_global.length() + 1);
  for (int i = 0; i < write_api_global.length() + 1; ++i) {
    EEPROM.put(eepromAddr + i, write_api[i]);
  }
  eepromAddr += write_api_global.length() + 1;
  api_saved = true;
}

void initFromEEPROM() {
  int eepromAddr = 0;
  char c1, c2;
  EEPROM.get(eepromAddr, c1);
  EEPROM.get(eepromAddr + sizeof(char), c2);
  eepromAddr += sizeof(char) * 2;
  if (c1 != 'n' || c2 != 'e') {
    api_saved = false;
  }
  else {
    api_saved = true;
    char *id = new char[7];
    for (int i = 0; i < 7; ++i) {
      EEPROM.get(eepromAddr + i, id[i]);
    }
    eepromAddr += 7;
    id_global = String(id);

    char *read_api = new char[17];
    for (int i = 0; i < 17; ++i) {
      EEPROM.get(eepromAddr + i, read_api[i]);
    }
    eepromAddr += 17;
    read_api_global = String(read_api);

    char *write_api = new char[17];
    for (int i = 0; i < 17; ++i) {
      EEPROM.get(eepromAddr + i, write_api[i]);
    }
    eepromAddr += 17;
    write_api_global = String(write_api);

    Serial.println("IN EEP");
    Serial.println(id);
    Serial.println(read_api);
    Serial.println(write_api);
  }
}


