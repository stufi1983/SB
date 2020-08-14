//Kalau ada P10 yang tampilan acak, ganti ke MAPPING1

#define MAPPING0
#define buzzerBesar A8
//DMD
#include <SPI.h>
#include <DMD2.h>
//#include <fonts/Droid_Sans_24.h>
#include "ElconFont.h"
SoftDMD dmd(1, 4); // DMD controls the entire display

//simpan ststus
#include <EEPROM.h>

//7-Segment
byte JamMenitTgl[8];//yp=0,ys=0,yr=2,ysr=0,mp=0,ms=0,dp=0,ds=0,sp=0,ss=0,mp=0,ms=0,hp=0,hs=0
#include "IC4094Setting.h"


//RTC  DS3231 //A4 - SDA //A5 - SCL
#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68
#include "RTCSetting.h"
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
bool changeDay = false; byte lastDay = 0; byte lastMonth = 0;
volatile boolean timeTick = false;  //also as flag every 1 second

//P14
#include <avr/pgmspace.h>
#include <EEPROM.h>

//Buzzer
#define pinBuzzer 12
boolean buzzerStatus = false;

//#define GRAPHICS_ON true
DMDGraphicsMode DMDMode;




enum mode {STDBY, SET, PLAY, ALARM, PAUSED, SETTIME};
byte mode = STDBY;
unsigned int detikPlay = 0, detikAlarm = 0;
byte alarmduration = 3;
byte iteration = 0;
byte jamT = 0, menitT = 0, detikT = 0;

byte ScoreA = 0, ScoreB = 0, Round = 0;
const int COUNTDOWN_FROM = 2;
byte counter = COUNTDOWN_FROM;

//Timer2 Overflow Interrupt Vector, called every 0.008 s
unsigned int count = 0;   //used to keep count of how many interrupts were fired
ISR(TIMER2_OVF_vect) {
  count++;               //Increments the interrupt counter
  if (count > 124) {
    timeTick = true;     //timeTick
    count = 0;           //Resets the interrupt counter
  }
  TCNT2 = 130;           //Reset Timer to 130 out of 255
  TIFR2 = 0x00;          //Timer2 INT Flag Reg: Clear Timer Overflow Flag
};

//rf


//eeprom
byte bounchCount = 5;
bool scoreChange = false;
bool longpress = false;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(buzzerBesar, OUTPUT);
  digitalWrite(buzzerBesar, LOW);
  Serial.begin(9600);
  dmd.setBrightness(0);  //off DMD
  dmd.selectFont(ElconFont);
  dmd.begin();
  Serial.println("DMD OK");

  enable4094(); //Start 4094
  Serial.println("7Segment OK");

  Wire.begin(); //RTC
  //uncomment to set RTC time
  //setDS3231time( 00,  38,  15,  02,  19,  04,  16); while (1);
  //Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  //Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  changeDay = true;
  Serial.print("RTC: "); Serial.print(hour); Serial.print(":"); Serial.print(minute); Serial.print(":"); Serial.println(second);
  updateJamSetting();

  //buzzer
  pinMode(pinBuzzer, OUTPUT);

  //timer2
  pinMode(13, OUTPUT);

  //Setup Timer2 to fire every 1ms
  TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 125;         //Reset Timer Count to 125 out of 255
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x07;        //Timer2 Control Reg B: Timer Prescaler set to 1024
  //f Timer = Fcpu/Prescale = 16000000/1024 = 15625 Hz T=~0.000064 s
  //Timer Tick = T*TCNT = 0.000064 * 125 = 0.008 s
  //Tdesire = 1 s
  //count = Tdesire / Ttick =  1 / 0.008 = 125 times

  //rf
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);

  //  pinMode(A9, INPUT_PULLUP);
  //  pinMode(A10, INPUT_PULLUP);
  //  pinMode(A11, INPUT_PULLUP);
  //  pinMode(A12, INPUT_PULLUP);

  //dmd.setBrightness(255);  //on DMD
  scoreChange = true;
  /*
    mode = EEPROM.read(5);
    if (mode > 5) {
      mode = STDBY;
      //EEPROM.write(5, mode);
    }
    if (mode == PLAY) {
      detikPlay = EEPROM.read(6);
      detikAlarm = EEPROM.read(7);
    }
    Serial.print(mode, DEC);
  */
}


