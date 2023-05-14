//Header File
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <LCD_I2C.h>

#define LED_R1 6
#define LED_B1 7
#define LED_G1 8
#define LED_R2 9
#define LED_B2 10
#define LED_G2 11

#define SPEAKER 12
#define DOOR 13



//Pin Define
SoftwareSerial simSerial(2, 3);  //TXD[SIM] => RX[Arduino = 2] & RXD[SIM] => TX[Arduino = 3]
SoftwareSerial fingerprintSerial(4, 5);  //TXD[FingerPrint] => RX[Arduino = 4] & RXD[FingerPrint] => TX[Arduino = 5]

LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according and PIN to
//SDA[LCD] => DATA[Arduino = 12C DATA] & SCL[LCD] => CLOCK[Arduino = 12C CLOCK]

//OPEN UNIQUE_ID OTP
//OPEN R 1234
const char* array[9][3] = {
  {"Name", "unique id", "Phone Number"},
  {"Sayantan", "L", "+919614975333"}, 
  {"Sayantan", "R", "+919614975333"}, 
  {"Akanksha", "L", "+917439450525"}, 
  {"Akanksha", "R", "+917439450525"}, 
  {"Adway", "L", "+919875360868"},    
  {"Adway", "R", "+919875360868"},     
  {"Debanjan", "L", "+918336075255"},  
  {"Debanjan", "R", "+918336075255"}  
};


int thisFingerId = -1;

char thisFingerIdChar = "-1";
char thisFingerConfidenceChar = "0";

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);


  simSerial.begin(57600);
  fingerprintSerial.begin(57600);

  pinMode(LED_R1, OUTPUT);
  pinMode(LED_B1, OUTPUT);
  pinMode(LED_G1, OUTPUT);
  pinMode(LED_R2, OUTPUT);
  pinMode(LED_B2, OUTPUT);
  pinMode(LED_G2, OUTPUT);

  pinMode(SPEAKER, OUTPUT);
  pinMode(DOOR, OUTPUT);



  LED_1_Off();
  LED_2_Off();
  digitalWrite(SPEAKER, LOW);   // Buzzer off
  digitalWrite(DOOR, LOW);      // Door Lock

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
  getFingerprintID();   // get fingerprint id from sensor
}




//Fingerprint Control

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

 thisFingerIdChar = finger.fingerID;
 thisFingerConfidenceChar = finger.confidence;

 thisFingerId = thisFingerIdChar-'0';
 thisFingerConfidence = thisFingerConfidenceChar-'0';

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}



//LED Control
void LED_R1_On() {
  digitalWrite(LED_R1, LOW);
}
void LED_B1_On() {
  digitalWrite(LED_B1, LOW);
}
void LED_G1_On() {
  digitalWrite(LED_G1, LOW);
}


void LED_R2_On() {
  digitalWrite(LED_R2, LOW);
}
void LED_B2_On() {
  digitalWrite(LED_B2, LOW);
}
void LED_G2_On() {
  digitalWrite(LED_G2, LOW);
}

void LED_1_Off() {
  digitalWrite(LED_R1, HIGH);
  digitalWrite(LED_G1, HIGH);
  digitalWrite(LED_B1, HIGH);
}

void LED_2_Off() {
  digitalWrite(LED_R2, HIGH);
  digitalWrite(LED_G2, HIGH);
  digitalWrite(LED_B2, HIGH);
}
