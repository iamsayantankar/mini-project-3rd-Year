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
//char OTP = "1234";
const String myDataList[9][3] = {
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
int randNumber = 0000;
bool smsCheck = false;
int smsCheckCountDown = 120;
String CellNumtemp;
String CellNum;
String textMessage;
String textMessagetemp;


String lcd_myString_1 = "";
String lcd_myString_2 = "";
String lcdString = "";
int lcdStringLenTemp = 0;




Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

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


  lcd.begin();
  lcd.noBacklight();

  simSerial.begin(9600);   // Setting the baud rate of GSM Module
  delay(100);
  simSerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS





  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    displayView("");
    Serial.println("Found fingerprint sensor!");
  } else {
    displayView("");
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getParameters();

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    displayView("");
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    displayView("");
    Serial.println("Waiting for valid finger...");
  }





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
      displayView("");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      displayView("");
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      displayView("");
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      displayView("");
      Serial.println("Imaging error");
      return p;
    default:
      displayView("");
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      displayView("");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      displayView("");
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      displayView("");
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      displayView("");
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      displayView("");
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      displayView("");
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    displayView("");
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    displayView("");
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    displayView("");
    Serial.println("Did not find a match");
    return p;
  } else {
    displayView("");
    Serial.println("Unknown error");
    return p;
  }

  // found a match!

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  thisFingerId = finger.fingerID;


  start_SIM_OTP_Process();


  return finger.fingerID;
}




void start_SIM_OTP_Process() {
  lcdString = "Welcome " + myDataList[thisFingerId][0];
  lcdStringLenTemp = lcdString.length();
  if (lcdStringLenTemp < 16) {
    lcdString = lcdString + " ";
    lcdStringLenTemp = lcdStringLenTemp + 1;
  }


  displayView("");
  randNumber = random(1011, 9999);
  smsCheckCountDown = 120;
  lcdString = lcdString + "OTP: " + String(randNumber);

  smsCheck = true;
  while (smsCheck == true) {
    if (simSerial.available() > 0) {

      textMessage = simSerial.readString();
      delay(10);

      //OPEN UNIQUE_ID OTP
      //OPEN R 1234
      // check if the SMS is "OPEN"
      if (textMessage.indexOf("OPEN") >= 0) {
        displayView("");
        //save the phone number of the senders in a string (this works with italian region you must adapt to   yours)
        CellNumtemp = textMessage.substring(textMessage.indexOf("+91"));
        CellNum = CellNumtemp.substring(0, 13);

        if (CellNum == myDataList[thisFingerId][2]) {
          textMessagetemp = "OPEN " + myDataList[thisFingerId][1] + " " + String(randNumber);
          if (textMessage.indexOf(textMessagetemp) >= 0) {
            doorOpen();
          }
        } else {

        }
        //      smsstato();
        CellNumtemp = "";
        CellNum = "";
        textMessage = "";
        textMessagetemp = "";

        thisFingerId = -1;
        randNumber = 0000;
        smsCheck = false;
        smsCheckCountDown = 120;
      }

    }
  }





}

void doorOpen() {

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



void displayView(String myString) {
  lcd.noBacklight();
  lcd.clear();
  delay(100);
  if (myString.length() > 16) {
    lcd_myString_1 = myString.substring(0, 16);
    lcd_myString_2 = myString.substring(16, 32);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print(lcd_myString_1);
    lcd.setCursor(0, 1);
    lcd.print(lcd_myString_2);
    delay(500);
  }
  else {
    lcd_myString_1 = myString.substring(0, 16);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print(lcd_myString_1);
    lcd.setCursor(0, 1);
    lcd.print("");
    delay(500);
  }
}

void displayDelete() {
  lcd.noBacklight();
  lcd.clear();
  delay(500);
}
