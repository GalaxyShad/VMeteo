// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 3
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define SET_COMPILE_TIME false

#define ONE_WIRE_BUS 6      // Пин внешнего датчика температуры
#define LED_PIN      7      // Пин светодиода              
#define BUTTON_LEFT  8      // Пин левой кнопки
#define BUTTON_CENTER  10   // Пин центральной кнопки
#define BUTTON_RIGHT  9     // Пин правой кнопки


ThreeWire myWire(4,5,3);              // Часы --> IO, SCLK, CE
OneWire oneWire(ONE_WIRE_BUS);        // Внешний датчик температуры        
RtcDS1302<ThreeWire> Rtc(myWire);     
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

#define countof(a) (sizeof(a) / sizeof(a[0]))
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

int screen = 0;
int state = 0;                // Состояние(Экран)  0 - Гл. Экран; 1 - Меню настроек; 2 - Настройки времени; 3 - Настройки даты 

// Устанавливаемое время
int setHours = 0;
int setMin = 0;              

// Устанавливаемая дата
int setMonth = 0;
int setDay = 0;
int setYear = 0;

bool isSelected = false;    // Выбрано ли значение 

bool isLeftPressed = false;
bool isRightPressed = false;


// Битмапы частей цифр
uint8_t numberPart1[8] = {
  B00111,
  B00111,
  B00111,
  B00111,
  B00111,
  B00111,
  B00111,
  B00111,
};

uint8_t numberPart2[8] = {
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
};

uint8_t numberPart3[8] = {
  B00111,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00111,
  B00111,
};

uint8_t numberPart4[8] = {
  B11111,
  B11111,
  B00111,
  B00111,
  B00111,
  B00111,
  B11111,
  B11111,
};

uint8_t numberPart5[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00111,
  B00111,
};

uint8_t numberPart6[8] = {
  B00111,
  B00111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
};

uint8_t numberPart7[8] = {
  B00111,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};

uint8_t numberPart8[8] = {
  B00111,
  B00111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00111,
  B00111,
};

void printDateTime(const RtcDateTime& dt, char dest) //dest-0 UART, 1-LCD
{
    char datestring[17];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u %02u.%02u.%02u"),
            dt.Hour(),
            dt.Minute(),
            dt.Day(),
            dt.Month(),
            dt.Year()
            );
    if (dest==1)
    {
        lcd.setCursor(0,0);
        lcd.print(datestring);
    }
}

void printBigTime(const RtcDateTime& dt, char dest) //dest-0 UART, 1-LCD
{
    char datestring[16];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute());
    if (dest==1)
    {
        lcd.setCursor(0,0);
        drawBigNumber(datestring[0], 0);
        drawBigNumber(datestring[1], 2);
        lcd.setCursor(4, 0);
        lcd.print("\xA5");
        lcd.setCursor(4, 1);
        lcd.print("\xA5");
        drawBigNumber(datestring[3], 5);
        drawBigNumber(datestring[4], 7);
    }
}

void lcdPrintCenter(char* str, int line)
{
  lcd.setCursor(8 - strlen(str) / 2, line);
  lcd.print(str);
}

