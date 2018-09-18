#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "OneButton.h"
#include <SPI.h>
#include <Timer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp8266.h>
#include <math.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Servo.h>
#include "probe.h"
#define LCD
/********************************************************************/
//probe´s in probe.h 
/********************************************************************/
// smoker probe
float A = A_ET73;      // "A" Coeffecient in Steinhart-Hart Equation
float B = B_ET73;      // "B"
float C = C_ET73;      // "C"
int rn1 = rn_ET73;
//meat probe
float A2 = A_ET73;      // "A" Coeffecient in Steinhart-Hart Equation
float B2 = B_ET73;      // "B"
float C2 = C_ET73;      // "C"
int rn2 = rn_ET73;
//meat2 probe
float A3 = A_FANTN;      // "A" Coeffecient in Steinhart-Hart Equation
float B3 = B_FANTN;      // "B"
float C3 = C_FANTN;      // "C"
int rn3 = rn_FANTN;
//meat3 probe
float A4 = A_FANTN;      // "A" Coeffecient in Steinhart-Hart Equation
float B4 = B_FANTN;      // "B"
float C4 = C_FANTN;      // "C"
int rn4 = rn_FANTN;
/********************************************************************/
/********************************************************************/
/********************************************************************/
float vin = 3.285;              //  DC Voltage as measured with DMM between +3.3 and GND
float r2 = 47070;                // Resistance in ohms of your fixed resistor
float r22 = 47130;                // Resistance in ohms of your fixed resistor
float r23 = 46920;                // Resistance in ohms of your fixed resistor
float r24 = 47370;                // Resistance in ohms of your fixed resistor
float r25 = 47160;                // Resistance in ohms of your fixed resistor bestÃ¼ckt aber nicht verwendet Multiplexer kanal 4(5)
float r27 = 47330;                // Resistance in ohms of your fixed resistor bestÃ¼ckt aber nicht verwendet Multiplexer kanal 6(7)
/********************************************************************/
/********************************************************************/
/********************************************************************/
// ThingSpeak und Blynk Settings Grilloino
//blynk
char auth[] = "--";
//thingspeak
const char* host = "api.thingspeak.com";
const int channelID = 66;
String writeAPIKey = "--"; // write API key for your ThingSpeak Channel
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
String vers = "V3.1";
/********************************************************************/
/********************************************************************/
int lowTemp;              //  Low limit warning for smoker temp
int highTemp;            // High limit warning for smoker temp
int setPoint = 110;
int meatDone = 94;            // Temperature at which your meat is done
int meatDone2 = 94;            // Temperature at which your meat is done
int meatDone3 = 94;            // Temperature at which your meat is done
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
//Pins zuweisen Layout v3
#define pushPin 14              //D3 connect Fet or Transistor for fan
#define pA 15                   //D8 Multiplexer A
#define pB 13                   //D7 Multiplexer B
#define pC 12                    //D6 Multiplexer C
// WIFI manager Triger Pin
#define TRIGGER_PIN 10
int servoPin = 16;
//Buttons
OneButton mbutton(2, true);    //D6 Menue Button
OneButton sbutton(0, true);    //D4 select button
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
////Pins zuweisen Layout v2
//#define pushPin 0               //D3 connect Fet or Transistor for fan
//#define pA 15                   //D8 Multiplexer A
//#define pB 13                   //D7 Multiplexer B
//#define pC 12                    //D6 Multiplexer C
// WIFI manager Triger Pin
//#define TRIGGER_PIN 10
//int servoPin = 16;
////Buttons
//OneButton mbutton(14, true);    //D6 Menue Button
//OneButton sbutton(2, true);    //D4 select button

