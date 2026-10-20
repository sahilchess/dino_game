#include <Arduino.h>
#include <LiquidCrystal.h>

const int lcd_rs = 14;
const int lcd_e = 13;
const int lcd_d4 = 32;
const int lcd_d5 = 33;
const int lcd_d6 = 25;
const int lcd_d7 = 26;
const int button_pin = 27;

LiquidCrystal lcd(lcd_rs, lcd_e, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

bool jumping = false;
bool game_started = false;
bool game_over = false;
int obstacle_column = 15;
int score = 0;
unsigned long jump_started_at = 0;
unsigned long obstacle_moved_at = 0;
unsigned long button_changed_at = 0;
bool last_button_reading = HIGH;
bool button_state = HIGH;

const unsigned long debounce_ms = 100;
const unsigned long jump_duration_ms = 500;

byte dino_character[8] = {
  0b00100,
  0b01110,
  0b01111,
  0b11110,
  0b11100,
  0b11110,
  0b01010,
  0b01010
};

byte cactus_character[8] = {
  0b00100,
  0b00100,
  0b10101,
  0b10101,
  0b11111,
  0b00100,
  0b00100,
  0b00100
};

bool buttonPressed() {
  bool reading = digitalRead(button_pin);

  if (reading != last_button_reading) {
    button_changed_at = millis();
    last_button_reading = reading;
  }

  if (millis() - button_changed_at >= debounce_ms && reading != button_state) {
    button_state = reading;
    return button_state == LOW;
  }

  return false;
}

void drawGame() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(11, 0);
  lcd.print("S:");
  lcd.print(score);

  lcd.setCursor(1, jumping ? 0 : 1);
  lcd.write(byte(0));

  lcd.setCursor(obstacle_column, 1);
  lcd.write(byte(1));
}

void startGame() {
  score = 0;
  obstacle_column = 15;
  jumping = false;
  game_over = false;
  game_started = true;
  obstacle_moved_at = millis();
  drawGame();
}
