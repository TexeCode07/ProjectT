/*
C:\Tejas_ArduProject\60RA 
  11/25 

  gps fix,only when online will do call, restart added, added notification , 2 number sms and call, use init(),

  v17 - contact1 
  v18 - contact2
  v7  - set temp     ,v12 - temp alert   ,v3 - cur temp
  v8  - set humid    ,v13 - hum  alert   ,v4 - cur humi 
  v14 - motion alert ,mot but - v20
  v15 - sw   alert   ,sw  but - v6
  v0  - GPS
  v1  - alertdel

  // Hardware Connection:
  red 12, blue 15, green 13, motion 22, button 21 dht 0 //2,23,19,18,5,39
*/

#define uS_TO_S_FACTOR      1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP       30          /* Time ESP32 will go to sleep (in seconds) */

#define UART_BAUD           115200

#define MODEM_TX            27
#define MODEM_RX            26
#define MODEM_PWRKEY        4
#define MODEM_DTR           32
#define MODEM_RI            33
#define MODEM_FLIGHT        25
#define MODEM_STATUS        34

#define SD_MISO             2
#define SD_SCLK             14


#define LED_PIN             12

#define SerialMon Serial
#define SerialAT  Serial1

#define BLYNK_TEMPLATE_ID "TMPLgHiM3u9V"
//#define BLYNK_TEMPLATE_NAME "Arduino GSM"
#define BLYNK_TEMPLATE_NAME "RA"
#define BLYNK_AUTH_TOKEN "xxxxxx"   // RA user
// #define BLYNK_AUTH_TOKEN "xxxxx" // arduino GSM user 

#define BLYNK_HEARTBEAT 60  // Default heartbeat interval for GSM is 60
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;

//GSM
char apn[]    = "";
char user[]   = "";
char pass[]   = "";
#define GSM_PIN ""
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb

#include <TinyGsmClient.h> 
#include <BlynkSimpleTinyGSM.h>  //https://github.com/blynkkk/blynk-library
#include <WiFi.h>

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG Serial
#define TINY_GSM_YIELD() { delay(2); }

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


#define USING_TINYGPS_LIBRARY   
#include <TinyGPS++.h>
TinyGPSPlus gps;

float latitude,longitude;

BlynkTimer timer;

bool modemOn = false;

bool obtainLoc = false;
#define red          12    //hum
#define blue         15    //button1 / motion
#define green        13    //temp

#define motionPin    22    //motion
#define button1      21    //sw  
#define smsnotibtn   5

#include <DHT.h>
#define DHT11_PIN 0        
#define DHTTYPE DHT11
DHT dht(DHT11_PIN, DHTTYPE);

WidgetLED buttled(V15);  WidgetLED motionLed(V14);  WidgetLED smsnotiled(V16);
WidgetLED templed(V12);  WidgetLED humled(V13);

int alertDel=0;
float sethum, settemp, t, h;
String contact1 = "";
String contact2 = "";

volatile bool motionDetected = false; 
// volatile bool motionflag = false;  volatile int  motionValue   = 0;
volatile bool buttflag = false;       volatile int  buttValue   = 0;
volatile bool smsnotiflag = false;    volatile int  smsnotiValue   = 0;

int motbut,swbut;

unsigned long lastAlertTime      = 0;
unsigned long delayBetweenAlerts = 0;
unsigned long currentTime        = 0;

unsigned long previousMillis = 0;
unsigned long interval = 1000*60*720;

unsigned long bootme             = 0;

bool alert1 = false;    bool alert2 = false;      // humid  ,temp
bool alert3 = false;    bool alert4 = false;      // motion ,button

bool callFlag = true;

bool reply    = false;


// Interrupt Service Routine (ISR)
void IRAM_ATTR handleMotion() {
  motionDetected = true;
}

bool containsOnlyNumbers(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (!isDigit(str[i])) {
      return false;
    }
  }
  return true;
}

