//Header File
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <LCD_I2C.h>
#include <afstandssensor.h>

#define L_R 9  // Door Close
#define L_B 10  // Door Close Waiting for reset
#define L_G 11  // Door Open

#define SPEAKER 13
#define DOOR 12



//Pin Define
SoftwareSerial fingerprintSerial(4, 5);  //TXD[FingerPrint] => RX[Arduino = D4] & RXD[FingerPrint] => TX[Arduino = D5]

AfstandsSensor afstandssensor(6, 7);     // Trig[U Sensor] => Arduino = D6 & Echo[U Sensor] => Arduino = D7
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
int smsCheckCountDown = 120;
String CellNumtemp;
String CellNum;
String textMessage;
String textMessagetemp;


int doorOpenUltra = 0;
uint8_t p ;


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);


  delay(100);
  //  simSerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS

  fingerprintSerial.begin(9600);

  pinMode(L_R, OUTPUT);
  pinMode(L_B, OUTPUT);
  pinMode(L_G, OUTPUT);

  pinMode(SPEAKER, OUTPUT);
  pinMode(DOOR, OUTPUT);

  LED_Off();
  LED_G();
  digitalWrite(SPEAKER, LOW);   // Buzzer off
  digitalWrite(DOOR, LOW);      // Door Lock


  lcd.begin();




  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    //    displayView("");
    Serial.println("Found fingerprint sensor!");
  } else {
    //    displayView("");
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  lcd.backlight();

  randomSeed(analogRead(0));

  delay(1000);
  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
  FCheck();   // get fingerprint id from sensor
  delay(500);            //don't ned to run this at full speed
}


//Fingerprint Control

bool FCheck() {
  lcd.setCursor(0, 0);
  lcd.print("No finger           ");
  lcd.setCursor(0, 1);
  lcd.print("detected!              ");
  Serial.println("No finger detected1!");


  p = finger.getImage();
  if (p != FINGERPRINT_OK)  return false;
  Serial.println("No finger detected2!");

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return false;
  Serial.println("No finger detected3!");

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return false;
  Serial.println("No finger detected4!");


  thisFingerId = finger.fingerID;


  start_SIM_OTP_Process();


}




void start_SIM_OTP_Process() {
  long randNumberTemp  = random(1011, 10000);
  Serial.println(random(1011, 10000));
  Serial.println(randNumberTemp);

  lcd.setCursor(0, 0);
  lcd.print("Welcome " + myDataList[thisFingerId][0] + "            ");
  Serial.println("Welcome " + myDataList[thisFingerId][0]);

  doorOpen();



}

void doorOpen() {
  digitalWrite(SPEAKER, HIGH);   // Buzzer on
  digitalWrite(DOOR, HIGH);
  lcd.setCursor(0, 1);
  lcd.print("Open the door       ");
  LED_Off();

  LED_R();
  // Door open
  delay(10000);
  digitalWrite(DOOR, LOW);
  lcd.setCursor(0, 1);
  lcd.print("Close the door             ");
doorOpenUltra = 0;
  while (doorOpenUltra < 15) {
    if (afstandssensor.afstandCM() < 10) {
      doorOpenUltra = doorOpenUltra + 1;
      delay(1000);
      LED_Off();
      digitalWrite(SPEAKER, LOW);

      delay(1000);
      LED_B() ;
      LED_G();
      digitalWrite(SPEAKER, HIGH);      // Door open

    } else {
      doorOpenUltra = 0;
      delay(1000);
      LED_Off();
      delay(1000);
      LED_B() ;

    }
  }

  digitalWrite(SPEAKER, LOW);      // Door open
  LED_Off();
  LED_G();
doorOpenUltra = 0;

}

//LED Control
void LED_R() {
  digitalWrite(L_R, LOW);
}
void LED_B() {
  digitalWrite(L_B, LOW);
}
void LED_G() {
  digitalWrite(L_G, LOW);
}



void LED_Off() {
  digitalWrite(L_R, HIGH);
  digitalWrite(L_G, HIGH);
  digitalWrite(L_B, HIGH);
}



void displayDelete() {
  lcd.noBacklight();
  lcd.clear();
  delay(500);
}
