// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6AMI2ABQ-"
#define BLYNK_TEMPLATE_NAME "Watering System"
#define BLYNK_AUTH_TOKEN "J47QzBDq6rmxevuPf3zphRto7w5PTy5S"

#include <Fuzzy.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define DHTPIN 27 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 34
#define RELAY_PIN 25

#define MANUAL_CONTROL_PIN V4
#define MODE_PIN V5

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "Huawei";       // WiFi SSID
char pass[] = "huaweiwawei";   // WiFi password
bool manualControl = false;
bool autoMode = true;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

Fuzzy *fuzzy = new Fuzzy();

// Define fuzzy sets and variables
FuzzySet *cold = new FuzzySet(0, 0, 5, 25);
FuzzySet *cool = new FuzzySet(20, 25, 25, 30);
FuzzySet *normal = new FuzzySet(25, 30, 30, 35);
FuzzySet *warm = new FuzzySet(30, 35, 35, 40);
FuzzySet *hot = new FuzzySet(35, 40, 45, 45);

FuzzySet *veryDry = new FuzzySet(0, 0, 0, 25);
FuzzySet *dry = new FuzzySet(20, 25, 40, 45);
FuzzySet *medium = new FuzzySet(40, 45, 60, 65);
FuzzySet *moist = new FuzzySet(60, 65, 80, 85);
FuzzySet *wet = new FuzzySet(80, 85, 100, 100);

FuzzySet *low = new FuzzySet(0, 0, 0, 35);
FuzzySet *mediumSpeed = new FuzzySet(30, 35, 35, 65);
FuzzySet *high = new FuzzySet(60, 65, 100, 100);

FuzzyInput *temperature = new FuzzyInput(1);
FuzzyInput *soilMoisture = new FuzzyInput(2);
FuzzyOutput *pumpSpeed = new FuzzyOutput(1);

BLYNK_WRITE(MODE_PIN) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    autoMode = true;
    Serial.println("Mode: Automatic");
  } else {
    autoMode = false;
    Serial.println("Mode: Manual");
  }
}

BLYNK_WRITE(MANUAL_CONTROL_PIN) {
  int buttonState = param.asInt();
  if (buttonState == 1 && !autoMode) {
    digitalWrite(RELAY_PIN, LOW); // Turn on pump
      lcd.setCursor(13, 1);
      lcd.print("ON");
    Serial.println("Pump ON (Manual)");
  } else if (buttonState == 0 && !autoMode) {
    digitalWrite(RELAY_PIN, HIGH); // Turn off pump
    Serial.println("Pump OFF (Manual)");
      lcd.setCursor(13, 1);
      lcd.print("OFF");
  }
}

