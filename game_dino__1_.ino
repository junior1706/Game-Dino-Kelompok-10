#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int switchState = 0;  // Variable to hold the state of the button
int lastSwitchState = 0;  // To store the previous state of the button for debouncing
int buzzerPin = 9;    // Pin for the buzzer
int jumpButton = 2;   // Pin for the jump button
bool dinoUp = false;  // Flag to check if Dino is in the air
bool buttonPressed = false;  // To ensure the button press only triggers once
int j = 15;  // Initial position of the cactus (off the screen)
int score = 0;  // Game score
unsigned long t, t1, t2, t3;  // Timer variables

int objectDelay = 300;  // Basic cactus moving speed
int objectDecrease = 20;  // Decreases delay after every 5 points (makes the game harder)

byte cactus[8] = {  // Cactus sprite
  B00101,
  B10101,
  B10101,
  B10101,
  B10110,
  B01100,
  B00100,
  B00100,
};

byte dino[8] = {  // Dino sprite
  B00000,
  B00111,
  B00101,
  B10111,
  B11100,
  B11111,
  B01101,
  B01100,
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(7, cactus);
  lcd.createChar(6, dino);
  pinMode(buzzerPin, OUTPUT);
  pinMode(jumpButton, INPUT_PULLUP);  // Use internal pull-up resistor
  t = millis();
  t1 = millis();
  t2 = millis();
  t3 = millis();
  Serial.begin(9600);

  // Display initial message
  lcd.setCursor(0, 0);
  lcd.print("  DINO BAR BAR");
  lcd.setCursor(0, 1);
  lcd.print(" Press to Start");

  // Wait for the button press to start the game
  while (digitalRead(jumpButton) == HIGH) {
    delay(10);  // Small delay to avoid bouncing issues
  }

  // Clear the startup message and begin the game
  lcd.clear();
}

void loop() {
  switchState = digitalRead(jumpButton);  // Read button state

  // Implementing debouncing
  if (switchState != lastSwitchState) {
    // If the button state changes (pressed or released), reset the debounce timer
    delay(50);  // Wait for 50ms for debouncing
  }

  // When the button is pressed, and Dino is not already jumping, make Dino jump
  if (switchState == LOW && !dinoUp && !buttonPressed) {
    lcd.setCursor(2, 0);
    lcd.write(6);  // Dino jumps
    dinoUp = true;
    t2 = millis();
    buttonPressed = true;  // Register that the button has been pressed
    tone(buzzerPin, 1000, 100);  // Play success sound when jump happens
  }

  // If Dino is jumping, keep it in the air
  if (dinoUp) {
    lcd.setCursor(2, 0);  // Draw Dino at the new position (in the air)
    lcd.write(6);  // Keep Dino in the air
  }

  // If Dino is not jumping, show it on the ground
  else {
    lcd.setCursor(2, 1);  // Dino on the ground
    lcd.write(6);
  }

  // After a jump, check if Dino should come back down (1.5 seconds)
  if (millis() - t2 >= 500 && dinoUp) {
    t2 = millis();
    lcd.setCursor(2, 1);
    lcd.write(6);  // Move Dino back to the ground (lower row)
    lcd.setCursor(2, 0);
    lcd.print(" ");  // Clear previous position of the Dino in the air
    dinoUp = false;  // Dino falls down
    buttonPressed = false;  // Reset the button pressed state
  }

  // Move cactus from right to left
  if (millis() - t >= objectDelay) {
    t = millis();

    // Clear the old cactus position
    lcd.setCursor(j, 1);
    lcd.print(" ");  // Clear the cactus sprite at its old position

    // Draw the cactus at its new position
    lcd.setCursor(j - 1, 1);
    lcd.write(7);  // Display cactus

    // Move cactus to the left
    j--;
    if (j <= 0) {  // If cactus moves off the left side, reset its position
      j = 15;
    }

    // Display score
    lcd.setCursor(6, 0);
    lcd.print("Score:");
    lcd.print(score);
  }

  // Check if Dino collides with cactus
  if (j == 2 && dinoUp == false) {
    // Collision occurs when cactus is at position 2 (right next to Dino)
    lcd.clear();
    lcd.setCursor(0, 0);
    tone(buzzerPin, 700, 100);  // Play sound on collision
    lcd.print("   PEA GOBLOK");
    lcd.setCursor(0, 1);
    lcd.print("  Press Button");
    score = 0;
    j = 15;
    objectDelay = 300;  // Reset speed to initial value
    // Wait for button press to restart the game
    while (digitalRead(jumpButton) == HIGH) {
      delay(10);
    }
    lcd.clear();
  } else if (j == 2 && dinoUp == true) {
    // Score increment when Dino jumps over the cactus
    score++;
    tone(buzzerPin, 1000, 100);  // Play success sound
    if (score % 5 == 0) {
      objectDelay = objectDelay - objectDecrease;  // Speed up cactus
      if (objectDelay <= 100) {
        objectDecrease = 0;  // Cap the speed
      }
    }
  }

  // Save current switch state for next iteration
  lastSwitchState = switchState;
}
