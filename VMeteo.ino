// VMeteo 
//    by @GalaxyShad
//
// v 0.1.0 - 23.12.2018
// v 1.0.0 - 03.03.2019
// v 1.5.0 - 15.09.2020 
// v 2.0.0 - 20.09.2020
// v 2.0.1 - 18.03.2024 / Добавлена информация о зависимостях

#include <Wire.h>              
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C by Marco Schwartz (1.1.2) https://github.com/johnrickman/LiquidCrystal_I2C
#include <OneWire.h>           // OneWire by Paul Stoffregen          (2.3.7) https://www.pjrc.com/teensy/td_libs_OneWire.html 
#include <DallasTemperature.h> // DallasTemperature by Miles Burton   (3.9.0) https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DHT.h>               // DHT sensor library by Adafruit      (1.4.6) https://github.com/adafruit/DHT-sensor-library
#include <ThreeWire.h>         // RTC by Makuna                       (2.4.2) https://github.com/Makuna/Rtc
#include <RtcDS1302.h>         // RTC by Makuna                       (2.4.2) https://github.com/Makuna/Rtc
#include <FastLED.h>           // FastLED by Daniel Garcia            (3.6.0) https://github.com/FastLED/FastLED

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OUTSIDE_SENSOR_PIN  6
#define INSIDE_SENSOR_PIN   2

#define LED_PIN             7

#define BUTTON_LEFT         8        // Пин левой кнопки
#define BUTTON_CENTER       9        // Пин центральной кнопки
#define BUTTON_RIGHT        10       // Пин правой кнопки

#define RTC_IO              4
#define RTC_SCLK            5
#define RTC_CE              3

#define BRIGHTNESS          255      // Яркость светодиода

LiquidCrystal_I2C lcd(0x3F,16,2);

OneWire wireOutSensor(OUTSIDE_SENSOR_PIN);
DallasTemperature outSensor(&wireOutSensor);

DHT inSensor(INSIDE_SENSOR_PIN, DHT11);

ThreeWire wiresRtc(RTC_IO, RTC_SCLK, RTC_CE);
RtcDS1302<ThreeWire> rtc(wiresRtc);

RtcDateTime curTime;

CRGB led[1];

int outTemp = 0;
int inTemp  = 0;
int lastUpdTime = 0;

int tempColor = 0;

int btnTimerLeft = 0;
int btnTimerRight = 0;

byte hour = 0;
byte minute = 0;
byte setMode = 0;

bool timeSetMode = false;
bool btnCenterPressed = false;

#include "Symb.h"
#include "Color.h"

void lcdPrintCenter(char* str, int line) {
  lcd.setCursor(8 - strlen(str) / 2, line);
  lcd.print(str);
}

void setup() {
  // Кнопки
  pinMode(BUTTON_LEFT,   INPUT_PULLUP);  
  pinMode(BUTTON_CENTER, INPUT_PULLUP);  
  pinMode(BUTTON_RIGHT,  INPUT_PULLUP);  

  // Светодиод
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(led, 1);
  FastLED.setBrightness( BRIGHTNESS );

  // Инициализация дисплея и загрузка символов
  lcd.init();
  lcd.backlight();
  lcd.createChar(S_TOP, sTop);
  lcd.createChar(S_BOT, sBot);
  lcd.createChar(S_HALF,sHalf);
  lcd.createChar(S_F1, sF1);
  lcd.createChar(S_F2, sF2);
  lcd.createChar(S_F3, sF3);
  lcd.createChar(S_PL1, sPl1);
  lcd.createChar(S_PL2, sPl2);

  // Отображение логотипа системы
  led[0] = 0xb464ff;
  FastLED.show();
  
  lcdPrintCenter("Powered by", 0);
  lcdPrintCenter("VadiK Meteo 2.0", 1);
  delay(1250);

  // Инициализация наружного сенсора
  lcdPrintCenter("initializing", 0);
  lcdPrintCenter("outside sensor...", 1);
  outSensor.begin();
  outSensor.setResolution(12);
  
  // Инициализация внутреннего сенсора
  lcdPrintCenter("inside sensor...", 1);
  inSensor.begin();

  // Инициализация часов
  lcdPrintCenter("RTC...", 1);
  rtc.Begin();

  lcd.clear();
  
  drawSymb(10, 0);
  drawSymb(0, 3);
  drawSymb(0, 7);
  lcd.setCursor(11, 0);
  lcd.print("ff:ff");
  lcd.setCursor(11, 1);
  lcd.print("i");
  lcd.write(0xDF);
  lcd.print("nul");
}

