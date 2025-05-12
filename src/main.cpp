#include <Arduino.h>
#include <DIYables_Keypad.h>
#include <Wire.h>
#include <LCD_I2C.h>
#include <Servo.h>
#include <DS3231.h>

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

Servo servo;
int servo_pos = 0;

DS3231 rtc;
bool century = false;
bool h12Flag;
bool pmFlag;

const int targetHour = 22;
const int targetMinute = 54;

bool hasTriggered = false;

void setup() {
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);

  Serial.begin(9600);
  delay(1000);
  Serial.println("Keypad 4x4 example");

  servo.attach(10);
  servo.write(servo_pos);

  Wire.begin();
  // Serial.begin(57600);

  // rtc.setClockMode(false); // 24h mode
  // rtc.setYear(25);
  // rtc.setMonth(5);
  // rtc.setDate(12);
  // rtc.setHour(22);
  // rtc.setMinute(43);
  // rtc.setSecond(0);

}

void loop() {
  char key = keypad.getKey();

  int currentHour = rtc.getHour(h12Flag, pmFlag);
  int currentMinute = rtc.getMinute();

  if (currentHour == targetHour && currentMinute == targetMinute && !hasTriggered) {
    servo.write(90);
    delay(1000);
    servo.write(0);
    hasTriggered = true;
  }

  if (currentMinute != targetMinute) {
    hasTriggered = false;
  }

  delay(1000);

  
  // if (key) {
  //   // Serial.println(key);
  //   lcd.print(key);
  //   delay(200);
  //   if (col_lcd == 17 && row_lcd == 0) {
  //     row_lcd = 1;
  //     col_lcd = 0;
  //   } else if (col_lcd == 16 && row_lcd == 1) {
  //     lcd.clear();
  //     row_lcd = 0;
  //     col_lcd = 0;
  //   } else {
  //     col_lcd++;
  //   }
  //   lcd.setCursor(col_lcd, row_lcd);

  //   if (key == 'A') {
  //     servo_pos += 90;
  //     servo.write(servo_pos);
  //     delay(15);
  //   }

    // if (key == '1') {
    //   servo_pos -= 90;
    //   servo.write(servo_pos);
    //   delay(15);
    // }


  // }

}
