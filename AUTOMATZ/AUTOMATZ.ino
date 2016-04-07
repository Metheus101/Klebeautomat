/* Markus Automat Kleben 2016-02-28
*
*
*/

// LEDs
#define BEREIT   8
#define FEHLER   9
#define MSCHNELL 10
#define MKLEBEN  12
#define LED_ANZ  4

// Input Taster & Schalter
#define STOP     2
#define START    3
#define HUB      6
#define DRUCK    7
#define MODUS    4
#define SPEED    5
#define TASTER_ANZ 6

// Pneumatik
#define VENTIL_HUB 0  // HUB
#define VENTIL_DRUCK 1  // Druck

// Motor
#define STEP   11
#define DIR    13
#define SLEEP  A0    // muss high sein
#define RESET  A1    // muss high sein
#define MS3    A2
#define MS2    A3
#define MS1    A4
#define ENABLE A5

uint8_t myLEDS[LED_ANZ] = {BEREIT, FEHLER, MSCHNELL, MKLEBEN};
uint8_t myInput[TASTER_ANZ] = {STOP, START, HUB, DRUCK, SPEED, MODUS};


// Global Vars
bool StateModus = false;
bool StateSpeed = false;

void StopFun();
void HubFun();
void DruckFun();
void SchnellFun();
void KlebenFun();

void setup() {
  // LEDs init
  for (uint8_t i = 0; i < LED_ANZ; i++) pinMode(myLEDS[i], OUTPUT);
  for (uint8_t i = 0; i < LED_ANZ; i++)  digitalWrite(myLEDS[i], LOW);

  // Taster init
  for (int i = 0; i < TASTER_ANZ; i++) pinMode(myInput[i], INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);  // PullUp sollte schon in der Schleife gesetzt sein... wird es aber nicht bug??

  // Pneumatik init
  pinMode(VENTIL_HUB, OUTPUT); digitalWrite(VENTIL_HUB, LOW);
  pinMode(VENTIL_DRUCK, OUTPUT); digitalWrite(VENTIL_DRUCK, LOW);

  // Motortreiber init & AUS
  pinMode(ENABLE, OUTPUT); digitalWrite(ENABLE, HIGH);
  pinMode(STEP,   OUTPUT); digitalWrite(STEP,  LOW);
  pinMode(SLEEP,  OUTPUT); digitalWrite(SLEEP, HIGH);
  pinMode(RESET,  OUTPUT); digitalWrite(RESET, LOW);
  pinMode(DIR,    OUTPUT);
  pinMode(MS3,    OUTPUT);
  pinMode(MS2,    OUTPUT);
  pinMode(MS1,    OUTPUT);


  // Serial Interface
  Serial.begin(9600);
  Serial.println("Init Automat");
  for (uint8_t i = 0; i < LED_ANZ; i++)  digitalWrite(myLEDS[i], HIGH);
  delay(1000);
  for (uint8_t i = 0; i < LED_ANZ; i++)  digitalWrite(myLEDS[i], LOW);


  // Hub schalten - hochfahren
  digitalWrite(VENTIL_HUB, HIGH);


}

void loop() {
  // Schalter einlesen
  StateModus = digitalRead(MODUS);
  if (StateModus) {
    digitalWrite(MSCHNELL, HIGH);
    digitalWrite(MKLEBEN, LOW);
  }
  else {
    digitalWrite(MKLEBEN, HIGH);
    digitalWrite(MSCHNELL, LOW);
  }
  digitalWrite(BEREIT, HIGH);  // ist bereit

  // Frage user input ab:
  if ( ! digitalRead(STOP)) StopFun(); // höchste prio wir wolle unsere finger behalten

  else if ( ! digitalRead(DRUCK)) DruckFun();

  else if ( ! digitalRead(HUB)) HubFun();

  else if ( ! digitalRead(START)) {
    if ( StateModus) SchnellFun(); // ist schnell!!
    else KlebenFun();              // ist kleben!
  }
  else delay(100);
}


void StopFun() {
  Serial.println("StopFun!!!!");

  // LED states
  digitalWrite(BEREIT, LOW);
  digitalWrite(MKLEBEN, LOW);
  digitalWrite(MSCHNELL, LOW);
  digitalWrite(FEHLER, HIGH);

  // Motor vollbremsen
  digitalWrite(MS3, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(RESET, HIGH);
  digitalWrite(ENABLE, LOW);

  while (! digitalRead(STOP)) {}; // warten bis taster == 1
  delay(100);
  while (digitalRead(STOP)) {};

  digitalWrite(RESET, LOW);
  digitalWrite(ENABLE, HIGH);
  digitalWrite(FEHLER, LOW);

  while (! digitalRead(STOP)) {}; // warten bis taster == 1
  Serial.println("exit STOP");
  delay(100);  
}

void HubFun() {
  Serial.println("StopFun!!!!");


}
void DruckFun() {
  Serial.println("StopFun!!!!");
  digitalWrite(VENTIL_DRUCK, HIGH);
  digitalWrite(BEREIT, LOW);
  digitalWrite(MSCHNELL, LOW);
  digitalWrite(MKLEBEN, LOW);
  
  while( ! digitalRead(DRUCK)) {}; // warten bis taster == 1
  delay(100);
  
  digitalWrite(VENTIL_DRUCK, LOW);
  Serial.println("exit Druck");
}

void SchnellFun() {
  Serial.println("StopFun!!!!");


}
void KlebenFun() {
  Serial.println("StopFun!!!!");


}


