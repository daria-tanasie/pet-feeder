#include <Arduino.h>
#include <DIYables_Keypad.h>
#include <Wire.h>
#include <LCD_I2C.h>

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte pin_rows[ROW_NUM] = { 9, 8, 7, 6 };
byte pin_column[COLUMN_NUM] = { 5, 4, 3, 2 };

DIYables_Keypad keypad = DIYables_Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

LCD_I2C lcd(0x27, 16, 2);
uint8_t col_lcd = 0;
uint8_t row_lcd = 0;

void setup() {
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);

  Serial.begin(9600);
  delay(1000);
  Serial.println("Keypad 4x4 example");
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  // lcd.clear();
  
  if (key) {
    // Serial.println(key);
    lcd.print(key);
    delay(200);
    if (col_lcd == 17 && row_lcd == 0) {
      row_lcd = 1;
      col_lcd = 0;
    } else if (col_lcd == 16 && row_lcd == 1) {
      lcd.clear();
      row_lcd = 0;
      col_lcd = 0;
    } else {
      col_lcd++;
    }
    lcd.setCursor(col_lcd, row_lcd);
  }

}
