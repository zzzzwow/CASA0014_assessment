#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 4;
const int echoPin2 = 5;

// MQTT setting
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

char mqtt_topic_demo[] = "student/CASA0014/light/26/pixel/";

int personCount = 0; // counter
bool wasSensor1Triggered = false;
bool wasSensor2Triggered = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
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

  int distance1 = measureDistance(trigPin1, echoPin1);
  int distance2 = measureDistance(trigPin2, echoPin2);

  Serial.print("Distance A: ");
  Serial.println(distance1);
  Serial.print("Distance B: ");
  Serial.println(distance2);

    bool isSensor1Triggered = distance1 < 20;
  bool isSensor2Triggered = distance2 < 20;

  // detect change
  checkPersonCount(isSensor1Triggered, isSensor2Triggered, wasSensor1Triggered, wasSensor2Triggered);

  wasSensor1Triggered = isSensor1Triggered;
  wasSensor2Triggered = isSensor2Triggered;

  Serial.print("There are");
  Serial.print(personCount);
  Serial.println("people in the room");

  sendCurrentLightState();
  delay(1000);
}

void checkPersonCount(bool sensor1Triggered, bool sensor2Triggered, bool wasSensor1Triggered, bool wasSensor2Triggered) {
  if (sensor1Triggered && !wasSensor1Triggered && !sensor2Triggered) {
    // The firse sensor detected and the second sensor does not detected, defining there was a person enter room
    personCount++;
  } else if (!sensor1Triggered && wasSensor1Triggered && sensor2Triggered) {
    // The firse sensor does not detected and the second sensor detected, defining there was a person exit room
    if (personCount > 0) {
      personCount--;
    }
  }
}

void updateLightColor() {
  if (personCount == 0) {
    sendmqttOff();
  } else if (personCount >= 1 && personCount <= 9) {
    sendmqttRed();
  } else if (personCount >= 10 && personCount <= 19) {
    sendmqttYellow();
  } else if (personCount >= 20 && personCount <= 30) {
    sendmqttGreen();
  }
}

void sendCurrentLightState() {
  if (personCount >= 1 && personCount <= 9) {
    sendmqttRed();
  } else if (personCount >= 10 && personCount <= 19) {
    sendmqttYellow();
  } else if (personCount >= 20 && personCount <= 30) {
    sendmqttGreen();
  } else {
    sendmqttOff();
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

int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
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