void setup() {
  // Initialize the DHT sensor and LCD
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  // Initialize the relay pin
  pinMode(RELAY_PIN, OUTPUT);

  // Add sets to fuzzy variables
  temperature->addFuzzySet(cold);
  temperature->addFuzzySet(cool);
  temperature->addFuzzySet(normal);
  temperature->addFuzzySet(warm);
  temperature->addFuzzySet(hot);

  soilMoisture->addFuzzySet(veryDry);
  soilMoisture->addFuzzySet(dry);
  soilMoisture->addFuzzySet(medium);
  soilMoisture->addFuzzySet(moist);
  soilMoisture->addFuzzySet(wet);

  pumpSpeed->addFuzzySet(low);
  pumpSpeed->addFuzzySet(mediumSpeed);
  pumpSpeed->addFuzzySet(high);

  fuzzy->addFuzzyInput(temperature);
  fuzzy->addFuzzyInput(soilMoisture);
  fuzzy->addFuzzyOutput(pumpSpeed);

  // Define fuzzy rules based on the provided table
  FuzzyRuleAntecedent *ifColdAndVeryDry = new FuzzyRuleAntecedent();
  ifColdAndVeryDry->joinWithAND(cold, veryDry);
  FuzzyRuleConsequent *thenHigh = new FuzzyRuleConsequent();
  thenHigh->addOutput(high);
  fuzzy->addFuzzyRule(new FuzzyRule(1, ifColdAndVeryDry, thenHigh));

  FuzzyRuleAntecedent *ifColdAndDry = new FuzzyRuleAntecedent();
  ifColdAndDry->joinWithAND(cold, dry);
  fuzzy->addFuzzyRule(new FuzzyRule(2, ifColdAndDry, thenHigh));

  FuzzyRuleAntecedent *ifColdAndMedium = new FuzzyRuleAntecedent();
  ifColdAndMedium->joinWithAND(cold, medium);
  FuzzyRuleConsequent *thenLow = new FuzzyRuleConsequent();
  thenLow->addOutput(low);
  fuzzy->addFuzzyRule(new FuzzyRule(3, ifColdAndMedium, thenLow));

  FuzzyRuleAntecedent *ifColdAndMoist = new FuzzyRuleAntecedent();
  ifColdAndMoist->joinWithAND(cold, moist);
  fuzzy->addFuzzyRule(new FuzzyRule(4, ifColdAndMoist, thenLow));

  FuzzyRuleAntecedent *ifColdAndWet = new FuzzyRuleAntecedent();
  ifColdAndWet->joinWithAND(cold, wet);
  fuzzy->addFuzzyRule(new FuzzyRule(5, ifColdAndWet, thenLow));

  FuzzyRuleAntecedent *ifCoolAndVeryDry = new FuzzyRuleAntecedent();
  ifCoolAndVeryDry->joinWithAND(cool, veryDry);
  fuzzy->addFuzzyRule(new FuzzyRule(6, ifCoolAndVeryDry, thenHigh));

  FuzzyRuleAntecedent *ifCoolAndDry = new FuzzyRuleAntecedent();
  ifCoolAndDry->joinWithAND(cool, dry);
  fuzzy->addFuzzyRule(new FuzzyRule(7, ifCoolAndDry, thenHigh));

  FuzzyRuleAntecedent *ifCoolAndMedium = new FuzzyRuleAntecedent();
  ifCoolAndMedium->joinWithAND(cool, medium);
  fuzzy->addFuzzyRule(new FuzzyRule(8, ifCoolAndMedium, thenLow));

  FuzzyRuleAntecedent *ifCoolAndMoist = new FuzzyRuleAntecedent();
  ifCoolAndMoist->joinWithAND(cool, moist);
  fuzzy->addFuzzyRule(new FuzzyRule(9, ifCoolAndMoist, thenLow));

  FuzzyRuleAntecedent *ifCoolAndWet = new FuzzyRuleAntecedent();
  ifCoolAndWet->joinWithAND(cool, wet);
  fuzzy->addFuzzyRule(new FuzzyRule(10, ifCoolAndWet, thenLow));

  FuzzyRuleAntecedent *ifNormalAndVeryDry = new FuzzyRuleAntecedent();
  ifNormalAndVeryDry->joinWithAND(normal, veryDry);
  fuzzy->addFuzzyRule(new FuzzyRule(11, ifNormalAndVeryDry, thenHigh));

  FuzzyRuleAntecedent *ifNormalAndDry = new FuzzyRuleAntecedent();
  ifNormalAndDry->joinWithAND(normal, dry);
  fuzzy->addFuzzyRule(new FuzzyRule(12, ifNormalAndDry, thenHigh));

  FuzzyRuleAntecedent *ifNormalAndMedium = new FuzzyRuleAntecedent();
  ifNormalAndMedium->joinWithAND(normal, medium);
  FuzzyRuleConsequent *thenMediumSpeed = new FuzzyRuleConsequent();
  thenMediumSpeed->addOutput(mediumSpeed);
  fuzzy->addFuzzyRule(new FuzzyRule(13, ifNormalAndMedium, thenMediumSpeed));

  FuzzyRuleAntecedent *ifNormalAndMoist = new FuzzyRuleAntecedent();
  ifNormalAndMoist->joinWithAND(normal, moist);
  fuzzy->addFuzzyRule(new FuzzyRule(14, ifNormalAndMoist, thenLow));

  FuzzyRuleAntecedent *ifNormalAndWet = new FuzzyRuleAntecedent();
  ifNormalAndWet->joinWithAND(normal, wet);
  fuzzy->addFuzzyRule(new FuzzyRule(15, ifNormalAndWet, thenLow));

  FuzzyRuleAntecedent *ifWarmAndVeryDry = new FuzzyRuleAntecedent();
  ifWarmAndVeryDry->joinWithAND(warm, veryDry);
  fuzzy->addFuzzyRule(new FuzzyRule(16, ifWarmAndVeryDry, thenHigh));

  FuzzyRuleAntecedent *ifWarmAndDry = new FuzzyRuleAntecedent();
  ifWarmAndDry->joinWithAND(warm, dry);
  fuzzy->addFuzzyRule(new FuzzyRule(17, ifWarmAndDry, thenHigh));

  FuzzyRuleAntecedent *ifWarmAndMedium = new FuzzyRuleAntecedent();
  ifWarmAndMedium->joinWithAND(warm, medium);
  fuzzy->addFuzzyRule(new FuzzyRule(18, ifWarmAndMedium, thenMediumSpeed));

  FuzzyRuleAntecedent *ifWarmAndMoist = new FuzzyRuleAntecedent();
  ifWarmAndMoist->joinWithAND(warm, moist);
  fuzzy->addFuzzyRule(new FuzzyRule(19, ifWarmAndMoist, thenLow));

  FuzzyRuleAntecedent *ifWarmAndWet = new FuzzyRuleAntecedent();
  ifWarmAndWet->joinWithAND(warm, wet);
  fuzzy->addFuzzyRule(new FuzzyRule(20, ifWarmAndWet, thenLow));

  FuzzyRuleAntecedent *ifHotAndVeryDry = new FuzzyRuleAntecedent();
  ifHotAndVeryDry->joinWithAND(hot, veryDry);
  fuzzy->addFuzzyRule(new FuzzyRule(21, ifHotAndVeryDry, thenHigh));

  FuzzyRuleAntecedent *ifHotAndDry = new FuzzyRuleAntecedent();
  ifHotAndDry->joinWithAND(hot, dry);
  fuzzy->addFuzzyRule(new FuzzyRule(22, ifHotAndDry, thenHigh));

  FuzzyRuleAntecedent *ifHotAndMedium = new FuzzyRuleAntecedent();
  ifHotAndMedium->joinWithAND(hot, medium);
  fuzzy->addFuzzyRule(new FuzzyRule(23, ifHotAndMedium, thenMediumSpeed));

  FuzzyRuleAntecedent *ifHotAndMoist = new FuzzyRuleAntecedent();
  ifHotAndMoist->joinWithAND(hot, moist);
  fuzzy->addFuzzyRule(new FuzzyRule(24, ifHotAndMoist, thenMediumSpeed));

  FuzzyRuleAntecedent *ifHotAndWet = new FuzzyRuleAntecedent();
  ifHotAndWet->joinWithAND(hot, wet);
  fuzzy->addFuzzyRule(new FuzzyRule(25, ifHotAndWet, thenLow));

  Serial.begin(115200);

  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
    // Read sensors
    float temp = dht.readTemperature();
    float soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

    // Map soil moisture value to percentage
    int soilMoisturePercent = map(soilMoistureValue, 2700, 940, 0, 100);

    // Set inputs to fuzzy system
    fuzzy->setInput(1, temp);
    fuzzy->setInput(2, soilMoisturePercent);
    fuzzy->fuzzify();

    // Get fuzzy output
    float pumpSpeedPercent = fuzzy->defuzzify(1);

    // Display data on LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Moist: ");
    lcd.print(soilMoisturePercent);
    lcd.print(" %");

    // Print data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" C, Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.print(" %, Moisture Value: ");
    Serial.print(soilMoistureValue);
    Serial.print(", Pump Speed: ");
    Serial.print(pumpSpeedPercent);
    Serial.print(" %");
    Serial.println("");

    // Send data to Blynk
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V2, soilMoisturePercent);
    Blynk.virtualWrite(V3, pumpSpeedPercent);

  if (autoMode) {
    // Control water pump automatically
    if (soilMoisturePercent < 85) {
      digitalWrite(RELAY_PIN, LOW); // Turn on pump
      Serial.println("Pump ON (Auto)");
      lcd.setCursor(13, 1);
      lcd.print("ON");
    } else if (soilMoisturePercent >= 85) {
      digitalWrite(RELAY_PIN, HIGH); // Turn off pump
      Serial.println("Pump OFF (Auto)");
      lcd.setCursor(13, 1);
      lcd.print("OFF");
    }
  }

  Blynk.run();
  delay(2000);
  lcd.clear();
}
