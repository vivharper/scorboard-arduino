#include <FastLED.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// Buttons
#define HOME_INCR_PIN 2
#define HOME_DECR_PIN 3
#define RESET_SCORE_PIN 4
#define SWAP_SIDES_PIN 5
#define AWAY_INCR_PIN 6
#define AWAY_DECR_PIN 7

// Rotary Switches
#define RADIAL_ONE_PIN 8
#define RADIAL_TWO_PIN 9
#define RADIAL_THREE_PIN 10
#define RADIAL_FOUR_PIN 11

// LED Din
#define LED_DATA_PIN 13

// LED Digit Data
#define NUM_LEDS 56
#define PIXELS_PER_SEGMENT  2
#define DIGITS 4

#define DIGIT_ONE 0
#define DIGIT_TWO 1
#define DIGIT_THREE 2
#define DIGIT_FOUR 3

// Init LCD
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);

// Init LEDS & colors
CRGB leds[NUM_LEDS];
CHSV teal = CHSV(200, 255, 255);
CHSV pink = CHSV(210, 255, 255);
CHSV red = CHSV(0, 255, 255);
CHSV colorOFF  = CHSV(0, 0, 0);
CHSV colorHome = teal;
CHSV colorAway = pink;

int homeScore = 0;
int awayScore = 0;
int matchNumber = 1;

bool isSwitched = false;
bool isDeuce = false;
bool isGameOver = false;
int finalScore;
String winner = "";

int homeIncrState = 0;
int lastHomeIncrState = 0;
int homeDecrState = 0;
int lastHomeDecrState = 0;
int resetScoreState = 0;
int lastResetScoreState = 0;
int swapSidesState = 0;
int lastSwapSidesState = 0;
int awayIncrState = 0;
int lastAwayIncrState = 0;
int awayDecrState = 0;
int lastAwayDecrState = 0;

int j = 0;
int i = 0;

// Rainbow effect
static float pulseSpeed = 5;
float valueMin = 100;
float valueMax = 255.0;
float val = valueMin;
static float delta = (valueMax - valueMin) / 2.35040238;
uint8_t hueA;
uint8_t satA;
uint8_t hueB;
uint8_t satB;

// Digits to lit segments array
byte segments[11] = {
  0b1111110,
  0b0011000,
  0b0110111,
  0b0111101,
  0b1011001,
  0b1101101,
  0b1101111,
  0b0111000,
  0b1111111,
  0b1111001,
  0b0000000
};

void setup() {
  // Light setup
  FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Input setup
  pinMode(HOME_INCR_PIN, INPUT);
  pinMode(HOME_DECR_PIN, INPUT);
  pinMode(RESET_SCORE_PIN, INPUT);
  pinMode(SWAP_SIDES_PIN, INPUT);
  pinMode(AWAY_INCR_PIN, INPUT);
  pinMode(AWAY_DECR_PIN, INPUT);
  pinMode(RADIAL_ONE_PIN, INPUT);
  pinMode(RADIAL_TWO_PIN, INPUT);
  pinMode(RADIAL_THREE_PIN, INPUT);
  pinMode(RADIAL_FOUR_PIN, INPUT);

  // Final Score Radial
  if (digitalRead(RADIAL_ONE_PIN) == HIGH) {
    finalScore = 25;
  }
  if (digitalRead(RADIAL_TWO_PIN) == HIGH) {
    finalScore = 21;
  }
  if (digitalRead(RADIAL_THREE_PIN) == HIGH) {
    finalScore = 15;
  }
  if (digitalRead(RADIAL_FOUR_PIN) == HIGH) {
    finalScore = NULL;
  }

  // LCD Setup
  lcd.begin (20,4);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.setCursor(2,0);
  lcd.print("Home");
  lcd.setCursor(9,0);
  lcd.print("#");
  lcd.setCursor(14,0);
  lcd.print("Away");
  lcd.setCursor(0,3);
  lcd.print("Status:");
}

