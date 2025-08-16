#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h> // Required for log()

// Pin configuration
#define THERMISTOR_PIN A0  // Analog pin connected to the thermistor
#define SERIES_RESISTOR 10000  // 10kΩ series resistor

// Thermistor parameters
#define NOMINAL_RESISTANCE 10000 // 10kΩ thermistor resistance at 25°C
#define NOMINAL_TEMPERATURE 25.0 // 25°C
#define B_COEFFICIENT 3950  // Beta coefficient for the thermistor

// Calibration offset (adjust based on calibration)
#define CALIBRATION_OFFSET -5.0  // Example offset; adjust as needed

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Thermistor Temp");
  display.display();
  delay(1000);
}

void loop() {
  int adcValue = analogRead(THERMISTOR_PIN);
  Serial.print("ADC Value: ");
  Serial.println(adcValue);

  // Check for open circuit
  if (adcValue == 0) {
    Serial.println("Error: Open circuit detected.");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Error: Open circuit");
    display.display();
    delay(1000);
    return;
  }

  // Convert ADC value to voltage
  float voltage = adcValue * (5.0 / 1023.0);

  // Calculate thermistor resistance
  float resistance = (5.0 - voltage) * SERIES_RESISTOR / voltage;

  // Apply the B-parameter equation
  float steinhart;
  steinhart = resistance / NOMINAL_RESISTANCE;     // (R/Ro)
  steinhart = log(steinhart);                      // ln(R/Ro)
  steinhart /= B_COEFFICIENT;                      // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                     // Invert
  float tempC = steinhart - 273.15;                // Convert to Celsius

  // Apply calibration offset
  tempC += CALIBRATION_OFFSET;

  float tempF = (tempC * 9.0 / 5.0) + 32.0;         // Convert to Fahrenheit

  // Display temperature on Serial Monitor
  Serial.print("Temp: ");
  Serial.print(tempC, 1);
  Serial.print(" °C | ");
  Serial.print(tempF, 1);
  Serial.println(" °F");

  // Display temperature on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(tempC, 1);
  display.print(" C");

  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(tempF, 1);
  display.print(" F");

  display.display();
  delay(1000);  // Wait for 1 second before updating again
}
