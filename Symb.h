// Дополнительные символы 
#define S_NULL  0x20
#define S_FILL  0xFF
#define S_TOP   0
#define S_BOT   1
#define S_HALF  2
#define S_F1    3
#define S_F2    4
#define S_F3    5
#define S_PL1   6
#define S_PL2   7
byte sTop[8] = {
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
};

byte sBot[8] = {
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
 B11111,
};

byte sHalf[8] = {
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
};

byte sF1[8] = {
 B11100,
 B11100,
 B11100,
 B11100,
 B11100,
 B11100,
 B11111,
 B11111,
};

byte sF2[8] = {
 B11100,
 B11100,
 B11100,
 B11100,
 B11110,
 B11111,
 B01111,
 B00111,
};

byte sF3[8] = {
 B00000,
 B00000,
 B00000,
 B00000,
 B01100,
 B11100,
 B11100,
 B11000,
};

byte sPl1[8] = {
 B00000,
 B00000,
 B00000,
 B00000,
 B00100,
 B00100,
 B00100,
 B11111,
};

byte sPl2[8] = {
 B00100,
 B00100,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
};

void drawSymb(byte sym, byte pos) {
  switch(sym) {
    case 0:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_TOP);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 1:
      lcd.setCursor(pos, 0);
      lcd.write(S_TOP);
      lcd.write(S_FILL);
      lcd.write(S_NULL);
      lcd.setCursor(pos, 1);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      break;
    case 2:
      lcd.setCursor(pos, 0);
      lcd.write(S_HALF);
      lcd.write(S_HALF);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      lcd.write(S_BOT);
      break;
    case 3:
      lcd.setCursor(pos, 0);
      lcd.write(S_HALF);
      lcd.write(S_HALF);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_BOT);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 4:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_NULL);
      lcd.write(S_NULL);
      lcd.write(S_FILL);
      break;
    case 5:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_HALF);
      lcd.write(S_HALF);
      lcd.setCursor(pos, 1);
      lcd.write(S_BOT);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 6:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_HALF);
      lcd.write(S_HALF);
      lcd.setCursor(pos, 1);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 7:
      lcd.setCursor(pos, 0);
      lcd.write(S_TOP);
      lcd.write(S_TOP);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_NULL);
      lcd.write(S_NULL);
      lcd.write(S_FILL);
      break;
    case 8:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_HALF);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_FILL);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 9:
      lcd.setCursor(pos, 0);
      lcd.write(S_FILL);
      lcd.write(S_HALF);
      lcd.write(S_FILL);
      lcd.setCursor(pos, 1);
      lcd.write(S_BOT);
      lcd.write(S_BOT);
      lcd.write(S_FILL);
      break;
    case 10: // Буква t
      lcd.setCursor(pos, 0);
      lcd.write(S_F1);
      lcd.write(0xDF);
      lcd.setCursor(pos, 1);
      lcd.write(S_F2);
      lcd.write(S_F3);
      break;
    case 11: // Плюс
      lcd.setCursor(pos, 0);
      lcd.write(S_PL1);
      lcd.setCursor(pos, 1);
      lcd.write(S_PL2);
      break;
  }
}