//sync new values
BLYNK_WRITE(V17) {
  contact1  = param.asStr(); 
  if (containsOnlyNumbers(contact1)) {
    Serial.println("Contact1 contains only numbers.");
  } else {
    contact1="";
    Blynk.virtualWrite(V17, "");
  }
}
BLYNK_WRITE(V18) {
  contact2  = param.asStr(); 
  if (containsOnlyNumbers(contact2)) {
    Serial.println("Contact2 contains only numbers.");
  } else {
    contact2="";
    Blynk.virtualWrite(V18, "");
  } 
}
BLYNK_WRITE(V7) {
  settemp   = param.asInt();  
}
BLYNK_WRITE(V8) {
  sethum    = param.asInt();  
}
BLYNK_WRITE(V1) {
  alertDel  = param.asInt();
  // Calculate the delay in milliseconds based on the alertDel value in minutes
  delayBetweenAlerts = alertDel * 60 * 1000;
  // delayBetweenAlerts = alertDel * 5 * 1000;
  Serial.print("time:");Serial.println(delayBetweenAlerts);
}
BLYNK_WRITE(V20) {
  motbut    = param.asInt();
  if(motbut == 0){
     alert3 = false;
  }
}
BLYNK_WRITE(V6) {
  swbut     = param.asInt();
  if(swbut == 0){
     alert4 = false;
  }
}

BLYNK_CONNECTED(){
  Serial.println("i con");
  digitalWrite(LED_PIN, HIGH);

  // sync all data at start
  // Blynk.syncAll();
  Blynk.syncVirtual(V17);    Blynk.syncVirtual(V18);       Blynk.syncVirtual(V7);      Blynk.syncVirtual(V8);   Blynk.syncVirtual(V1); 
  Blynk.syncVirtual(V6);     Blynk.syncVirtual(V20);
  // make all alert off
  Blynk.virtualWrite(V14, 0);Blynk.virtualWrite(V15, 0);   Blynk.virtualWrite(V13, 0); Blynk.virtualWrite(V12, 0);  Blynk.virtualWrite(V16, 0);
}

BLYNK_DISCONNECTED(){
  Serial.println("i discon in BLYNK_DISCONNECTED ");
  digitalWrite(LED_PIN, LOW);
  powerOnModem();   // on simcom
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  pinMode(red,    OUTPUT);   digitalWrite(red,  LOW);
  pinMode(blue,   OUTPUT);   digitalWrite(blue, LOW);
  pinMode(green,  OUTPUT);   digitalWrite(green,LOW);

  //while(1);   // this is to block code  okokok

  dht.begin();

  pinMode(motionPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionPin), handleMotion, FALLING);

  pinMode(button1,   INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1),   button1Change,   CHANGE);

  pinMode(smsnotibtn,  INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(smsnotibtn),smsnotibtnChange,   CHANGE);
  delay(10);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); // Start AT Mode
  delay(100);
  
  startModem();

  modem.enableGPS();
  // Download Report GPS NMEA-0183 sentence , NMEA TO AT PORT
  modem.sendAT("+CGPSINFOCFG=1,31");
  modem.waitResponse(30000);
   
  
  Blynk.begin(auth, modem, apn, user, pass); // Initialize Blynk 
  // Blynk.begin(auth, modem, apn, user, pass, "Blynk.cloud", 8080); // Initialize Blynk     YOU CHANGE THIS WAS ACTIVE

  blinkLED(red);   blinkLED(red);   blinkLED(red);

  digitalWrite(red,  LOW);digitalWrite(blue,  LOW);digitalWrite(green,  LOW); //fdefault at start                                  

  timer.setInterval(1000L, SendDhtData);
  timer.setInterval(1000L, checkMotButt);
  timer.setInterval(1000L, checkAlerts);
}


void loop() {
  // put your main code here, to run repeatedly:
  sendGPS();
  Blynk.run();    
  timer.run();

}

bool pa1,pa2,pa3,pa4=false;

void  checkAlerts(){
  currentTime = millis();
  if (alert1 || alert2 || alert3 || alert4) {
    String message = "";
    if (alert1) {
      message += "T="+String(t)+"; H="+String(h)+" \nTs="+String(settemp)+" Hs="+String(sethum)+" ";
      if(pa1 == false){
        callFlag =true;
        // Serial.println("h");
      }
    }
    if(alert2){
      message += "T="+String(t)+"; H="+String(h)+" \nTs="+String(settemp)+" Hs="+String(sethum)+" ";
      if(pa2 == false){
        callFlag =true;
        // Serial.println("n");
      }
    }
    
    if (alert3) {
      message += "Motion Detected! ";
      if(pa3 == false){
        callFlag =true; 
      }
    }

    if (alert4) {
      message += "Switch Pressed! ";
      if(pa4 == false){
        callFlag =true; 
      }
    }

    if(obtainLoc){
      message += "https://www.google.com/maps?q="+String(latitude,6)+","+String(longitude,6);
    }else{
      message += "GPS Location Not Found Yet!";
    }

 
    if(callFlag == true){
      if(Blynk.connected()){
        String str = "+1"+contact1;
        String str2= "+1"+contact2;
        Serial.println(message);
        letsSMSCALL(str,  message);   
        delay(100);
    // //     // letsSMSCALL(str2, message);   //RA ,it was note i remove 
    // //     // delay(100);
        callFlag = false;
        lastAlertTime = currentTime;  // Update the last alert time to the current time
      }   
    }
      
  }
  pa1= alert1;
  pa2= alert2;
  pa3= alert3;
  pa4= alert4;

  if(!alert1 && !alert2 && !alert3 && !alert4){
    pa1= false;pa2= false;pa3= false;pa4= false;
  }
  if (currentTime - lastAlertTime >= delayBetweenAlerts) {
    pa1= false;pa2= false;pa3= false;pa4= false;
  }
}