void drawBigNumber(char ch, int c)
{
   switch(ch)
   {
    case '1':
      lcd.setCursor(c, 0);
      lcd.print(' ');
      lcd.setCursor(c+1, 0);
      lcd.write(byte(0));
      lcd.setCursor(c, 1);
      lcd.print(' ');
      lcd.setCursor(c+1, 1);
      lcd.write(byte(0));
      break;
    case '2':
      lcd.setCursor(c, 0);
      lcd.write(byte(2));
      lcd.setCursor(c+1, 0);
      lcd.print("\xFF");
      lcd.setCursor(c, 1);
      lcd.write(byte(0));
      lcd.setCursor(c+1, 1);
      lcd.write(byte(1));
      break;
    case '3':
      lcd.setCursor(c, 0);
      lcd.write(byte(2));
      lcd.setCursor(c+1, 0);
      lcd.print("\xFF");
      lcd.setCursor(c, 1);
      lcd.write(byte(2));
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case '4':
      lcd.setCursor(c, 0);
      lcd.write(byte(4));
      lcd.setCursor(c+1, 0);
      lcd.print("\xFF");
      lcd.setCursor(c, 1);
      lcd.print(' ');
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case '5':
      lcd.setCursor(c, 0);
      lcd.write(byte(0));
      lcd.setCursor(c+1, 0);
      lcd.write(byte(1));
      lcd.setCursor(c, 1);
      lcd.write(byte(2));
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case '6':
      lcd.setCursor(c, 0);
      lcd.write(byte(0));
      lcd.setCursor(c+1, 0);
      lcd.write(byte(1));
      lcd.setCursor(c, 1);
      lcd.write(byte(7));
      lcd.setCursor(c+1, 1);
      lcd.write(byte(3));
      break;
    case '7':
      lcd.setCursor(c, 0);
      lcd.write(byte(6));
      lcd.setCursor(c+1, 0);
      lcd.print("\xFF");
      lcd.setCursor(c, 1);
      lcd.print(' ');
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case '8':
      lcd.setCursor(c, 0);
      lcd.write(byte(7));
      lcd.setCursor(c+1, 0);
      lcd.write(byte(3));
      lcd.setCursor(c, 1);
      lcd.write(byte(7));
      lcd.setCursor(c+1, 1);
      lcd.write(byte(3));
      break;
    case '9':
      lcd.setCursor(c, 0);
      lcd.write(byte(7));
      lcd.setCursor(c+1, 0);
      lcd.write(byte(3));
      lcd.setCursor(c, 1);
      lcd.write(byte(2));
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case '0':
      lcd.setCursor(c, 0);
      lcd.write(byte(5));
      lcd.setCursor(c+1, 0);
      lcd.print("\xFF");
      lcd.setCursor(c, 1);
      lcd.write(byte(4));
      lcd.setCursor(c+1, 1);
      lcd.print("\xFF");
      break;
    case ':':
      lcd.setCursor(c, 0);
      lcd.write(byte(6));
      lcd.setCursor(c, 1);
      lcd.write(byte(6));
      break;
   }
}