void loop() {
  homeIncrState = digitalRead(HOME_INCR_PIN);
  homeDecrState = digitalRead(HOME_DECR_PIN);
  resetScoreState = digitalRead(RESET_SCORE_PIN);
  swapSidesState = digitalRead(SWAP_SIDES_PIN);
  awayIncrState = digitalRead(AWAY_INCR_PIN);
  awayDecrState = digitalRead(AWAY_DECR_PIN);

  readHomeIncr();
  readHomeDecr();
  readResetScore();
  readSwapSides();
  readAwayIncr();
  readAwayDecr();
  displayScore();
  updateLCD();

  if (isGameOver == false) {
    // Deuce logic
    if (homeScore == (finalScore - 1) && awayScore == (finalScore - 1) && isDeuce == false) {
      isDeuce = true;
      colorHome = red;
      colorAway = red;
    }
    // Calc new final score
    if (homeScore == awayScore && isDeuce == true) {
      finalScore = homeScore > awayScore ? homeScore + 2 : awayScore + 2;
    }
    // Game over logic
    if (homeScore == finalScore || awayScore == finalScore) {
      winner = (homeScore == finalScore) ? "Home" : "Away";
      isGameOver = true;
    } else {
      winner = "";
      isGameOver = false;
    }
    FastLED.show();
  } else if (isGameOver == true) {
    if (j < 255) {
      if (i < NUM_LEDS) {
        if (leds[i]) {
          leds[i].setHue(j);
        }
        i++;
      } else {
        i = 0;
      }
      j++;
    } else {
      j = 0;
    }
    FastLED.show();
  }
}

void readHomeIncr() {
  if (homeIncrState != lastHomeIncrState) {
    if (homeIncrState == HIGH) {
      if (homeScore == finalScore) {
      } else {
        homeScore++;
      }
    }
    delay(50);
  }
  lastHomeIncrState = homeIncrState;
}

void readHomeDecr() {
  if (homeDecrState != lastHomeDecrState) {
    if (homeDecrState == HIGH) {
      if (homeScore == 0) {
      } else {
        homeScore--;
      }
    }
    delay(50);
  }
  lastHomeDecrState = homeDecrState;
}

void readResetScore() {
  if (resetScoreState != lastResetScoreState) {
    if (resetScoreState == HIGH) {
      homeScore = 0;
      awayScore = 0;
      isGameOver = false;
      isDeuce = false;
      colorHome = teal;
      colorAway = pink;
      FastLED.clear();
      FastLED.show();
      if (digitalRead(RADIAL_ONE_PIN) == HIGH) {
        finalScore = 25;
      }
      if (digitalRead(RADIAL_TWO_PIN) == HIGH) {
        finalScore = 21;
      }
      if (digitalRead(RADIAL_THREE_PIN) == HIGH) {
        finalScore = 15;
      }
      if (digitalRead(RADIAL_FOUR_PIN) == HIGH) {
        finalScore = NULL;
      }
    }
    delay(50);
  }
  lastResetScoreState = resetScoreState;
}

void readSwapSides() {
  if (swapSidesState != lastSwapSidesState) {
    if (swapSidesState == HIGH) {
      FastLED.clear();
      FastLED.show();
      isSwitched = !isSwitched;
      if (isSwitched) {
        int temp = homeScore;
        homeScore = awayScore;
        awayScore = temp;

        CHSV tempColor = colorHome;
        colorHome = colorAway;
        colorAway = tempColor;
      } else {
        int temp = awayScore;
        awayScore = homeScore;
        homeScore = temp;

        CHSV tempColor = colorAway;
        colorAway = colorHome;
        colorHome = tempColor;
      }
      displayScore();
    }
    delay(50);
  }
  lastSwapSidesState = swapSidesState;
}

void readAwayIncr() {
  if (awayIncrState != lastAwayIncrState) {
    if (awayIncrState == HIGH) {
      if (awayScore == finalScore) {
      } else {
        awayScore++;
      }
    }
    delay(50);
  }
  lastAwayIncrState = awayIncrState;
}