void loop() {//return;

  if (digitalRead(A9)) { //A
    if (mode == STDBY) {
      ScoreA = 0; ScoreB = 0; Round = 1;

       dmd.drawString(16, 32, String(ScoreB % 10));
       dmd.drawString(0, 32, String(ScoreB / 10));
       dmd.drawString(0, 0, String(ScoreA / 10));
       dmd.drawString(16, 0, String(ScoreA % 10));


#ifdef MAPPING1
      String ch;
      ch = inttochar(ScoreB % 10);
      dmd.drawString(16, 32, ch);
      ch = inttochar(ScoreB / 10);
      dmd.drawString(0, 32, ch);
      dmd.drawString(0, 0, String(ScoreA / 10));
      dmd.drawString(16, 0, String(ScoreA % 10));
      clearLine(ScoreB % 10);

#endif


      jamT = EEPROM.read(0);
      if (jamT > 24) jamT = 00;
      menitT = EEPROM.read(1);
      if (menitT > 60) menitT = 00;
      detikT = 0;

      updateTimerSetting();
      mode = SET; deBounching3dt(); ////EEPROM.write(5, mode);

    } else if (mode == SET ) {
      menitT++; if (menitT > 59) menitT = 00;
      EEPROM.write(1, menitT);
      updateTimerSetting();
    } else if (mode == SETTIME ) {
      menitT++; if (menitT > 59) menitT = 00;
      EEPROM.write(1, menitT);
      updateTimerSetting();

    } else if (mode == PLAY || mode == ALARM || mode == PAUSED) {
      while (digitalRead(A9)) {
        iteration++;
        delay(50);
        if (iteration > 30) {
          deBounching3dt();
          longpress = true;
          break;
        }
      }
      while (digitalRead(A9)) {
        delay(20);
        Serial.println(iteration);
      }
      iteration = 0;

      if (longpress) {
        longpress = false;
        mode = STDBY; //EEPROM.write(5, mode);
        Round = 0;
        scoreChange = true;
        
      } else {
        if (mode == PLAY) {
          ScoreB++; scoreChange = true;
        }
      }

    }
    deBounching();
  }

  if (digitalRead(A10)) { //B
    if (mode == SET) {
      while (digitalRead(A10)) {
        iteration++;
        delay(50);
        if (iteration > 30) {
          deBounching3dt();
          longpress = true;

          STARTDIGITTGL
          displayRound(Round);
          displayJamMenitTgl(0, 0, 0);
          ENDDIGITTGL

          break;
        }
      }
      while (digitalRead(A10)) {
        delay(20);
        Serial.println(iteration);
      }
      iteration = 0;

      if (longpress) {
        longpress = false;
        detikAlarm = EEPROM.read(0) * 3600 + EEPROM.read(1) * 60;
        if (detikAlarm < 60) detikAlarm = 60;
        detikPlay = 0;
        mode = PLAY; //EEPROM.write(5, mode);
      } else {
        if (menitT > 0) menitT--; else menitT = 59;
        EEPROM.write(1, menitT);
        updateTimerSetting();
      }

    } else if (mode == PAUSED) {
      while (digitalRead(A10)) {
        iteration++;
        delay(50);
        if (iteration > 30) {
          deBounching3dt();
          longpress = true;
          Round++;
          Serial.print("Round:");
          Serial.println(Round);
          STARTDIGITTGL
          displayRound(Round);
          displayJamMenitTgl(0, 0, 0);
          ENDDIGITTGL

          break;
        }
      }
      while (digitalRead(A10)) {
        delay(20);
        Serial.println(iteration);
      }

      iteration = 0;

      if (longpress) {
        longpress = false;
        mode = PLAY; //EEPROM.write(5, mode);
      } else {
        if (ScoreB > 0) ScoreB--; scoreChange = true;
      }


    } else if (mode == PLAY) {
      if (ScoreB > 0) ScoreB--; scoreChange = true;
    }
    else if (mode == SETTIME ) {
      if (menitT > 0) menitT--; else menitT = 59;
      EEPROM.write(1, menitT);
      updateTimerSetting();
    }
    deBounching();
  }

  if (digitalRead(A11)) { //C
    if (mode == SET) {
      jamT++; if (jamT > 59) jamT = 00;
      EEPROM.write(0, jamT);
      updateTimerSetting();

    }  else  if (mode == SETTIME) {
      jamT++; if (jamT > 23) jamT = 00;
      EEPROM.write(0, jamT);
      updateTimerSetting();

    } else if (mode == PLAY || mode == STDBY) {
      while (digitalRead(A11)) {
        iteration++;
        delay(50);
        if (iteration > 30) {
          deBounching3dt();
          longpress = true;
          break;
        }
      }
      while (digitalRead(A11)) {
        delay(20);
        Serial.println(iteration);
      }
      iteration = 0;

      if (longpress && mode == PLAY) {                //PLAY: Round up
        longpress = false;
        Round++;
      } else if (longpress && mode == STDBY) {        //STDBY --> SETTIME (Press C 3 second at standby)
        longpress = false;
        mode = SETTIME; //EEPROM.write(5, mode);
        jamT = hour; menitT = minute; detikT = second;
      } else if (longpress && mode == SETTIME) {        //SETTIME --> STDBY, cancel set time (Press C 3 second at set time)
        longpress = false;
        mode = STDBY; //EEPROM.write(5, mode);
      } else {
        ScoreA++; scoreChange = true;
      }
    }
    deBounching();
  }

  if (digitalRead(A12)) {//D
    if (mode == SET) {
      if (jamT > 0) jamT--; else jamT = 23;
      EEPROM.write(0, jamT);
      updateTimerSetting();
    } else if (mode == PLAY || mode == SETTIME) {
      while (digitalRead(A12)) {
        iteration++;
        delay(50);
        if (iteration > 30) {
          deBounching3dt();
          longpress = true;
          break;
        }
      }
      while (digitalRead(A12)) {
        delay(20);
        Serial.println(iteration);
      }
      iteration = 0;

      if (longpress && mode == PLAY) {                //PLAY: Round down
        longpress = false;
        Round--;
      } else if (longpress && mode == SETTIME) {
        longpress = false;
        setDS3231time(  detikT, menitT, jamT, dayOfWeek, dayOfMonth, month, year);
        mode = STDBY; //EEPROM.write(5, mode);
        Round = 0x0A;
      }
      else if (mode == PLAY) {
        if (ScoreA > 0)ScoreA--; scoreChange = true;
      } else  if (mode == SETTIME ) {

        if (jamT > 0) jamT--; else jamT = 23;
        EEPROM.write(0, jamT);
        Serial.println(jamT); updateTimerSetting();
      }
    }
    else if (longpress && mode == SETTIME) {        //SETTIME --> STDBY, cancel set time (Press C 3 second at set time)
    }
    deBounching();
  }




  if (timeTick) {
    timeTick = false;
    if (mode == PAUSED) {
      Serial.println("Paused....");
      digitalWrite(pinBuzzer, LOW);

    }

    if (mode == SET) {
      Serial.println("Set....");
    }

    if (mode == STDBY) {
      Serial.println("Standby....");
      updateJamSetting();


    }
    if (mode == ALARM) {
      Serial.println("Alarm....");
      iteration++;
      if (alarmduration == iteration) {
        iteration = 0;
        detikPlay = 0;
        updateTimerSetting();
        mode = PAUSED; //EEPROM.write(5, mode);
        digitalWrite(buzzerBesar, LOW);
      }
    }

    if (mode == PLAY) {
      if (detikPlay <= detikAlarm) {
        detikPlay++;
        unsigned int detik2jam = detikPlay;
        byte jamLocal = 0, menitLocal = 0, detikLocal = 0;
        while (detik2jam > 3600) {
          detik2jam = detik2jam - 3600;
          jamLocal++;
        }
        while (detik2jam > 60) {
          detik2jam = detik2jam - 60;
          menitLocal++;
        }
        detikLocal = detik2jam - 1;

        STARTDIGITTGL            //jam menit detik local variable
        displayRound(Round);
        displayJamMenitTgl(jamLocal, menitLocal, detikLocal);
        ENDDIGITTGL

        EEPROM.write(6, detikPlay);
        EEPROM.write(7, detikAlarm);

        Serial.print(Round); Serial.print(";"); Serial.print(jamLocal); Serial.print(":"); Serial.print(menitLocal); Serial.print(":"); Serial.println(detikLocal);
      } else {
        Serial.println("TIME UP");
        mode = ALARM; //EEPROM.write(5, mode);
        digitalWrite(pinBuzzer, HIGH);
        digitalWrite(buzzerBesar, HIGH);
        iteration = 0;
      }
    }
    //if (mode == PLAY || mode == ALARM || mode == PAUSED ) {
    //}

    if (mode == SETTIME) {
      digitalWrite(pinBuzzer, HIGH);
      delay(50);
      digitalWrite(pinBuzzer, LOW);
    }
  }

  if (mode == SETTIME) {
    //digitalWrite(pinBuzzer, HIGH);
    Serial.println("Setting RTC....");
    Round = 0x0a;
    //updateJamSetting();
    //EEPROM.write(2,hour);
    //EEPROM.write(3,minute);
    //EEPROM.write(4,second);
    //jamT = hour; menitT = minute; detikT = second;
    updateTimerSetting();
  }


  if (scoreChange) {
    scoreChange = false;
    if (mode == STDBY) {
      dmd.clearScreen();

      //      dmd.drawString(16, 32, String(":"));
      //      dmd.drawString(0, 32, String(":"));
      //      dmd.drawString(0, 0, String(":"));
      //      dmd.drawString(16, 0, String(":"));

      dmd.clearScreen();
      Round = 0x0A;
#ifdef MAPPING1
      dmd.drawString(16, 32, String("D"));
      dmd.drawString(0, 32, String("D"));
#endif
      //clearLine(1);
      dmd.setBrightness(255);  //on DMD

    } else {

#ifdef MAPPING1
      String ch;
      ch = inttochar(ScoreB % 10);
      dmd.drawString(16, 32, ch);
      ch = inttochar(ScoreB / 10);
      dmd.drawString(0, 32, ch);
      clearLine(ScoreB % 10);
#endif
      dmd.setBrightness(0);  //on DMD
      dmd.drawString(16, 32, String(ScoreB % 10));
      dmd.drawString(0, 32, String(ScoreB / 10));

      dmd.drawString(0, 0, String(ScoreA / 10));
      dmd.drawString(16, 0, String(ScoreA % 10));


      dmd.setBrightness(255);  //on DMD
    }
  }

}
String inttochar(byte num) { //: ; < = > ? @ A B C D
  if (num == 0) return String(":");
  else if (num == 1) return String(";");
  else if (num == 2) return String("<");
  else if (num == 3) return String("=");
  else if (num == 4) return String(">");
  else if (num == 5) return String("?");
  else if (num == 6) return String("@");
  else if (num == 7) return String("A");
  else if (num == 8) return String("B");
  else if (num == 9) return String("C");
  else return "D";
}
///RTC Function
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void updateJamSetting() {
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  STARTDIGITTGL
  displayRound(Round);
  displayJamMenitTgl(hour, minute, second);
  ENDDIGITTGL
}