void setup()
{
  lcd.init(); // initialize the lcd 
  lcd.backlight();

  // Отображение логотипа системы
  lcdPrintCenter("Powered by", 0);
  lcdPrintCenter("VaDiK Meteo 1.0", 1);
  delay(1500);

  // Загрузка светодиода
  lcd.clear();
  lcd.print("initializing LED...");
  
  led.begin();

  // Загрузка частей больших цифр
  lcd.clear();
  lcd.print("initializing Numbers...");

  lcd.createChar(0, numberPart1);
  lcd.createChar(1, numberPart2);
  lcd.createChar(2, numberPart3);
  lcd.createChar(3, numberPart4);
  lcd.createChar(4, numberPart5);
  lcd.createChar(5, numberPart6);
  lcd.createChar(6, numberPart7);
  lcd.createChar(7, numberPart8);

  // Загрузка кнопок
  lcd.clear();
  lcd.print("initializing Buttons...");

  pinMode(BUTTON_LEFT, INPUT_PULLUP);  
  pinMode(BUTTON_CENTER, INPUT_PULLUP);  
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);  

  // Загрузка контроллера
  lcd.clear();
  lcd.print("initializing DHT...");
  
  dht.begin();

  // Загрузка часов
  lcd.clear();
  lcd.print("initializing Clock...");
  
  Rtc.Begin();
  if (SET_COMPILE_TIME)
  {
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcDateTime now = Rtc.GetDateTime();
    if (now != compiled) 
      Rtc.SetDateTime(compiled);
  }
    if (Rtc.GetIsWriteProtected())

  {
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning())
  {
    Rtc.SetIsRunning(true);
  }
  
  lcd.clear();
  lcd.print("initializing DS...");
  
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);

  lcd.noAutoscroll(); 
  lcd.clear();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float tempC = 0;

  if (digitalRead(BUTTON_RIGHT) == HIGH && isRightPressed)
    isRightPressed = false;
  if (digitalRead(BUTTON_LEFT) == HIGH && isLeftPressed)
    isLeftPressed = false;

  if (state == 0)
  {
    if (digitalRead(BUTTON_RIGHT) == LOW && !isRightPressed)
    {
      screen++;
      lcd.clear();
      isRightPressed = true;
    }

    if (digitalRead(BUTTON_LEFT) == LOW && !isLeftPressed)
    {
      screen--;
      lcd.clear();
      isLeftPressed = true;
    }

    if (screen < 0)
      screen = 2;
    else if (screen > 2)
      screen = 0;

    if (screen == 0)
    {
      RtcDateTime now = Rtc.GetDateTime();
      printDateTime(now,1);
      sensors.requestTemperatures();
      tempC = sensors.getTempC(insideThermometer);
     
      if (!isnan(t) && !isnan(h)) 
      {
        lcd.setCursor(0,1);
        lcd.print("ti=");
        lcd.print(int(t));
        lcd.write(0xDF);
        lcd.print("C");
        lcd.print(" to=");
        lcd.print(int(tempC));
        lcd.write(0xDF);
        lcd.print("C");
      }
    }
    else if (screen == 1)
    {
      sensors.requestTemperatures();
      tempC = sensors.getTempC(insideThermometer);
      
      if (!isnan(t) && !isnan(h)) 
      {
        lcd.setCursor(0,0);
        lcd.print("Inside t");
        lcd.write(0xDF);
        lcd.print("C = ");
        lcd.print(int(t));
        lcd.setCursor(0,1);
        lcd.print("Outside t");
        lcd.write(0xDF);
        lcd.print("C = ");
        lcd.print(int(tempC));
      }
    }
    else if (screen == 2)
    {
      RtcDateTime now = Rtc.GetDateTime();
      printBigTime(now, 1);
      sensors.requestTemperatures();
      tempC = sensors.getTempC(insideThermometer);
      
      if (!isnan(t) && !isnan(h)) 
      {
        lcd.setCursor(10, 0);
        lcd.print("ti=");
        lcd.print(int(t));
        lcd.setCursor(9, 1);
        lcd.print(" to=");
        lcd.print(int(tempC));
      }
    }

    if (digitalRead(BUTTON_CENTER) == LOW)
    {
      screen = 0;
      lcd.clear();
      state = 1;
      delay(500);
    }
  }
  if (state == 1)
  {
     lcd.setCursor(0, 0);
     lcd.print("Time  Date  Exit");

      if (digitalRead(BUTTON_RIGHT) == LOW)
      {
        screen++;
        lcd.clear();
      }

      if (digitalRead(BUTTON_LEFT) == LOW)
      { 
        screen--;
        lcd.clear();    
      }

       if (screen < 0)
        screen = 2;
      else if (screen > 2)
        screen = 0;

     lcd.setCursor(1 + 6 * screen, 1);
     lcd.print("\x5E");
     lcd.print("\x5E");

      if (digitalRead(BUTTON_CENTER) == LOW)
      {
        if (screen == 0)
        {
          screen = 0;
          RtcDateTime now = Rtc.GetDateTime();
          setHours = now.Hour();
          setMin = now.Minute();
          state = 2;
          lcd.clear();
          delay(500);
        }
        if (screen == 1)
        {
          screen = 0;
          RtcDateTime now = Rtc.GetDateTime();
          setDay = now.Day();
          setMonth = now.Month();
          setYear = now.Year();
          state = 3;
          lcd.clear();
          delay(500);
        }
        else if (screen == 2)
        {
          screen = 0;
          lcd.clear();
          state = 0;
        }
      }

  }
  if (state == 2)
  {
    char datestring[7];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u "),
            setHours,
            setMin);
    lcdPrintCenter(datestring, 0);
    lcd.setCursor(12, 0);
    lcd.print("Exit");

    if (screen == 0)
      lcd.setCursor(5, 1);
    else if (screen == 1)
      lcd.setCursor(8, 1);
    else if (screen == 2)
      lcd.setCursor(13, 1);
      
    if (isSelected)
      lcd.print("\x2A\x2A");
    else
      lcd.print("\x5E\x5E");
    
    if (digitalRead(BUTTON_RIGHT) == LOW)
    {
       if (isSelected == false)
        screen++;
       else 
       {
        if (screen == 0)
          setHours++;
        else if (screen == 1)
          setMin++;
       }

       lcd.clear();       
    }

    if (digitalRead(BUTTON_LEFT) == LOW)
    {  
       if (isSelected == false)
        screen--;
       else 
       {
        if (screen == 0)
          setHours--;
        else if (screen == 1)
          setMin--;
       }
    
       lcd.clear();   
    }

    if (setHours < 0)
      setHours = 23;
    else if (setHours > 23)
      setHours = 0;

    if (setMin < 0)
      setMin = 59;
    else if (setMin > 59)
      setMin = 0;

    if (screen > 2)
      screen = 0;
    else if (screen < 0)
      screen = 2;
      
    if (digitalRead(BUTTON_CENTER) == LOW)
    {
      if (screen < 2)
      {
        isSelected = !isSelected;
        lcd.clear();
      }
      if (screen == 2)
      {
        screen = 0;
        RtcDateTime nowTime = Rtc.GetDateTime();
        RtcDateTime newTime(nowTime.Year(), nowTime.Month(), nowTime.Day(), setHours, setMin, 0);
        Rtc.SetDateTime(newTime);
        lcd.clear();
        state = 1;
      }
    } 
  }
  if (state == 3)
  {
    char datestring[11];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u.%02u.%02u"),
            setDay,
            setMonth,
            setYear);
    lcd.setCursor(0, 0);
    lcd.print(datestring);
    lcd.setCursor(12, 0);
    lcd.print("Exit");

    if (screen == 0)
      lcd.setCursor(0, 1);
    else if (screen == 1)
      lcd.setCursor(3, 1);
    else if (screen == 2)
      lcd.setCursor(7, 1);
    else if (screen == 3)
      lcd.setCursor(13, 1);
      
    if (isSelected)
      lcd.print("\x2A\x2A");
    else
      lcd.print("\x5E\x5E");
    
    if (digitalRead(BUTTON_RIGHT) == LOW)
    {
       if (isSelected == false)
        screen++;
       else 
       {
        if (screen == 0)
          setDay++;
        else if (screen == 1)
          setMonth++;
        else if (screen == 2)
          setYear++;
       }

       lcd.clear();       
    }

    if (digitalRead(BUTTON_LEFT) == LOW)
    {  
       if (isSelected == false)
        screen--;
       else 
       {
        if (screen == 0)
          setDay--;
        else if (screen == 1)
          setMonth--;
        else if (screen == 2)
          setYear--;
       }
    
       lcd.clear();   
    }

    if (setDay < 1)
      setDay = 31;
    else if (setDay > 31)
      setDay = 1;

    if (setMonth < 1)
      setMonth= 12;
    else if (setMonth > 12)
      setMonth = 1;

    if (setYear < 2019)
      setYear = 2019;

    if (screen > 3)
      screen = 0;
    else if (screen < 0)
      screen = 3;
      
    if (digitalRead(BUTTON_CENTER) == LOW)
    {
      if (screen < 3)
      {
        isSelected = !isSelected;
        lcd.clear();
      }
      if (screen == 3)
      {
        screen = 0;
        RtcDateTime nowData = Rtc.GetDateTime();
        RtcDateTime newData(setYear, setMonth, setDay, nowData.Hour(), nowData.Minute(), nowData.Second());
        Rtc.SetDateTime(newData);
        lcd.clear();
        state = 1;
      }
    } 
  }

  led.setPixelColor(0, led.Color(0,0,255)); 
  led.show(); // This sends the updated pixel color to the hardware.

  if (state == 0)
    delay(500);
}
