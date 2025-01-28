#define BLYNK_TEMPLATE_ID "template id"
#define BLYNK_TEMPLATE_NAME "Irrigation SYSTEMS"
#define BLYNK_AUTH_TOKEN "authentication token"

#include <WiFi.h>
#include "time.h"
#include <BlynkSimpleEsp32.h>

const char* ntpServer = "in.pool.ntp.org";
const long gmtOffset_sec = 5 * 3600 + 30 * 60;  // GMT+5:30
const int daylightOffset_sec = 0;               // No daylight saving time in India

char ssid[] = "ssid";
char pass[] = "password";
char auth[] = "template link";

BlynkTimer timer;

#define relay_pin 22
#define relayTouch_pin T5
#define modeTouch_pin T9
#define sensor_pin 36
#define wifi_pin 2
#define auto_pin 25
#define manual_pin 33
#define water_pin T7
#define buzzer_pin 19      // Assuming pin 19 is connected to the buzzer
#define Threshold 1450     // Moisture threshold
#define touchThreshold 20  // Touch threshold
#define waterThreshold 50  // water threshold


bool relay_button = LOW;
bool mode_button = 0;
const char m_time[] = "10:47";
const char e_time[] = "17:00";

void setup() {
  Serial.begin(115200);

  // Inputs
  pinMode(sensor_pin, INPUT);
  pinMode(relayTouch_pin, INPUT);
  pinMode(water_pin, INPUT);

  // Outputs
  pinMode(relay_pin, OUTPUT);
  pinMode(wifi_pin, OUTPUT);
  pinMode(manual_pin, OUTPUT);
  pinMode(auto_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);

  // Setting to initial conditions
  digitalWrite(relay_pin, LOW);
  digitalWrite(manual_pin, HIGH);
  digitalWrite(auto_pin, LOW);

  // Wifi connection
  WiFi.begin(ssid, pass);
  
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, LOW);
  Blynk.virtualWrite(V1, LOW);
  Blynk.virtualWrite(V3, "No messages");

  Serial.println("");
  Serial.println("WiFi connected.");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Send moisture of soil in real time every 1 sec
  timer.setInterval(500L, sendMoistureValue);
  alertSound();
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Manual button in app");
  Serial.println(mode_button);
  Serial.println(returnTime());
  delay(1000);
  // Check wifi connection or else blink LED
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    digitalWrite(wifi_pin, HIGH);  // Blink LED to indicate connection loss
    delay(500);
    digitalWrite(wifi_pin, LOW);
    delay(500);
  }
  digitalWrite(wifi_pin, LOW);
  Blynk.run();
  timer.run();

  checkModeTouchSwitch();  // Which mode auto or manual

  if (mode_button == 1) {
    autoWaterPlant();
    digitalWrite(auto_pin, HIGH);
    digitalWrite(manual_pin, LOW);
  } else if (mode_button == 0) {
    checkRelayTouchSwitch();
    digitalWrite(auto_pin, LOW);
    digitalWrite(manual_pin, HIGH);
  }

else {
  Blynk.virtualWrite(V3, "Tank is empty...");
  Serial.println("Tank is empty...");
  Blynk.virtualWrite(V1, LOW);
  digitalWrite(relay_pin, LOW);
  alertSound();
}
}

BLYNK_WRITE(V0) {  // mode button when change is detected
  mode_button = param.asInt();
  if (mode_button == 0) {
    Blynk.virtualWrite(V1, LOW);
    digitalWrite(relay_pin, LOW);
  }
  beep();  // Call the beep function when mode changes
}


BLYNK_WRITE(V1) {  // relay button when change is detected
  if (mode_button == 0) {
    relay_button = param.asInt();
    digitalWrite(relay_pin, relay_button);
  }
  beep();  // Call the beep function when relay state changes
}

void sendMoistureValue() {
  int moistureValue = analogRead(sensor_pin);
  int x = (100 - map(moistureValue, 0, 4095, 0, 100));
  Blynk.virtualWrite(V2, x);
}

void autoWaterPlant() {
  int moistureValue = 4095 - analogRead(sensor_pin);
  Serial.print("Moisture Value:");
  Serial.println(moistureValue);
  if (returnTime() == m_time || returnTime() == e_time) {
    while (moistureValue >= Threshold) {
      digitalWrite(relay_pin, HIGH);
      Blynk.virtualWrite(V1, HIGH);
    }
  } else {
    digitalWrite(relay_pin, LOW);
    Blynk.virtualWrite(V1, LOW);
  }
}


void checkRelayTouchSwitch() {
  // Read the touch input
  int touchValue = touchRead(relayTouch_pin);

  // Print touch value to Serial Monitor
  Serial.print("Relay State: ");
  Serial.println(relay_button);

  // Check if touch value is below the threshold
  if (touchValue < touchThreshold) {
    // Toggle relay state
    relay_button = !relay_button;
    beep();

    // Update relay
    digitalWrite(relay_pin, relay_button);

    // Update Blynk server
    Blynk.virtualWrite(V1, relay_button);

    // Print state to Serial Monitor
    Serial.print("Relay State: ");
    Serial.println(relay_button ? "ON" : "OFF");

    // Wait for a moment to avoid rapid toggling
    delay(500);
  }
}

void checkModeTouchSwitch() {
  // Read the touch input
  int touchValue = touchRead(modeTouch_pin);

  // Print touch value to Serial Monitor
  Serial.print("Mode State: ");
  Serial.println(mode_button);

  // Check if touch value is below the threshold
  if (touchValue < touchThreshold) {
    // Toggle relay state
    mode_button = !mode_button;
    beep();

    // Update relay
    digitalWrite(manual_pin, mode_button);
    digitalWrite(auto_pin, !mode_button);

    // Update Blynk server
    Blynk.virtualWrite(V0, mode_button);

    // Print state to Serial Monitor
    Serial.print("Mode State: ");
    Serial.println(mode_button ? "Auto" : "Manual");

    // Wait for a moment to avoid rapid toggling
    delay(500);
  }
}

void beep() {
  tone(buzzer_pin, 1000, 200);  // Adjust frequency and duration as needed
  delay(200);
  noTone(buzzer_pin);
}

void alertSound() {
  tone(buzzer_pin, 500, 1000);  // Play a short beep at 500 Hz
  delay(100);                   // Pause before restarting
  delay(1000);                  // Pause before restarting
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Print the time on the serial monitor
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // Update Blynk virtual pin V3 with the formatted time
  char formattedTime[32];
  strftime(formattedTime, sizeof(formattedTime), "On -> %B %d %Y, %H:%M:%S", &timeinfo);
  Blynk.virtualWrite(V3, formattedTime);  // Replace V3 with your desired Blynk virtual pin
}

String returnTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "Failed to obtain time";
  }
  char formattedTime[9];
  strftime(formattedTime, sizeof(formattedTime), "%H:%M", &timeinfo);
  Serial.println(formattedTime);
  return formattedTime;
}
