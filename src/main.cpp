#include <Arduino.h>
#include <LiquidCrystal.h>

constexpr uint8_t lcdRsPin = 14;
constexpr uint8_t lcdEnablePin = 13;
constexpr uint8_t lcdD4Pin = 32;
constexpr uint8_t lcdD5Pin = 33;
constexpr uint8_t lcdD6Pin = 25;
constexpr uint8_t lcdD7Pin = 26;
constexpr uint8_t jumpButtonPin = 27;
constexpr uint8_t lcd_columns = 16;
constexpr uint8_t dino_column = 1;
constexpr uint8_t dino_glyph = 0;
constexpr uint8_t cactus_glyph = 1;

LiquidCrystal lcd(lcdRsPin, lcdEnablePin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

// the cactus gets the far seat.
bool dinoIsJumping = false;
bool gameIsRunning = false;
int cactus_column = lcd_columns - 1;
int score = 0;
unsigned long jumpStartedAt = 0;
unsigned long cactusMovedAt = 0;
unsigned long buttonChangedAt = 0;
bool lastButtonReading = HIGH;
bool jumpButtonState = HIGH;

constexpr unsigned long buttonDebounceMs = 100;
constexpr unsigned long jumpDurationMs = 500;

byte dinoGlyphData[8] = {
  0b00100,
  0b01110,
  0b01111,
  0b11110,
  0b11100,
  0b11110,
  0b01010,
  0b01010
};

byte cactusGlyphData[8] = {
  0b00100,
  0b00100,
  0b10101,
  0b10101,
  0b11111,
  0b00100,
  0b00100,
  0b00100
};

bool jumpButtonWasPressed() {
  bool reading = digitalRead(jumpButtonPin);

  // a little patience for the button.
  if (reading != lastButtonReading) {
    buttonChangedAt = millis();
    lastButtonReading = reading;
  }

  if (millis() - buttonChangedAt >= buttonDebounceMs && reading != jumpButtonState) {
    jumpButtonState = reading;
    return jumpButtonState == LOW;
  }

  return false;
}

void draw_playfield() {
  // clear both rows before drawing the action again.
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(11, 0);
  lcd.print("S:");
  lcd.print(score);

  lcd.setCursor(dino_column, dinoIsJumping ? 0 : 1);
  lcd.write(dino_glyph);

  lcd.setCursor(cactus_column, 1);
  lcd.write(cactus_glyph);
}

void start_round() {
  score = 0;
  cactus_column = lcd_columns - 1;
  dinoIsJumping = false;
  gameIsRunning = true;
  cactusMovedAt = millis();
  draw_playfield();
}

void showGameOver() {
  gameIsRunning = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void setup() {
  Serial.begin(115200);
  pinMode(jumpButtonPin, INPUT_PULLUP);

  lcd.begin(lcd_columns, 2);
  lcd.createChar(dino_glyph, dinoGlyphData);
  lcd.createChar(cactus_glyph, cactusGlyphData);
  lcd.setCursor(0, 0);
  lcd.print("Start Game?");
  lcd.setCursor(0, 1);
  lcd.print("click to start");
}

void loop() {
  bool jump_pressed = jumpButtonWasPressed();

  if (!gameIsRunning) {
    if (jump_pressed) {
      start_round();
    }
    return;
  }

  unsigned long now = millis();
  if (jump_pressed && !dinoIsJumping) {
    dinoIsJumping = true;
    jumpStartedAt = now;
  }

  if (dinoIsJumping && now - jumpStartedAt >= jumpDurationMs) {
    dinoIsJumping = false;
  }

  unsigned long cactusStepMs = max(130UL, 350UL - min(score, 20) * 10UL);
  if (now - cactusMovedAt >= cactusStepMs) {
    cactusMovedAt = now;
    cactus_column--;

    if (cactus_column < 0) {
      cactus_column = lcd_columns - 1;
      score++;
    }

    if (cactus_column == dino_column && !dinoIsJumping) {
      showGameOver();
      return;
    }

    draw_playfield();
  }
}
