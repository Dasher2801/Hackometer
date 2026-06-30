#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SensirionI2cScd4x scd4x;

const int I2C_SDA = 8; 
const int I2C_SCL = 9;

void setup() {
  Serial.begin(115200);
  
  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED Display wurde nicht gefunden!"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Raummesser v1.0");
  display.setCursor(10, 35);
  display.println("Starte Sensor...");
  display.display();

  scd4x.begin(Wire);

  uint16_t error;
  char errorMessage[256];
  error = scd4x.stopPeriodicMeasurement();
  if (error) {
    Serial.print("Fehler beim Stoppen der Messung: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  error = scd4x.startPeriodicMeasurement();
  if (error) {
    Serial.print("Fehler beim Starten der Messung: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  Serial.println("System erfolgreich gestartet!");
  delay(1000);
}

void loop() {
  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  bool isDataReady = false;
  uint16_t error;
  char errorMessage[256];

  error = scd4x.getDataReadyFlag(isDataReady);
  if (error) {
    Serial.print("Fehler beim Lesen des Daten-Status: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    return;
  }

  if (isDataReady) {
    error = scd4x.readMeasurement(co2, temperature, humidity);
    
    if (!error) {
      Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
      Serial.print("Temperatur: "); Serial.print(temperature); Serial.println(" *C");
      Serial.print("Luftfeuchtigkeit: "); Serial.print(humidity); Serial.println(" %");
      Serial.println("-------------------------------------");

      display.clearDisplay();
      
      display.setTextSize(1);
      display.setCursor(20, 0);
      display.println("- CO2 MONITOR -");
      
      display.setCursor(15, 18);
      display.setTextSize(2);
      display.print(co2);
      display.setTextSize(1);
      display.print(" ppm");
      
      display.writeLine(0, 40, 128, 40, SSD1306_WHITE);
      
      display.setTextSize(1);
      display.setCursor(0, 46);
      display.print("Temp:   "); 
      display.print(temperature, 1);
      display.print(" C");
      
      display.setCursor(0, 56);
      display.print("Feucht: "); 
      display.print(humidity, 1);
      display.print(" %");
      
      display.display();
    }
  }

  delay(1000);
}