void checkMotButt(){
  if(buttflag){
    if(buttValue){
      Serial.println("swpress detected!"); 
      digitalWrite(blue,HIGH);
      buttled.on();
      Blynk.syncVirtual(V6);
      if(swbut ==1){
        alert4 = true; 
      }
    }else{
      buttled.off();
      digitalWrite(blue,LOW);
    }
    buttflag = false;
  }

  if(smsnotiflag){
    if(smsnotiValue){
      Serial.println("smsnotibtn detected!"); 
      smsnotiled.on();
      Blynk.syncVirtual(V16);
 
    }else{
      smsnotiled.off();
    }
    smsnotiflag = false;
  }

  if (motionDetected) {
    motionDetected = false; // Reset the flag
    Serial.println("Motion detected!");
    digitalWrite(blue,HIGH);
    motionLed.on();
    Blynk.syncVirtual(V14);
    if(motbut ==1){
      alert3 = true; 
    }
    delay(100);
    motionLed.off();
    digitalWrite(blue,LOW);
  }
  
}

void button1Change() {
  buttValue = !digitalRead(button1);
  buttflag = true;
}

void smsnotibtnChange() {
  smsnotiValue = !digitalRead(smsnotibtn);
  smsnotiflag  = true;
}

void SendDhtData() {
  //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  //-----------------------------------------------------------------------
  //Read temperature as Celsius (the default)
  t = dht.readTemperature(true);
  //-----------------------------------------------------------------------
  //Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
    return;
  }
  
  //Serial.println("Here i send sensor");
  //Serial.print("Temperature: ");  Serial.print(t);
  //Serial.print("   Humidity: ");  Serial.print(h);
  //Serial.print("   heatIndex: "); Serial.println(hic);
  //Serial.print("Set temp");       Serial.print(settemp);
  //Serial.print("   Set hum: ");   Serial.println(sethum);
  //Serial.println(contact1);
  //Serial.print(contact2);

  //Serial.println("------------------------------------------");
 
  if( h > sethum && t > settemp){
    humled.on();                           templed.on();
    digitalWrite(green, HIGH);             digitalWrite(red, HIGH);
    alert1 = true;                         alert2 = true;  
  }else if(h > sethum){
    humled.on();                           templed.off(); 
    digitalWrite(green, LOW);              digitalWrite(red, HIGH);
    alert1 = true;                         alert2 = false; 
  }else if(t > settemp){
    humled.off();                          templed.on();
    digitalWrite(green, HIGH);             digitalWrite(red, LOW);
    alert1 = false;                        alert2 = true;  
  }else{
    humled.off();                          templed.off();
    digitalWrite(green, LOW);              digitalWrite(red, LOW);
    alert1 = false;                        alert2 = false; 
  }

  // // You can send any value at any time.
  // // Plase don't send more that 10 values per second.
  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V4, h);
}

void letsSMSCALL(String cont, String msg) {
  if(smsnotiValue){ 
    if(!modem.sendSMS(cont, msg)) {
      Serial.println("Failed to Send SMS");
      // ////////////////////////////////////////////////////////////////////////////////////////////
      //  Serial.println("will call now");      //ra put note off call  OK?
      //   if (!modem.callNumber(cont)) {                
      //     Serial.println("Failed Call22222222");
      //   } 
      //   else {
      //     Serial.println("Call Done");
      //     delay(15000); 
      //     modem.callHangup(); // i remove this it keep boot why it boot 
      //     delay(15000); 
      //     // callRetryCount = 0; // Reset retry count if successful
      //   }
      // /////////////////////////////////////////////////////////////////////////////////////////////////
      // Serial.println("ip:");
      // Serial.println(modem.getLocalIP());
    } else {
      // Serial.println("send SMs success");
    }
  }else{
    Serial.println("SMS Notify off");
  }
}


