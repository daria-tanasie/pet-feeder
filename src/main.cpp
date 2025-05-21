#include <Arduino.h>
#include <DIYables_Keypad.h>
#include <Wire.h>
#include <LCD_I2C.h>
#include <Servo.h>
#include <DS3231.h>
#include <SoftwareSerial.h>

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pin_rows[ROW_NUM] = {10, 9, 8, 7};
byte pin_column[COLUMN_NUM] = {6, 5, 4, 3};

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

volatile bool button_pressed = false;

SoftwareSerial arduinoSer(14, 15);

struct meal{
  int hour;
  int min;
};

meal meals[20]; 
int idx_meals = 0;
int nr_meals = 0;

void button_isr() {
  button_pressed = true;
}

int get_nr_meals() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nr meals/day:");
  lcd.setCursor(0,1);

  String user_input = "";

  while (true) {
    char key = keypad.getKey();

    if (key >= '0' && key <= '9') {
      if (user_input.length() >= 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("sure?;)");
      } else {
        user_input += key;
        lcd.print(user_input);
      }
    } else if (key == '*') {
      if (user_input.length() > 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        delay(1000);
        return user_input.toInt();
      }
    }
  }
}

int min_of_day(int h, int m) {
  return h * 60 + m;
}

void set_time() {
  for (int i = 0; i < nr_meals; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hour/minute:");
    lcd.setCursor(0, 1);

    String user_input = "";
    int cursor = 0;
    while(true) {
      char key = keypad.getKey();

      if (cursor == 5) {
        meals[i].min = user_input.substring(user_input.length() - 2).toInt();
        break;
      }

      if ((cursor == 3 || cursor == 4) && key >= '0' && key <= '9') {
        user_input += key;
        Serial.println(user_input);
        lcd.setCursor(cursor, 1);
        cursor++;
        lcd.print(key);
      }

      if (cursor == 2) {
        meals[i].hour = user_input.toInt();
        Serial.print(user_input);
        Serial.println(":");
        lcd.setCursor(cursor, 1);
        cursor++;
        lcd.print(":");
        user_input = "";
      }

      if ((cursor == 0 || cursor == 1 )
                && key >= '0' && key <= '9') {
        user_input += key;
        Serial.println(user_input);
        lcd.setCursor(cursor, 1);
        cursor++;
        lcd.print(key);
      }
    }
    delay(1000);
  }
  lcd.clear();
}

void swap_meals(int i, int j) {
  meal aux = meals[i];
  meals[i] = meals[j];
  meals[j] = aux;
}

void sort_times() {
  int current_time = min_of_day(rtc.getHour(h12Flag, pmFlag), rtc.getMinute());

  for (int i = 0; i < nr_meals - 1; i++) {
    for (int j = i + 1; j < nr_meals; j++) {
      int ti = min_of_day(meals[i].hour, meals[i].min) - current_time;
      int tj = min_of_day(meals[j].hour, meals[j].min) - current_time;

      if (ti * tj < 0 && ti < tj) {
        swap_meals(i, j);
      } else if (ti * tj > 0 && tj < ti) {
        swap_meals(i, j);
      }
    }
  }

  for (int i = 0; i < nr_meals; i++) {
    Serial.print(meals[i].hour);
    Serial.print(":");
    Serial.println(meals[i].min);
  }
}

int button1 = 2;

void setup() {
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);

  Serial.begin(9600);
  arduinoSer.begin(9600);
  delay(1000);

  servo.attach(11);
  servo.write(0);

  Wire.begin();

  // rtc.setClockMode(false);
  // rtc.setYear(25);
  // rtc.setMonth(5);
  // rtc.setDate(13);
  // rtc.setHour(18);
  // rtc.setMinute(58);
  // rtc.setSecond(0);
  pinMode(button1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1), button_isr, FALLING);
}

void loop() {

  // if (arduinoSer.available()) {
  //   String msg = arduinoSer.readStringUntil('\n');
  //   msg.trim();
  //   Serial.println(msg);
  // }

  int current_hour = rtc.getHour(h12Flag, pmFlag);
  int current_minute = rtc.getMinute();

  if (current_hour == meals[idx_meals].hour
        && current_minute == meals[idx_meals].min) {
    servo.write(90);
    delay(1000);
    servo.write(0);
    idx_meals++;
    if (idx_meals >= nr_meals) {
      idx_meals = 0;
    }
    // aici sa dea esp32 mesaj
    // arduinoSer.println("send_msg");
  }

  int next_feed_m = min_of_day(meals[idx_meals].hour, meals[idx_meals].min)
                      - min_of_day(current_hour, current_minute);
  lcd.setCursor(0, 0);
  lcd.print("Next feed in:");
  lcd.setCursor(0, 1);
  lcd.printf("%d minutes", next_feed_m);
  // de rezolvat: cand urmatoarea masa e maine
  // sa apara din 5 in 5 minute, la final un minut


  if (button_pressed) {
    nr_meals = get_nr_meals();
    set_time();
    lcd.print("All done!");
    lcd.setCursor(0, 1);
    lcd.print("Wait for food!");
    sort_times();
    idx_meals = 0;
    delay(3000);
    lcd.clear();
    button_pressed = false;
  }
}
