# CASA0014_assessment
*Project purpose*:
Based on two ultrasonic sensors and Arduino MKR 1010 to count how many people in the room, and send message to control the LED Neopixel  through MQTT. This project directly reflect the room occupy situation by different LED color:
peopleCount >= 1 && peopleCount <= 9: red;
peopleCount >= 10 && peopleCount <= 19: yellow;
peopleCount >= 20: green.

*File explanation*:
1) The counting logic in "main01" is: 
When the first ultrasonic sensor detected a person and the second ultrasonic sensor does not detected a person, it count as someone entered room. If the detection sequence is reversed, it is considered that someone has left the room. This counting logic can applies to the places where entry and exit channels are separate, such as some concert venues. 
Then, based on the number of people in the room, control the color of the lights by publishing messages via MQTT.

2) The counting logic in "main02" is: 
If the first ultrasonic sensor detects a distance less than 5cm, start a while loop to judge the second ultrasonic sensor. When the second sensor also detects a distance less than 5cm, determine that someone has entered. If the sequence of sensor triggers is reversed, it is considered that someone is leaving.

*Project progress*:
I already integrated the second people-counting logic with the message-sending mechanism to control the lighting, in order to better apply it to the lab scenario:)

