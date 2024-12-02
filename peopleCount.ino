#define ULTRASONIC_IN_PIN_1 7   // The first ultrasonic sensor receiver pin E1
#define ULTRASONIC_OUT_PIN_1 6  // the first ultrasonic sensor transmitter pin T1
#define ULTRASONIC_IN_PIN_2 5   // The second ultrasonic sensor receiver pin E2
#define ULTRASONIC_OUT_PIN_2 4  // the second ultrasonic sensor transmitter pin T2

float distance1 = 0;
float distance2 = 0;
int peopleCount = 0;  

bool u1 = false;
bool u2 = false;

void setup() {
  Serial.begin(115200);
  pinMode(ULTRASONIC_IN_PIN_1, INPUT);
  pinMode(ULTRASONIC_OUT_PIN_1, OUTPUT);
  pinMode(ULTRASONIC_IN_PIN_2, INPUT);
  pinMode(ULTRASONIC_OUT_PIN_2, OUTPUT);
}

void loop() {
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