LiquidCrystal_I2C lcd(0x3F, 16, 2); //D2 & D1
Servo myservo;  // create servo object to control a servo
WiFiClient client;
WiFiServer server(80);
WidgetLED FAN(V9);
WidgetLED KLAPPE(V20);
double highTempd;
double setPointd;
int pushMode = 0;               // Pushingbox 1 = on 0= off
int logMode = 1;                // Thingspeak Loggin 1 = on 0= off
int httpMode = 1;               // Http Server 1 = on 0= off
int menue = 0;                  // Menue off
int displaymode = 1;           // Menue pages
int logstate;
int pushstate;
int httpstate;
int idleMode = 0;
int idlestate;
int val;
int val2;
int valservo = 0;
int lowlim = 0;               // Values below this will considered erroneous
float air;                      //  Smoker temperature
float meat;                     //   Meat temperature
float meat2;                     //   Meat temperature
float meat3;                     //   Meat temperature
int probecount = 2;
int sensorValue = 0;
float volt;
float volt2;
float offset = -0.35;
float Acalc;
float Bcalc;
float Ccalc;
int rn;
float a0;
float v0;
float r0;
float rref;
float logr0;
float k0;
float f0;
int tempDown;
int tempUp;
double setPointd2;
int meatUp;
int meatDown;
int meatUp2;
int meatDown2;
int meatUp3;
int meatDown3;

