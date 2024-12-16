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
const int trigPin1 = 6;
const int echoPin1 = 7;
const int trigPin2 = 4;
const int echoPin2 = 5;

// MQTT topic
char mqtt_topic_demo[] = "student/CASA0014/light/2/pixel/";

// Variables to store the state of the sensors and the count of people
int peopleCount = 0;
bool u1 = false;
bool u2 = false;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

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

  checkUltrasonicSensor(trigPin1, echoPin1);
  checkUltrasonicSensor(trigPin2, echoPin2);

  bool isSensor1Triggered = u1;
  bool isSensor2Triggered = u2;

  // detect change
  checkPersonCount(isSensor1Triggered, isSensor2Triggered);

  // Reset sensor triggered flags
  u1 = false;
  u2 = false;

  Serial.print("There are ");
  Serial.print(peopleCount);
  Serial.println(" people in the room");

  sendCurrentLightState();
  delay(500);
}

void checkUltrasonicSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  if (distance < 5) {
    if (trigPin == trigPin1) {
      u1 = true;
    } else if (trigPin == trigPin2) {
      u2 = true;
    }
  }
}

void checkPersonCount(bool sensor1Triggered, bool sensor2Triggered) {
  if (sensor1Triggered && !sensor2Triggered) {
    peopleCount++;
  } else if (!sensor1Triggered && sensor2Triggered) {
    if (peopleCount > 0) {
      peopleCount--;
    }
  }
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
