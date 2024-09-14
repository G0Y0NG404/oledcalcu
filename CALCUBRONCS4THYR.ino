#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

//  OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Bluetooth setup on pins 10 (TX) and 11 (RX)
SoftwareSerial bluetooth(10, 11);

String receivedData = "";
String expression = "";
int currentNumber = 0;
int total = 0;
bool operatorPending = false;

void setup() {
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED initialization failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();

  // Initialize Bluetooth
  bluetooth.begin(9600);
  Serial.begin(9600); // Optional for debugging
}

void loop() {
  // Check if data is available from Bluetooth
  if (bluetooth.available()) {
    char c = bluetooth.read();
    
    // Check if the character is a digit
    if (isDigit(c)) {
      receivedData += c; // Append the digit to the current number string
      currentNumber = receivedData.toInt(); // Convert to integer
      operatorPending = true; // Indicates that a number was entered and operator is needed next
      
      // Update the OLED display with the live input
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(receivedData);
      display.display();
    } 
    // If '+' is received, build the expression
    else if (c == '+') {
      if (operatorPending) {
        expression += String(currentNumber) + " + ";
        total = currentNumber; // Start with the current number
        receivedData = ""; // Clear received data
        currentNumber = 0; // Reset current number
        operatorPending = false; // Reset operatorPending flag
        
        // Update the OLED display with the live input
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(expression);
        display.display();
      }
    }
    // If '=' is received, calculate the result
    else if (c == '=') {
      if (operatorPending) {
        expression += String(currentNumber);
        total += currentNumber; // Add the last number
        receivedData = ""; // Clear received data
        currentNumber = 0; // Reset current number
        operatorPending = false; // Reset operatorPending flag

        // Display the result
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(expression);
        display.print(" = ");
        display.print(total);
        display.display();
        
        // Reset expression and total for next operation
        expression = "";
        total = 0;
      }
    }
    // If 'C' is received, clear the OLED display and show "Ready"
    else if (c == 'C') {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Ready");
      display.display();
      receivedData = ""; // Clear received data
      currentNumber = 0; // Reset current number
      expression = ""; // Clear expression
      total = 0; // Reset total
      operatorPending = false; // Reset operatorPending flag
    }
  }
}

