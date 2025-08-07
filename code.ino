// Dual pH Regulator with Relay-Controlled Pumps
// Author: Prasan Mangrati 
/*
  Instructions
  NOTE: DO NOT EDIT THE CODE UNLESS YOU KNOW WHAT YOU ARE DOING.


  (Water Solution means your required pH controlled tank eg, Container with fish for your context)
  1. SETUP of Acid pump
      Insert the INPUT valve of Acid pump to Acidic solution
      Insert the OUTPUT valve of Acid Pump to the Water Solution

  2. SETUP of Base pump
      Insert the INPUT valve of Base pump to the Basic solution
      Insert the OUTPUT valve of Base pump to the Water Solution

  3. Use MicroUSB cable for Arduino (USE it with an adapter) and connect to Arduino and power it on.

  4. Use the Provided 12V adapter for motor, connect it directly to the wall socket.
  note: Insulate the live wires properly before usage 
  CAUTION: DO NOT MAKE MODIFICATIONS TO THE WIRING.


  5. IMPORTANT: Keep the BNC board (Board where pH probe is connected) of pH sensor dry and dont interfare with the position of potentiometers.

  6. Clean the pH probe every week for long life and never expost the probe dry in any conditions.

  7. Advice: Its always better to have a separate pH monitor to manually check the pH readings if sth goes wrong.


*/

// Relay for Acid Pump
const int ACID_PUMP_PIN = 5;     

// Relay for Base Pump
const int BASE_PUMP_PIN = 6;     

// Pin for the pH sensor
const int PH_SENSOR_PIN = A0;    

// Time for running the pump (Acid or Base pump)
const int doseDurationInSeconds = 10;  //(in seconds)
const int doseDuration = doseDurationInSeconds*1000;   


// Delay between pH checks of ph Sensor
const int sensorReadInSeconds = 10 //(in Seconds)
const int sensorReadInterval = sensorReadInSeconds*1000; 

const float phTarget = 7.00;
const float phToleration = 0.25;

// Change this after running pH calibration in the Serial Monitor output
float phOffset = 0.00;          


void setup() {
  Serial.begin(9600);
  pinMode(ACID_PUMP_PIN, OUTPUT);
  pinMode(BASE_PUMP_PIN, OUTPUT);
  digitalWrite(ACID_PUMP_PIN, LOW);
  digitalWrite(BASE_PUMP_PIN, LOW);
  Serial.println("Dual pH Regulator Initialized");
  Serial.println("Uncomment phCalibration() in loop() to calibrate.");
}

void loop() {
  mainLoop();
  // Uncomment below to calibrate:
  // phCalibration();
}

void mainLoop() {
  float ph = getPh();
  Serial.print("pH: ");
  Serial.println(ph, 2);

  if (ph < phTarget - phToleration) {
    Serial.println("pH too low → dosing base");
    doseBase();
  } else if (ph > phTarget + phToleration) {
    Serial.println("pH too high → dosing acid");
    doseAcid();
  } else {
    Serial.println("pH within acceptable range");
  }

  delay(sensorReadInterval);
}

void doseAcid() {
  Serial.println("Acid pump ON");
  digitalWrite(ACID_PUMP_PIN, HIGH);
  delay(doseDuration);
  digitalWrite(ACID_PUMP_PIN, LOW);
  Serial.println("Acid pump OFF");
}

void doseBase() {
  Serial.println("Base pump ON");
  digitalWrite(BASE_PUMP_PIN, HIGH);
  delay(doseDuration);
  digitalWrite(BASE_PUMP_PIN, LOW);
  Serial.println("Base pump OFF");
}

float getPh() {
  int buf[10], temp;
  unsigned long avgValue = 0;

  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(PH_SENSOR_PIN);
    delay(10);
  }

  // Sort readings
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // Average middle 6 values
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }

  float voltage = avgValue * 5.0 / 1024 / 6;
  float phValue = 3.5 * voltage + phOffset;
  return phValue;
}

void phCalibration() {
  float measuredPh = getPh();
  Serial.println("Calibration Mode");
  Serial.print("Measured pH: ");
  Serial.println(measuredPh, 2);

  float recommendedOffset = 7.00 - measuredPh;
  Serial.print("👉 Recommended phOffset = ");
  Serial.println(recommendedOffset, 3);
  Serial.println("Copy this value into your code to set phOffset.");
  delay(sensorReadInterval);
}