void updateTimerSetting() {
  STARTDIGITTGL
  displayRound(Round);
  displayJamMenitTgl(jamT, menitT, detikT);
  ENDDIGITTGL
}

void deBounching() {
  digitalWrite(pinBuzzer, HIGH);
  delay(100);
  digitalWrite(pinBuzzer, LOW);
  delay(200);
}
void deBounching3dt() {
  digitalWrite(pinBuzzer, HIGH);
  delay(50);
  digitalWrite(pinBuzzer, LOW);
  delay(50);
  digitalWrite(pinBuzzer, HIGH);
  delay(100);
  digitalWrite(pinBuzzer, LOW);
}

void displayRound(byte Round) {
  //  displayDigitTgl(Round % 10);
  //  displayDigitTgl(Round / 10);
  displayDigitTgl(Round);
}

void updateRTC() {
  setDS3231time(  second, minute, hour, dayOfWeek, dayOfMonth, month, year);
}
void clearLine(byte angka) {
  DMDMode = GRAPHICS_ON;
  //clear line  2356890
  for (byte x = 48; x < 52; x++) { //garis pada C on
    dmd.setPixel(16, x, DMDMode);
    //delay(100);
    dmd.setPixel(16, 55, DMDMode);
  }

  //angka 147
  if (angka == 1 || angka == 4 || angka == 7) { //garis pada C off
    for (byte x = 52; x < 56; x++) {
      dmd.setPixel(16, x, GRAPHICS_ON);
      //delay(100);
    }
  }

  for (byte x = 56; x < 64; x++) { //Outline
    dmd.setPixel(16, x, DMDMode);
    //delay(100);
  }

  if (angka == 2 || angka == 3 || angka == 4 || angka == 5 || angka == 6 || angka == 8 || angka == 9) { //G on
    for (byte x = 32; x < 48; x++) {
      if (x == 38 || x == 39) continue;
      dmd.setPixel(16, x, DMDMode);
      //delay(100);
    }
  }
  if (angka == 1 || angka == 7 || angka == 0) { //G off
    for (byte x = 32; x < 48; x++) {
      dmd.setPixel(16, x, DMDMode);
      //delay(100);
    }
  }


}
