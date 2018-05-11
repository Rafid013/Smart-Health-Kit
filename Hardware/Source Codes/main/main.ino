#include <LiquidCrystal.h>
#include <EEPROM.h>



// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //lcd pin (4, 6, 11, 12, 13 ,14)



const int switchInput3 = 13;
const int switchInput2 = 11;
const int switchInput1 = 10;


volatile const int switchInterrupt = 19;
volatile unsigned long lastInterrupt = 0;

bool channel_created = false;
bool api_saved = false;

bool temp_value_valid = true;
bool heart_rate_value_valid = true;

float temperatures[10];
int heart_rate;
int sys_bp, dias_bp;

String id_global;
String read_api_global;
String write_api_global;


volatile int work_mode; //0 = temp, 1 = heart and blood, 2 = clear_eep, 3 = save, 4 = show api
volatile int prev_work_mode = -1;

/******************************************************************/
//Heart Rate related variables

//  Variables
int pulsePin = A2;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 49;                // pin to blink led at each beat
int fadePin = 12;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin



// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse

volatile int rate[10];                      // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P = 512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

/******************************************************************/


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



  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);
  /*if (digitalRead(switchInput3) == 1) {
    work_mode = 4;
    Serial.println("SHOW API KEY");
    }
    else if (digitalRead(switchInput1) == 0 && digitalRead(switchInput2) == 0) {
    work_mode = 0;
    Serial.println("TEMPERATURE");
    temp_value_valid = true;
    }
    else if (digitalRead(switchInput1) == 0 && digitalRead(switchInput2) == 1) {
    work_mode = 1;
    Serial.println("HEART RATE AND BLOOD PRESSURE");
    TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
    heart_rate_value_valid = true;
    }
    else {
    work_mode = 4;
    Serial.println("SHOW API KEY");
    }*/

  work_mode = 4;
  Serial.println("SHOW API KEY");
  initFromEEPROM();
}

void loop() {
  if (work_mode == 0) {
    calculate_temp();
  }
  else if (work_mode == 1) {
    if (QS == true) { // A Heartbeat Was Found
      // BPM and IBI have been Determined
      // Quantified Self "QS" true when arduino finds a heartbeat
      fadeRate = 255; // Makes the LED Fade Effect Happen, Set 'fadeRate' Variable to 255 to fade LED with pulse
      serialOutputWhenBeatHappens(); // A Beat Happened, Output that to serial.
      QS = false; // reset the Quantified Self flag for next time
    }

    ledFadeToBeat(); // Makes the LED Fade Effect Happen
    delay(20); //  take a break
  }
  else if (work_mode == 2) {
    if (!channel_created) {
      Serial3.println("0");
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
    /*Serial3.print(1);
      Serial3.println("\n");
      Serial3.flush();
      if (temp_value_valid) {
      /*float avgTemp = temperatures[0];
        for(int i = 1; i < 10; ++i) {
        avgTemp += temperatures[i];
        }
        avgTemp /= 10;*/

    /*Serial3.print(300);
      Serial3.println("\n");
      Serial3.flush();

      temp_value_valid = false;
      }
      else {
      Serial3.print(-1);
      Serial3.println("\n");
      Serial3.flush();
      }
      if (heart_rate_value_valid) {
      //Serial3.print(heart_rate);
      Serial3.print(400);
      Serial3.println("\n");
      Serial3.flush();

      //Serial3.print(sys_bp);
      Serial3.print(130);
      Serial3.println("\n");
      Serial3.flush();

      //Serial3.print(dias_bp);
      Serial3.print(80);
      Serial3.println("\n");
      Serial3.flush();

      heart_rate_value_valid = false;
      }
      else {
      Serial3.print(-1);
      Serial3.println("\n");
      Serial3.flush();

      Serial3.print(-1);
      Serial3.println("\n");
      Serial3.flush();

      Serial3.print(-1);
      Serial3.println("\n");
      Serial3.flush();
      }*/
  }
  else {
    /*if(!api_saved) {
      api_saved = true;
      Serial3.print(4);
      Serial3.println("\n");
      Serial3.flush();

      id_global = Serial3.readString();
      id_global.remove(id_global.length() - 1, 2);

      if(Serial3.read() == '\n') {
         read_api_global = Serial3.readString();
         if(Serial3.read() == '\n') return;
      }
      read_api_global.remove(read_api_global.length() - 1);
      Serial.println(id_global);
      Serial.println(read_api_global);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(id_global);
      lcd.setCursor(0, 1);
      lcd.println(read_api_global);
      delay(1000);*/
    if (api_saved) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(id_global);
      lcd.setCursor(0, 1);
      lcd.println(read_api_global);
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("CREATE A NEW");
      lcd.setCursor(0, 1);
      lcd.println("CHANNEL");
    }
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
      temp_value_valid = true;
    }
    else if (digitalRead(switchInput2) == 0 && digitalRead(switchInput1) == 1) {
      work_mode = 1;
      Serial.println("HEART RATE AND BLOOD PRESSURE"); //turn on timer INT for heart rate
      TIMSK2 = 0x02;
      heart_rate_value_valid = true;
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

  lcd.clear();
  lcd.print("Temp = ");
  lcd.print(tempc);
  delay(500); //Delay of 1 second for ease of viewing
}





void ledFadeToBeat()
{
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate, 0, 255);   //  keep LED fade value from going into negative numbers!
  analogWrite(fadePin, fadeRate);         //  fade LED
}