void loop() {
  
  if ((millis() - lastUpdTime > 1000) && (!timeSetMode)) {
    lastUpdTime = millis();
    
    //Датчик улицы
    outSensor.requestTemperatures();
    outTemp = outSensor.getTempCByIndex(0);

    if (outTemp > 0) {
      drawSymb(11, 2); // Плюс
    } else if (outTemp < 0) {
      lcd.setCursor(2, 0); //Минус
      lcd.print("_");
    } else {
      lcd.setCursor(2, 0); //Пустота
      lcd.print(" ");
      lcd.setCursor(2, 1); //Пустота
      lcd.print(" ");
    }

    // Отрисовка большими символами
    drawSymb(abs(outTemp) / 10, 3);
    drawSymb(abs(outTemp) % 10, 7);

    // Индикация светом
    tempColor = 35 + constrain(outTemp, -35, 34);

    // Датчик дома
    inTemp = inSensor.readTemperature();
    lcd.setCursor(13, 1);
    if (inTemp > 0)      lcd.print('+');
    else if (inTemp < 0) lcd.print('-');
    else                 lcd.print(' ');
    lcd.print(abs(inTemp));

    //Время
    hour   = rtc.GetDateTime().Hour();
    minute = rtc.GetDateTime().Minute();

    lcd.setCursor(11, 0);
    if (hour < 10) lcd.print(' ');
    lcd.print(hour);
    lcd.print(':');
    if (minute < 10) lcd.print('0');
    lcd.print(minute);
  }

  if (timeSetMode) {
    if (digitalRead(BUTTON_LEFT) == LOW)  btnTimerLeft++;
    else btnTimerLeft = 0;
    if (digitalRead(BUTTON_RIGHT) == LOW) btnTimerRight++;
    else btnTimerRight = 0;

    if ((btnTimerLeft >= 1000) || (btnTimerLeft == 1)) {
      btnTimerLeft = 1000;
      if (setMode == 0) { hour--; if (hour == 0) hour = 23; }
      else              { minute--; if (minute == 0) minute = 59;}
      delay(500);
    }

    if ((btnTimerRight >= 1000) || (btnTimerRight == 1)) {
      btnTimerRight = 1000;
      if (setMode == 0) hour++;
      else              minute++;
      delay(500);
    }

    if (hour > 23) hour = 0;
    if (minute > 59) minute = 0;
    
    lcd.setCursor(11, 0);
    if (hour < 10) lcd.print(' ');
    lcd.print(hour);
    lcd.print(':');
    if (minute < 10) lcd.print('0');
    lcd.print(minute);

    if ((millis() % 500 > 250) && (btnTimerRight == 0) && (btnTimerLeft == 0)) {
      if (setMode == 0) {
        lcd.setCursor(11, 0);
        lcd.print("  ");
      } else {
        lcd.setCursor(14, 0);
        lcd.print("  ");
      }
    }
  }

  if (digitalRead(BUTTON_CENTER) == LOW && !btnCenterPressed) {
    if (timeSetMode) {
      setMode++;   
      if (setMode > 1) {
        rtc.SetDateTime(RtcDateTime(curTime.Year(), curTime.Month(), curTime.Day(), hour, minute, curTime.Second()));
        setMode = 0;
        timeSetMode = false;
      }        
    } else {
      curTime = rtc.GetDateTime();
      btnTimerLeft = 0;
      btnTimerRight = 0;
      timeSetMode = true;
    }
    btnCenterPressed = true;
  }

  if (digitalRead(BUTTON_CENTER) == HIGH)
    btnCenterPressed = false;

  if (hour >= 0 && hour < 6)
    FastLED.setBrightness(0);
  else
    FastLED.setBrightness( BRIGHTNESS );

  led[0] = tempPal[tempColor];
  FastLED.show();
}                                                                                                                                         
