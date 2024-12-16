#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

// WiFi and MQTT settings
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

// Ultrasonic sensor pins
#define ULTRASONIC_IN_PIN_1 7   // The first ultrasonic sensor receiver pin E1
#define ULTRASONIC_OUT_PIN_1 6  // the first ultrasonic sensor transmitter pin T1
#define ULTRASONIC_IN_PIN_2 5   // The second ultrasonic sensor receiver pin E2
#define ULTRASONIC_OUT_PIN_2 4  // the second ultrasonic sensor transmitter pin T2


// MQTT topic
char mqtt_topic_demo[] = "student/CASA0014/light/2/pixel/";

float distance1 = 0;
float distance2 = 0;
int peopleCount = 0;  

bool u1 = false;
bool u2 = false;


WiFiClient mkrClient;
PubSubClient client(mkrClient);

void setup() {
  Serial.begin(115200);
  pinMode(ULTRASONIC_IN_PIN_1, INPUT);
  pinMode(ULTRASONIC_OUT_PIN_1, OUTPUT);
  pinMode(ULTRASONIC_IN_PIN_2, INPUT);
  pinMode(ULTRASONIC_OUT_PIN_2, OUTPUT);
  
  WiFi.setHostname("Lumina ucfnqze");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("setup complete");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // decect the first sensor
  checkUltrasonicSensor(ULTRASONIC_IN_PIN_1, ULTRASONIC_OUT_PIN_1, distance1);
  // detect the second sensor
  checkUltrasonicSensor(ULTRASONIC_IN_PIN_2, ULTRASONIC_OUT_PIN_2, distance2);

  // If the first ultrasonic sensor detects a distance less than 5cm, start a while loop to judge the second ultrasonic sensor
  if (distance1 < 5) {
    u1 = true;
  }
  if (distance2 < 5) {
    u2 = true;
  }

  while (u1 == true) {
    checkUltrasonicSensor(ULTRASONIC_IN_PIN_2, ULTRASONIC_OUT_PIN_2, distance2);
    Serial.print("The 1st sensor detected, distance 2:");
    Serial.print(distance2);
    Serial.print("   Number of people in the lab：");
    Serial.println(peopleCount);
    if (distance2 <= 5) {
      Serial.print("Entered!  ");
      peopleCount++;
      Serial.print("   Number of people in the lab：");
      Serial.println(peopleCount);
      u1 = false;
      u2 = false;
      delay(2000);
      
    }
  }
  while (u2 == true) {
    checkUltrasonicSensor(ULTRASONIC_IN_PIN_1, ULTRASONIC_OUT_PIN_1, distance1);
    Serial.print("The 2nd sensor detected, distance 1:");
    Serial.print(distance1);
    Serial.print("   Number of people in the lab：");
    Serial.println(peopleCount);
    if (distance1 <= 5) {
      Serial.print("Exited!  ");
      peopleCount--;
      Serial.print("   Number of people in the lab：");
      Serial.println(peopleCount);
      u1 = false;
      u1 = false;
      u2 = false;
      delay(2000);
    }
  }
  sendCurrentLightState();
  delay(500);
}

void checkUltrasonicSensor(int inPin, int outPin, float &distance) {
  digitalWrite(outPin, LOW);
  delayMicroseconds(2);
  digitalWrite(outPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(outPin, LOW);
  long duration = pulseIn(inPin, HIGH);
  distance = duration / 58.0;
}

void sendCurrentLightState() {
  if (peopleCount == 0) {
    sendmqttOff();
  } else if (peopleCount >= 1 && peopleCount <= 9) {
    sendmqttRed();
  } else if (peopleCount >= 10 && peopleCount <= 19) {
    sendmqttYellow();
  } else if (peopleCount >= 20) {
    sendmqttGreen();
  }
}

void sendmqttOff() {
  setLightColor(0, 0, 0);
  Serial.println("MQTT message sent: Light Off");
}

void sendmqttRed() {
  setLightColor(255, 0, 0);
  Serial.println("MQTT message sent: Light Red");
}

void sendmqttYellow() {
  setLightColor(255, 255, 0);
  Serial.println("MQTT message sent: Light Yellow");
}

void sendmqttGreen() {
  setLightColor(0, 255, 0);
  Serial.println("MQTT message sent: Light Green");
}

void setLightColor(int r, int g, int b) {
  char mqtt_message[100];
  for (int i = 0; i < 12; i++) {
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", i, r, g, b);
    Serial.print("Publishing message: ");
    Serial.println(mqtt_message);
    if (client.publish(mqtt_topic_demo, mqtt_message)) {
      Serial.println("Message published");
    } else {
      Serial.println("Failed to publish message");
    }
  }
}

void startWifi() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.print("Trying to connect to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(600);
      Serial.print(".");
    }
    Serial.println("Connected to " + String(ssid));
    Serial.println("IP address: " + WiFi.localIP().toString());
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
