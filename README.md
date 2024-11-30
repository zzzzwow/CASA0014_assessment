# CASA0014_assessment
*Project purpose:
Based on two ultrasonic sensors and Arduino MKR 1010 to count how many people in the room, and send message to control the LED Neopixel  through MQTT. This project directly reflect the room occupy situation by different LED color:
1-9 people: red;
10-19 people: yellow;
20-30 people: green.

*Project progress:
The current counting logic is: when the first ultrasonic sensor detected a person and the second ultrasonic sensor does not detected a person, it count as someone entered room. If the detection sequence is reversed, it is considered that someone has left the room. This counting logic can applies to the places where entry and exit channels are separate, such as some concert venues. 

I'm working on the iteration of this counting logic in order to apply this project in CE Lab, please look forward to the next version :)