void readAwayDecr() {
  if (awayDecrState != lastAwayDecrState) {
    if (awayDecrState == HIGH) {
      if (awayScore == 0) {
      } else {
        awayScore--;
      }
    }
    delay(50);
  }
  lastAwayDecrState = awayDecrState;
}

void displayScore() {
  int homeOnes = homeScore % 10;
  int homeTens = homeScore / 10;
  int awayOnes = awayScore % 10;
  int awayTens = awayScore / 10;

  updateSegments(DIGIT_FOUR, awayOnes, "B");
  if (awayScore == 0) {
    updateSegments(DIGIT_THREE, 10, "B");
  } else {
    updateSegments(DIGIT_THREE, awayTens, "B");
  }
  updateSegments(DIGIT_TWO, homeOnes, "A");
  if (homeScore == 0) {
    updateSegments(DIGIT_ONE, 10, "A");
  } else {
    updateSegments(DIGIT_ONE, homeScore, "A");
  }
}

void updateLCD() {
  int homeOnes = homeScore % 10;
  int homeTens = homeScore / 10;
  int awayOnes = awayScore % 10;
  int awayTens = awayScore / 10;

  lcd.setCursor(10,0);
  lcd.print(matchNumber);
  lcd.setCursor(3,1);
  lcd.print(homeTens);
  lcd.print(homeOnes);
  lcd.setCursor(15,1);
  lcd.print(awayTens);
  lcd.print(awayOnes);
  lcd.setCursor(7,3);
  if(homeScore == (finalScore - 1) || awayScore == (finalScore - 1)){
    lcd.print("Game point  ");
  } else if(homeScore == finalScore || awayScore == finalScore){
    lcd.print("Game over    ");
  } else {
    lcd.print("In progress  ");
  }
}

void updateSegments(int index, int value, String team) {
  byte seg = segments[value];
  if (team == "Home") {
    for (int i = 6; i >= 0; i--) {
      int offset = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;
      if (homeScore == finalScore - 1) {
        colorHome = pulseColor("Home");
      } else {
        colorHome = CHSV(colorHome.h, colorHome.s, 255);
      }
      CHSV color = seg & 0x01 != 0 ? colorHome : colorOFF;
      for (int x = offset; x < offset + PIXELS_PER_SEGMENT; x++) {
        leds[x] = color;
      }
      seg = seg >> 1;
    }
  } else if (team == "Away") {
    for (int i = 6; i >= 0; i--) {
      int offset = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;
      if (awayScore == finalScore - 1) {
        colorAway = pulseColor("Away");
      } else {
        colorAway = CHSV(colorAway.h, colorAway.s, 255);
      }
      CHSV color = seg & 0x01 != 0 ? colorAway : colorOFF;
      for (int x = offset; x < offset + PIXELS_PER_SEGMENT; x++) {
        leds[x] = color;
      }
      seg = seg >> 1;
    }
  }
}

CHSV pulseColor(String team) {
  if (team == "Home") {
    float dV = ((exp(sin(pulseSpeed * millis() / 2000.0 * PI)) - 0.36787944) * delta);
    val = valueMin + dV;
    hueA = map(val, valueMin, valueMax, colorHome.h, colorHome.h);  // Map hue based on current val
    satA = map(val, valueMin, valueMax, colorHome.s, colorHome.s);  // Map sat based on current val
    return CHSV(hueA, satA, val);
  } else {
    float dV = ((exp(sin(pulseSpeed * millis() / 2000.0 * PI)) - 0.36787944) * delta);
    val = valueMin + dV;
    hueB = map(val, valueMin, valueMax, colorAway.h, colorAway.h);  // Map hue based on current val
    satB = map(val, valueMin, valueMax, colorAway.s, colorAway.s);  // Map sat based on current val
    return CHSV(hueB, satB, val);
  }
}