Timer t;
double Output, Input;
double Output2, Input2;
double Kp = 30, Kp2 = 180, Ki = 0.005, Kd = 0.5;
PID myPID(&Input, &Output, &setPointd, Kp, Ki, Kd, DIRECT);
PID myPID2(&Input2, &Output2, &setPointd2, Kp2, Ki, Kd, DIRECT);
//*******************************************************************
//SETUP
//*******************************************************************
void setup() {
  myPID.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  pinMode(pA, OUTPUT);
  pinMode(pB, OUTPUT);
  pinMode(pC, OUTPUT);
  // pusher init
  pinMode(pushPin, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  digitalWrite(pushPin, LOW);
  //LCD init
#ifdef LCD
  lcd.begin();
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.home();
  lcd.print("-Grilloino 2000-");
  lcd.setCursor(0, 1);
  lcd.print(">>>>>>");
  lcd.print(vers);
  lcd.print("<<<<<<");
  #endif

  
  Serial.begin(115200);
  Serial.println("-Grilloino 2000-");
  delay(3000);
  Serial.println();
  WiFi.begin();
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("connected...yeey :)");
    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.print("Connected!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    #endif
    delay(1000);
    Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
    Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
    while (Blynk.connect() == false) {
      // Wait until connected
      Serial.println("Try to connect to Blynk ");
      #ifdef LCD
      lcd.clear();
      lcd.home();
      lcd.print("Try to connect");
      lcd.setCursor(0, 1);
      lcd.print("to Blynk");
      #endif
    }
    Serial.println("Connected to Blynk server");
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.print("Connected");
    lcd.setCursor(0, 1);
    lcd.print("to Blynk");
    #endif
    delay(1000);
  }
  else {
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.print("No WiFi");
    lcd.setCursor(0, 1);
    lcd.print("No Blynk");
    delay(1000);
    #endif
  }
  server.begin();
  t.every(2000, readSensors);
  t.every(3000, updateDisplay);
  t.every(60000, sendPush);
  t.every(10000, sendData);
  t.every(3500, pushHeat);
  t.every(3500, checkVolt);
  // link the button 1 functions.
  mbutton.attachClick(click1); // --
  mbutton.attachDoubleClick(doubleclick1); // menue
  mbutton.attachDuringLongPress(longPress1); // --
  // link the button 2 functions.
  sbutton.attachClick(click2); // ++
  sbutton.attachDuringLongPress(longPress2); //++
  sbutton.attachDoubleClick(doubleclick2); // menue
  myservo.attach(servoPin);  // attaches the servo on GIO2 to the servo object
  for (int i = 0; i < 180; i++) {
    myservo.write(i);
    delay(10);
  }
  for (int i = 180; i > 0; i--) {
    myservo.write(i);
    delay(10);
  }


  myservo.write(0);
}
void loop() {
  // is configuration portal requested?#
  wifiConfig();

  //Main Loop
  lowTemp = setPoint - 8;
  highTemp = setPoint + 8;
  highTempd = highTemp;
  setPointd = setPoint;
  setPointd2 = setPointd + 2;
  mbutton.tick();
  sbutton.tick();
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
  if (0 == menue)
  {
    t.update();
    updatehttp();
  }
}

BLYNK_WRITE(V4)  // target temp down
{
  tempDown = param.asInt();
  if (tempDown == 1) {
    tempDownFunc();
  }
}
BLYNK_WRITE(V5) // target temp up
{
  tempUp = param.asInt();
  if (tempUp == 1) {
    tempUpFunc();
  }
}
BLYNK_WRITE(V7) // Logging on/off
{
  logstate = param.asInt();
  if (logstate == 1) {
    logMode = 1;
  }
  else {
    logMode = 0;
  }
}
BLYNK_WRITE(V8) // push on/off
{
  pushstate = param.asInt();
  if (pushstate == 1) {
    pushMode = 1;
  }
  else {
    pushMode = 0;
  }
}
BLYNK_WRITE(V19) // http on/off
{
  httpstate = param.asInt();
  if (httpstate == 1) {
    httpMode = 1;
  }
  else {
    httpMode = 0;
  }
}
BLYNK_WRITE(V21) // NOTAUS
{
  idlestate = param.asInt();
  if (idlestate == 1) {
    idleMode = 1;
  }
  else {
    idleMode = 0;
  }
}

//MeatDone set by Blynk
BLYNK_WRITE(V13)  // meatdone temp down
{
  meatDown = param.asInt();
  if (meatDown == 1) {
    meatDownFunc();
  }
}
BLYNK_WRITE(V14) // meatdone temp up
{
  meatUp = param.asInt();
  if (meatUp == 1) {
    meatUpFunc();
  }
}
void meatDownFunc()
{
  meatDone = meatDone - 1;
  Blynk.virtualWrite(V10, meatDone);
}
void meatUpFunc()
{
  meatDone = meatDone + 1;
  Blynk.virtualWrite(V10, meatDone);
}
// MeatDone2 set by Blynk
BLYNK_WRITE(V15)  // meatdone temp down
{
  meatDown2 = param.asInt();
  if (meatDown2 == 1) {
    meatDownFunc2();
  }
}
BLYNK_WRITE(V16) // meatdone temp up
{
  meatUp2 = param.asInt();
  if (meatUp2 == 1) {
    meatUpFunc2();
  }
}
void meatDownFunc2()
{
  meatDone2 = meatDone2 - 1;
  Blynk.virtualWrite(V11, meatDone2);
}
void meatUpFunc2()
{
  meatDone2 = meatDone2 + 1;
  Blynk.virtualWrite(V11, meatDone2);
}
//MeatDone 3 set by Blynk
BLYNK_WRITE(V17)  // meatdone temp down
{
  meatDown3 = param.asInt();
  if (meatDown3 == 1) {
    meatDownFunc3();
  }
}
BLYNK_WRITE(V18) // meatdone temp up
{
  meatUp3 = param.asInt();
  if (meatUp3 == 1) {
    meatUpFunc3();
  }
}
void meatDownFunc3()
{
  meatDone3 = meatDone3 - 1;
  Blynk.virtualWrite(V12, meatDone3);
}
void meatUpFunc3()
{
  meatDone3 = meatDone3 + 1;
  Blynk.virtualWrite(V12, meatDone3);
}
void tempDownFunc()
{
  setPoint = setPoint - 1;
  Blynk.virtualWrite(V6, setPoint);
}
void tempUpFunc()
{
  setPoint = setPoint + 1;
  Blynk.virtualWrite(V6, setPoint);
}
void sendData()
{
  if (WiFi.status() == WL_CONNECTED) {

    if (logMode != 1)
      return;
    Serial.println("try to send");
    if (client.connect(host, 80)) {
      // compose ThindSpeak update data url
      String url = "/update?key=";
      url = url + writeAPIKey;
      url = url + "&field1=";
      url = url + String(air);
      url = url + "&field2=";
      url = url + String(meat);
      url = url + "&field3=";
      url = url + String(meat2);
      url = url + "&field4=";
      url = url + String(meat3);
      url = url + "\r\n";
      // This will send the request to the ThingSpeak
      if (WiFi.status() == WL_CONNECTED) {
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
      }
    }
    client.stop();
  }
}
void sendPush()
{
  if (WiFi.status() == WL_CONNECTED) {
    if (pushMode != 1)
      return;
    Serial.println("try to push");
    if (air < lowTemp)
    {
      Blynk.notify("!!!Low Temp!!!");
    }
    if (air > highTemp)
    {
      Blynk.notify("!!!High Temp!!!");
    }
    if (meat > meatDone)
    {
      Blynk.notify("!!!Meat Done!!!");
    }
    if (meat2 > meatDone2)
    {
      Blynk.notify("!!!Meat2 Done!!!");
    }
    if (meat3 > meatDone3)
    {
      Blynk.notify("!!!Meat3 Done!!!");
    }
  }
}
void readSensors()
{
  digitalWrite(pA, LOW);
  digitalWrite(pB, LOW);
  digitalWrite(pC, LOW);
  rn = rn1;
  Acalc = A;
  Bcalc = B;
  Ccalc = C;
    Serial.println(A);
  calcTemp();

  if (f0 < 0)                                             // If value is not a number, assign an arbitrary value
  {
    air = 0;
  }
  else
  {
    air = f0;                                                    // Otherwise use the calculated value

  }
  Blynk.virtualWrite(V0, air);
  // request Meat 1:
  digitalWrite(pA, HIGH);
  digitalWrite(pB, LOW);
  digitalWrite(pC, LOW);
  rn = rn2;
  Acalc = A2;
  Bcalc = B2;
  Ccalc = C2;
  calcTemp();
  if (f0 < 0)                                                // If value is not a number, assign an arbitrary value
  {
    meat = 0;
  }
  else
  {
    meat = f0;                                                    // Otherwise use the calculated value

  }
  Blynk.virtualWrite(V1, meat);
  // request Meat 2:
  digitalWrite(pA, LOW);
  digitalWrite(pB, HIGH);
  digitalWrite(pC, LOW);
  rn = rn3;
  Acalc = A3;
  Bcalc = B3;
  Ccalc = C3;
  calcTemp();
  if (f0 < 0)                                                // If value is not a number, assign an arbitrary value
  {
    meat2 = 0;
  }
  else
  {
    meat2 = f0;                                                    // Otherwise use the calculated value

  }
  Blynk.virtualWrite(V2, meat2);
  // request Meat 3:
  digitalWrite(pA, HIGH);
  digitalWrite(pB, HIGH);
  digitalWrite(pC, LOW);
  rn = rn4;
  Acalc = A4;
  Bcalc = B4;
  Ccalc = C4;
  calcTemp();
  if (f0 < 0)                                              // If value is not a number, assign an arbitrary value
  {
    meat3 = 0;
  }
  else
  {
    meat3 = f0;                                                    // Otherwise use the calculated value

  }
  Blynk.virtualWrite(V3, meat3);


}
void calcTemp() {
  a0 = analogRead(0);   // This reads the "voltage" value on A0. Value is actually divided into 1024 steps from 0-1023.
  v0 = a0 * .00322;                                       // Converts A0 value to an actual voltage (3.3V / 1024 steps = .00322V/step.
  r0 = (((r2 * vin) / v0) - r2);                         // Calculates resistance value of thermistor based on fixed resistor value and measured voltage
  rref = r0 / (rn * 1000);
  logr0 = log(rref);
  k0 = 1.0 / (Acalc + (Bcalc * logr0) + (Ccalc * logr0 * logr0));        // Steinhart-Hart Equation to calculate temperature in Kelvin
  f0 = k0 - 273;                                        // Convert temperature to celsius
}
void updateDisplay()
{
  Blynk.virtualWrite(V6, setPoint);
  Blynk.virtualWrite(V7, logMode);
  Blynk.virtualWrite(V8, pushMode);
  Blynk.virtualWrite(V19, httpMode);
  Blynk.virtualWrite(V10, meatDone);
  Blynk.virtualWrite(V11, meatDone2);
  Blynk.virtualWrite(V12, meatDone3);
  switch (displaymode) {
    case 1:
      updateDisplaytemp();
      displaymode = 2;
      break;
    case 2:
      updateDisplayEvent();
      displaymode = 1;
      break;
  }
}
void pushHeat()
{
  Input = air;
  Input2 = air;
  myPID.Compute();
  myPID2.Compute();
  val2 = map(Output, 0, 255, 0, 300);
  val = map(Output2, 0, 255, 0, 150);     // scale it to use it with the servo (value between 0 and 180)
  if (val2 > 1) {
    val2 = val2 + 50;
  }
  if (val > 0) {
    //    val2 = val2 + 30;
  }
  if (air > highTemp)
  {
    val2 = 0;
    val = 0;
  }
  if (idleMode == 1)
  {
    val2 = 0;
    val = 0;
  }
  analogWrite(pushPin, val2);
  //  myservo.write(val);                  // sets the servo position according to the scaled value

  //going slow to save power litle lm7805
  if (val2 > valservo) {
    for (int i = valservo; i <= val2; i++) {
      myservo.write(i);
      delay(10);
      valservo = i;
    }
  }
  else {
    for (int i = valservo; i >= val2; i--) {
      myservo.write(i);
      valservo = i;
      delay(10);
    }
  }

  if (val2 > 0) {
    FAN.on();
  }
  else {
    FAN.off();
  }
  if (val > 0) {

    KLAPPE.on();
  }
  else {
    KLAPPE.off();
  }
}
void updateDisplaytemp()
{
  if (probecount <= 2)
  {
    #ifdef LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smoker:");
    #endif
    if (air < lowlim)
    {
      #ifdef LCD
      lcd.print(" N/A");
      #endif
    }
    else if (air > lowlim & air < 100)
    {
      #ifdef LCD
      lcd.print(" "); //pad with a space
      lcd.print(air);
      lcd.print(" C");
      #endif
    }
    else
    {
      #ifdef LCD
      lcd.print(air);
      lcd.print(" C");
      #endif
    }
    #ifdef LCD
    lcd.setCursor(0, 1);
    lcd.print("Meat:");
    #endif
    if (meat < lowlim)
    {
      #ifdef LCD
      lcd.print(" N/A");
      #endif
    }
    else if (meat > lowlim & meat < 100)
    {
      #ifdef LCD
      lcd.print(" "); //pad with a space
      lcd.print(meat);
      lcd.print(" C");
      #endif
    }
    else
    {
      #ifdef LCD
      lcd.print(meat);
      lcd.print(" C");
      #endif
    }
  }
  else {
    #ifdef LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("S:");
    #endif
    if (air < lowlim)
    {
      #ifdef LCD
      lcd.print(" N/A");
      #endif
    }
    else
    {
      #ifdef LCD
      lcd.print(air);
      #endif
    }
    #ifdef LCD
    lcd.setCursor(9, 0);
    lcd.print("1:");
    #endif 
    if (meat < lowlim)
    {
      #ifdef LCD
      lcd.print(" N/A");
      #endif
    }

    else
    {
          #ifdef LCD
      lcd.print(meat);
      #endif
    }
        #ifdef LCD
    lcd.setCursor(0, 1);
    lcd.print("2:");
    #endif
    if (meat2 < lowlim)
    {
          #ifdef LCD
      lcd.print(" N/A");
      #endif
    }
    else
    {
          #ifdef LCD
      lcd.print(meat2);
      #endif
    }
        #ifdef LCD
    lcd.setCursor(9, 1);
    lcd.print("3:");
    #endif
    if (meat3 < lowlim)
    {
          #ifdef LCD
      lcd.print(" N/A");
      #endif
    }

    else
    {
         #ifdef LCD
      lcd.print(meat3);
      #endif
    }
  }
}
void updateDisplayEvent()
{
  if (air < lowTemp)
  {
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.setCursor(2, 0);
    lcd.print("!!Low Temp!!");
    #endif
  }
  else  if (air > lowTemp & air < highTemp)
  {
    updateDisplaytemp();
  }
  else if (air > highTemp)
  {
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.setCursor(1, 0);
    lcd.print("!!High Temp!!");
    #endif
  }
  if (meat > meatDone)
  {
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.setCursor(2, 0);
    lcd.print("!Meat Finish!");
    #endif
  }
  if (meat2 > meatDone2)
  {
    #ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.setCursor(2, 0);
    lcd.print("!Meat2 Finish!");
    #endif
  }
  if (meat3 > meatDone3)
  {
#ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.setCursor(2, 0);
    lcd.print("!Meat3 Finish!");
    #endif
  }
}
void click1() {
  Serial.println("Button 1 click.");
  switch (menue) {
    case 1:
      setPoint --;
      updateDisplaymenue();
      break;
    case 2:
      meatDone --;
      updateDisplaymenue();
      break;
    case 3:
      meatDone2 --;
      updateDisplaymenue();
      break;
    case 4:
      meatDone3 --;
      updateDisplaymenue();
      break;
    case 5:
      probecount --;
      if (probecount < 1) probecount = 4;
      updateDisplaymenue();
      break;
  }
} // click1
void doubleclick1() {
  Serial.println("Button 1 doubleclick.");
  menue ++;
  if (menue > 8) menue = 0;
  updateDisplaymenue();
  // Serial.println(menue);
}
void doubleclick2() {
  Serial.println("Button 1 doubleclick.");
  menue --;
  if (menue < 1) menue = 8;
  updateDisplaymenue();
  // Serial.println(menue);
}
void longPress1() {
  Serial.println("Button 1 longPress...");
  switch (menue) {
    case 1:
      setPoint --;
      updateDisplaymenue();
      break;
    case 2:
      meatDone --;
      updateDisplaymenue();
      break;
    case 3:
      meatDone2 --;
      updateDisplaymenue();
      break;
    case 4:
      meatDone2 --;
      updateDisplaymenue();
      break;
    case 5:
      probecount --;
      if (probecount < 1) probecount = 4;
      updateDisplaymenue();
      break;
  }
} // longPress1
void click2() {
  Serial.println("Button 2 click.");
  switch (menue) {
    case 1:
      setPoint ++;
      updateDisplaymenue();
      break;
    case 2:
      meatDone ++;
      updateDisplaymenue();
      break;
    case 3:
      meatDone2 ++;
      updateDisplaymenue();
      break;
    case 4:
      meatDone3 ++;
      updateDisplaymenue();
      break;
    case 5:
      probecount ++;
      if (probecount > 4) probecount = 1;
      updateDisplaymenue();
      break;
    case 6:
      pushMode ++;
      if (pushMode > 2) pushMode = 1;
      updateDisplaymenue();
      break;
    case 7:
      logMode ++;
      if (logMode > 1) logMode = 0;
      updateDisplaymenue();
      break;
    case 8:
      httpMode ++;
      if (httpMode > 1) httpMode = 0;
      updateDisplaymenue();
      break;
  }
} // click2
void longPress2() {
  Serial.println("Button 2 longPress...");
  switch (menue) {
    case 0:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vin:");
      lcd.print(" "); //pad with a space
      lcd.print(volt);
      lcd.print(" V");
      lcd.setCursor(13, 0);
      #endif
      if (WiFi.status() == WL_CONNECTED) {
        #ifdef LCD
        lcd.print("On");
        #endif
        Serial.println("on");
      }
      else {
        #ifdef LCD
        lcd.print("Off");
        #endif
        Serial.println("off");
      }
      #ifdef LCD
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP());
      #endif
      delay(1000);
      break;
    case 1:
      setPoint ++;
      updateDisplaymenue();
      break;
    case 2:
      meatDone ++;
      updateDisplaymenue();
      break;
    case 3:
      meatDone2 ++;
      updateDisplaymenue();
      break;
    case 4:
      meatDone3 ++;
      updateDisplaymenue();
      break;
    case 5:
      probecount ++;
      if (probecount > 4) probecount = 1;
      updateDisplaymenue();
  }
} // longPress2
void updateDisplaymenue()
{
  switch (menue) {
    case 1:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sweat Temp:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      lcd.print(setPoint);
      lcd.print(" C");
#endif
      break;
    case 2:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Meat Done:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      lcd.print(meatDone);
      lcd.print(" C");
#endif
      break;
    case 3:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Meat Done 2:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      lcd.print(meatDone2);
      lcd.print(" C");
#endif
      break;
    case 4:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Meat Done 3:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      lcd.print(meatDone3);
      lcd.print(" C");
#endif
      break;
    case 5:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Meat count:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      lcd.print(probecount);
#endif
      break;

    case 6:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Push Nachrichten:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      if (pushMode == 1)
      {
        lcd.print("ON");
      }
      else
        lcd.print("OFF");
#endif
      break;
    case 7:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Loging:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      if (logMode == 1)
      {
        lcd.print("ON");
      }
      else
        lcd.print("OFF");
#endif
      break;
    case 8:
#ifdef LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HTTP Server:");
      lcd.print(" "); //pad with a space
      lcd.setCursor(0, 1);
      if (httpMode == 1)
      {
        lcd.print("ON");
      }
      else
        lcd.print("OFF");
#endif
      break;
  }
}
void updatehttp()
{
  if (WiFi.status() == WL_CONNECTED) {

    //  if (httpMode != 1)
    //    return;
    WiFiClient client = server.available();
    if (client)
    {
      Serial.println("new client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank)
          {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("Cook Time: ");
            client.println(float(float(millis() / 1000) / 60));
            client.println(" m");
            client.println("<br>");
            client.print("Smoker Temp: ");
            if (air > lowlim)
            {
              client.print(air);
            } else {
              client.print("NA "); //values below 20F are probably due to disconnected probe
            }
            client.println("<br>");
            client.print("Meat Temp: ");
            if (meat > lowlim)
            {
              client.print(meat);
            }
            else
            {
              client.print("NA "); //values below 20F are probably due to disconnected probe
            }
            client.println("<br>");
            client.println("</html>");

            client.print("Meat2 Temp: ");
            if (meat2 > lowlim)
            {
              client.print(meat2);
            }
            else
            {
              client.print("NA "); //values below 20F are probably due to disconnected probe
            }
            client.println("<br>");
            client.println("</html>");
            client.print("Meat3 Temp: ");
            if (meat3 > lowlim)
            {
              client.print(meat3);
            }
            else
            {
              client.print("NA "); //values below 20F are probably due to disconnected probe
            }
            client.println("<br>");
            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          }
          else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(1);

      // close the connection:
      client.stop();
      Serial.println("client disonnected");
    }
  }
}
void checkVolt()
{
  digitalWrite(pA, HIGH);
  digitalWrite(pB, LOW);
  digitalWrite(pC, HIGH);
  sensorValue = analogRead(0);
  volt2 = (sensorValue / 1023.0) * 18.8; // 3,3 V am Eingang bei 18,8V am Eingang Spannungsteiler R14700 R21000
  volt = volt2 + offset;
  // Serial.println("Spannung");
  //Serial.println(volt);
}
void wifiConfig() {
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    server.stop();
    Serial.println("Config mode Startetd");
#ifdef LCD
    lcd.clear();
    lcd.home();
    lcd.print("Config mode");
    lcd.setCursor(0, 1);
    lcd.print("connect to");
    delay(1000);
    lcd.clear();
    lcd.home();
    lcd.print("SSID: Grilloino");
    lcd.setCursor(0, 1);
    lcd.print("PWD: 12345678");
#endif
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //reset settings - for testing
    //wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);

    if (!wifiManager.startConfigPortal("Grilloino", "12345678")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    server.begin();
    Serial.println("connected...yeey :)");
    delay(1000);
    //Blynk neu verbinden
    while (Blynk.connect() == false) {
#ifdef LCD
      lcd.clear();
      lcd.home();
      lcd.print("Connected!");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP());
#endif
      delay(1000);
      Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
      Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
      while (Blynk.connect() == false) {
        // Wait until connected
        Serial.println("Try to connect to Blynk ");
#ifdef LCD
        lcd.clear();
        lcd.home();
        lcd.print("Try to connect");
        lcd.setCursor(0, 1);
        lcd.print("to Blynke");
#endif
      }
#ifdef LCD
      lcd.clear();
      lcd.home();
      lcd.print("connected");
      lcd.setCursor(0, 1);
      lcd.print("to Blynke");
      delay(1000);
#endif
    }
  }
}
