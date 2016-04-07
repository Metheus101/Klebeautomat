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

#define Speed_Schnell 50
#define Speed_Langsam 700

uint8_t myLEDS[LED_ANZ] = {BEREIT, FEHLER, MSCHNELL, MKLEBEN};
uint8_t myInput[TASTER_ANZ] = {STOP, START, HUB, DRUCK, MODUS, SPEED};


// Global Vars
bool StateModus = false;
bool StateSpeed = false;

void StopFun();
void HubFun();
void DruckFun();
void StartFun();
void SchnellFun();
void KlebenFun();

//void myRamp(int SollSpeed);

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
  StateSpeed = digitalRead(5);
  
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

  else if ( ! digitalRead(START)) StartFun();

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
  Serial.println("HUBFun!!!!");
  digitalWrite(VENTIL_HUB, HIGH);
  digitalWrite(MKLEBEN, HIGH);
  digitalWrite(MSCHNELL, HIGH);
  digitalWrite(BEREIT, LOW);

  while (! digitalRead(HUB)) {}; // warten bis taster == 1
  delay(100);
  while (digitalRead(HUB)) {};

  digitalWrite(VENTIL_HUB, LOW);
  digitalWrite(MKLEBEN, LOW);
  digitalWrite(MSCHNELL, LOW);
  digitalWrite(BEREIT, LOW);

  while (! digitalRead(HUB)) {}; // warten bis taster == 1

  Serial.println("exit HUB");
}
void DruckFun() {
  Serial.println("DRUCKFun!!!!");
  digitalWrite(VENTIL_DRUCK, HIGH);
  digitalWrite(BEREIT, LOW);
  digitalWrite(MSCHNELL, LOW);
  digitalWrite(MKLEBEN, LOW);

  while ( ! digitalRead(DRUCK)) {}; // warten bis taster == 1
  delay(100);

  digitalWrite(VENTIL_DRUCK, LOW);
  Serial.println("exit Druck");
}

void StartFun() {
  Serial.println("StartFun!!!");
  // Motor init
  digitalWrite(MS3, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS1, HIGH);
  digitalWrite(RESET, HIGH);
  digitalWrite(ENABLE, LOW);

  while (! digitalRead(START)) {};  // warten bis taster == 1

  if (StateModus) SchnellFun();     // ist schnell!!
  else KlebenFun();                 // ist kleben!!!

  Serial.println("exit Start!!");
}

void SchnellFun() {
  int MotorSpeed;
  Serial.println("SchnellFUN!!!");
  Serial.print("StateSpeed: ");
  Serial.println(StateSpeed);  
  if (StateSpeed) MotorSpeed = Speed_Schnell;
  else {
    MotorSpeed = Speed_Langsam;
  }

  // RAMPE
  for (int i = 0; i < MotorSpeed; i++)
  {
    //Serial.println("in der for schleife");
    digitalWrite(STEP, HIGH);
    delayMicroseconds(myRamp[i]);
    digitalWrite(STEP, LOW);
    delayMicroseconds(myRamp[i]);
    
    // tasten event
    if ( ! digitalRead(START))
    {
      Serial.println("tasten event alpha");
      digitalWrite(RESET, LOW);
      digitalWrite(ENABLE, HIGH);
      while (! digitalRead(START)) {};  // warten bis taster == 1
      return;
    }
    if ( ! digitalRead(STOP))
    {
      Serial.println("tasten event beta");
      StopFun();
      return;
    }
  }
  //Serial.println("Motor läuft in der whielloop");
  while(true){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(MotorSpeed);
    digitalWrite(STEP, LOW);
    delayMicroseconds(MotorSpeed);
    
    // tasten event
    if ( ! digitalRead(START))
    {
      Serial.println("tasten event gamma");
      digitalWrite(RESET, LOW);
      digitalWrite(ENABLE, HIGH);
      while (! digitalRead(START)) {};  // warten bis taster == 1
      return;
    }
    if ( ! digitalRead(STOP))
    {
      Serial.println("tasten event delta");
      StopFun();
      return;
    }
    
  }
  Serial.println("exit Schnell");
}
void KlebenFun() {
  Serial.println("KlebenFun!!!");

  Serial.println("exit Kleben");
}