void interruptSetup()
{
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  //TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
  TCCR2B = 0x05;      //test value
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}


void serialOutputWhenBeatHappens()
{
  if (serialVisual == true) //  Code to Make the Serial Monitor Visualizer Work
  {
    Serial.print("*** Heart-Beat Happened *** ");  //ASCII Art Madness
    Serial.print("BPM: ");
    Serial.println(BPM);
    lcd.clear();
    lcd.print("BPM: ");
    lcd.print(BPM);
  }
  else
  {
    sendDataToSerial('B', BPM);  // send heart rate with a 'B' prefix
    sendDataToSerial('Q', IBI);  // send time between beats with a 'Q' prefix
  }
}

void sendDataToSerial(char symbol, int data )
{
  Serial.print(symbol);
  Serial.println(data);
}

ISR(TIMER2_COMPA_vect) //triggered when Timer2 counts to 124
{
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor
  sampleCounter += 1;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
  //  find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3) // avoid dichrotic noise by waiting 3/5 of last IBI
  {
    if (Signal < T) // T is the trough
    {
      T = Signal; // keep track of lowest point in pulse wave
    }
  }

  if (Signal > thresh && Signal > P)
  { // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 500)
  { // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) )
    {
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin, HIGH);                // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if (secondBeat)
      { // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for (int i = 0; i <= 9; i++) // seed the running total to get a realisitic BPM at startup
        {
          rate[i] = IBI;
        }
      }

      if (firstBeat) // if it's the first time we found a beat, if firstBeat == TRUE
      {
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        sei();                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }
      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable

      if (N <= 1000) {
        for (int i = 0; i <= 8; i++)
        { // shift data in the rate array
          rate[i] = rate[i + 1];                // and drop the oldest IBI value
          runningTotal += rate[i];              // add up the 9 oldest IBI values
        }

        rate[9] = IBI; // add the latest IBI to the rate array
        runningTotal += rate[9];                // add the latest IBI to runningTotal
      }
      else {
        for (int i = 0; i <= 8; i++)
        {
          runningTotal += rate[i];              // add up the 9 oldest IBI values
        }
      }
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(BPM);
      QS = true;                              // set Quantified Self flag
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal < thresh && Pulse == true)
  { // when the values are going down, the beat is over
    digitalWrite(blinkPin, LOW);           // turn off pin 13 LED
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500)
  { // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  sei();                                   // enable interrupts when youre done!
}// end isr


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


