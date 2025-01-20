#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize the LCD and Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

// Define pin connections
const int IR1 = 2; // IR sensor for car entry
const int IR2 = 3; // IR sensor for car exit
int maxSlot = 5;
int Slot = 5; // Variable to hold the number of available slots

// Flags to manage state
int flag1 = 0; // Flag for entry
int flag2 = 0; // Flag for exit

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    
    // Initialize the LCD
    lcd.init();
    lcd.backlight();

    // Set up pin modes
    pinMode(IR1, INPUT);
    pinMode(IR2, INPUT);
    
    // Attach servo to pin and set initial position
    myservo.attach(8);
    myservo.write(100);
    delay(1000);
    myservo.write(0);
    
    // Display welcome message on the LCD
    lcd.setCursor(0, 0);
    lcd.print("     ARDUINO    ");
    lcd.setCursor(0, 1);
    lcd.print(" PARKING SYSTEM ");
    delay(2000);
    lcd.clear();
}

void loop() {
    // Check for car entry
    if (digitalRead(IR1) == LOW && flag1 == 0) {
        if (Slot > 0) {
            flag1 = 1;
            // Open the gate if there's space
            if (flag2 == 0) {
                myservo.write(90);
                Slot = Slot - 1;
                delay(1500);
                myservo.write(0);
            }
        } else {
            // Display parking full message
            lcd.setCursor(0, 0);
            lcd.print("    SORRY :(    ");
            lcd.setCursor(0, 1);
            lcd.print(" Parking Full   ");
            delay(3000);
            lcd.clear();
        }
    }

    // Check for car exit
    if (digitalRead(IR2) == LOW && flag2 == 0) {
        flag2 = 1;
        // Open the gate when a car exits
        if (flag1 == 0) {
            myservo.write(90);
            if (Slot < maxSlot){              
              Slot = Slot + 1;
              delay(1500);
              myservo.write(0);
            }
        }
    }

    // Reset flags and close gate if both flags are set
    if (flag1 == 1 && flag2 == 1) {
        delay(1500);
        myservo.write(0);
        flag1 = 0;
        flag2 = 0;
    }

    // Display current status on the LCD
    lcd.setCursor(0, 0);
    lcd.print("    WELCOME!    ");
    lcd.setCursor(0, 1);
    lcd.print("Slot Left: ");
    lcd.print(Slot);
}