void sendGPS(){
  while (SerialAT.available()) {
    if (gps.encode(SerialAT.read())) {
      if (gps.location.isValid()) {
        latitude  = gps.location.lat();
        longitude = gps.location.lng();
        // Serial.print(F("Latitude:"));     Serial.print(latitude,6);      Serial.print(F("   Longitude:"));  Serial.println(longitude,6);
        Blynk.virtualWrite(V0,gps.location.lng(),gps.location.lat());
        obtainLoc= true;
      }    
    }
  }
}


void startModem(){
  /*
    MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
    otherwise the modulator will not reply when the command is sent
  */

  pinMode(MODEM_PWRKEY, OUTPUT);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(300); //Need delay
  digitalWrite(MODEM_PWRKEY, LOW);

  /*
    MODEM_FLIGHT IO:25 Modulator flight mode control,
    need to enable modulator, this pin must be set to high
  */
  pinMode(MODEM_FLIGHT, OUTPUT);  
  digitalWrite(MODEM_FLIGHT, HIGH);
  
  int i = 40;
  Serial.print(F("\r\n# Startup #\r\n"));
  Serial.print(F("# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
  while (i) {
    SerialAT.println(F("AT"));

    // Show the User: we are doing something.
    Serial.print(F("."));
    delay(500);

    // Did the Modem send something?
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      Serial.print("\r\n# Response:\r\n" + r);
      if ( r.indexOf("OK") >= 0 ) {
        reply = true;
        break;;
      } else {
        Serial.print(F("\r\n# "));
      }
    }

    // Did the User try to send something? Maybe he did not receive the first messages yet. Inform the User what is happening
    if (Serial.available() && !reply) {
      Serial.read();
      Serial.print(F("\r\n# Modem is not yet online."));
      Serial.print(F("\r\n# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
    }

    // On the 5th try: Inform the User what is happening
    if(i == 35) {
      Serial.print(F("\r\n# Modem did not yet answer. Probably Power loss?\r\n"));
      Serial.print(F("# Sending \"AT\" to Modem. Waiting for Response\r\n# "));
    }
    delay(500);
    i--;
  }
  Serial.println(F("#\r\n"));
 
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if (!modem.init()) {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
  }
  // Stop GPS Server
  modem.sendAT("+CGPS=0");
  modem.waitResponse(30000);

  // Configure GNSS support mode
  modem.sendAT("+CGNSSMODE=15,1");
  modem.waitResponse(30000);

  // Configure NMEA sentence type
  modem.sendAT("+CGPSNMEA=200191");
  modem.waitResponse(30000);

  // Set NMEA output rate to 1HZ
  modem.sendAT("+CGPSNMEARATE=1");
  modem.waitResponse(30000);

  // Enable GPS
  modem.sendAT("+CGPS=1");
  modem.waitResponse(30000);

  // Download Report GPS NMEA-0183 sentence , NMEA TO AT PORT
  modem.sendAT("+CGPSINFOCFG=1,31");
  modem.waitResponse(30000);


  //Disable NMEA OUTPUT
  // modem.sendAT("+CGPSINFOCFG=0,31");
  // modem.waitResponse(30000);
}

void powerOnModem(){
  /*
    MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
    otherwise the modulator will not reply when the command is sent
  */
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(300); //Need delay
  digitalWrite(MODEM_PWRKEY, LOW);

  // Check if modem responds to AT commands
  int retries = 10;
  while (retries--) {
      if (modem.testAT()) {
          Serial.println("Modem powered on successfully!");
          Blynk.begin(auth, modem, apn, user, pass);
          modemOn = true;
          return;
      }
      delay(1000);
  }

  Serial.println("Modem power on failed!");
  modemOn = false;
  powerOnModem();
}

void powerOffModem() {
  Serial.println("Powering off modem...");
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(2600); // Need delay
  digitalWrite(MODEM_PWRKEY, LOW);

  // Check if modem has powered off
  int retries = 5;
  while (retries--) {
      if (!modem.testAT()) {
          Serial.println("Modem powered off successfully!");
          modemOn = false;
          return;
      }
      delay(1000);
  }

  Serial.println("Modem power off failed!");
  modemOn = true;
  powerOffModem();
}

void blinkLED(int pin) {
  digitalWrite(pin, LOW);
  delay(1500); // Half-second delay
  digitalWrite(pin, HIGH);
  delay(1500); // Half-second delay
}