#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

#define buzzer 11

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

String newPasswordString; // hold the new password
char newPassword[6]; // character string of newPasswordString
byte a = 5;
bool isDoorLocked = true;

Password password = Password("1111"); // Enter your password

byte maxPasswordLength = 6;
byte currentPasswordLength = 0;
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'},
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  servo.attach(10);
  servo.write(110); // Start with the door locked
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("DOOR LOCK SYSTEM");
  delay(3000);
  lcd.clear();
}

void loop() {
  if (isDoorLocked) {
    displayPassword();
  } else {
    displayWaterLevel();
    checkDoorLock();
  }
}

void displayPassword() {
  lcd.setCursor(1, 0);
  lcd.print("ENTER PASSWORD");

  char key = keypad.getKey();
  if (key != NO_KEY) {
    delay(60);
    if (key == 'C') {
      resetPassword();
    } else if (key == 'D') {
      dooropen(); // Attempt to open the door
    } else {
      processNumberKey(key);
    }
  }
}

void processNumberKey(char key) {
  lcd.setCursor(a, 1);
  lcd.print("*");
  a++;
  if (a == 11) {
    a = 5;
  }
  currentPasswordLength++;

  password.append(key);

  if (currentPasswordLength == maxPasswordLength) {
    dooropen(); // Attempt to open the door
  }
}

void dooropen() {
  if (password.evaluate()) {
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    servo.write(50); // Unlock the door
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("CORRECT PASSWORD");
    lcd.setCursor(0, 1);
    lcd.print("DOOR OPENED");
    delay(2000);
    lcd.clear();
    isDoorLocked = false; // Set the flag to display the water level
    a = 5;
  } else {
    incorrectPassword();
  }
  resetPassword();
}

void checkDoorLock() {
  char key = keypad.getKey();
  if (key == 'D') {
    doorlocked(); // Lock the door if D is pressed
  }
}

void doorlocked() {
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
  servo.write(110); // Lock the door
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("DOOR LOCKED");
  lcd.setCursor(2, 1);
  lcd.print("PRESS D TO OPEN");
  delay(2000);
  lcd.clear();
  isDoorLocked = true; // Set the flag to show password screen
  a = 5;
}

void incorrectPassword() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
  }
  lcd.setCursor(0, 0);
  lcd.print("WRONG PASSWORD!");
  lcd.setCursor(0, 1);
  lcd.print("PLEASE TRY AGAIN");
  delay(2000);
  lcd.clear();
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  lcd.clear();
  a = 5;
}

void displayWaterLevel() {
  int value = analogRead(A0);
  lcd.setCursor(0, 0);
  lcd.print("Value :");
  lcd.print(value);
  lcd.print("   ");
  Serial.println(value);
  lcd.setCursor(0, 1);
  lcd.print("W Level :");

  if (value == 0) {
    lcd.print("Empty ");
  } else if (value > 1 && value < 350) {
    lcd.print("LOW   ");
  } else if (value > 350 && value < 510) {
    lcd.print("Medium");
  } else if (value > 510) {
    lcd.print("HIGH  ");
  }
}
