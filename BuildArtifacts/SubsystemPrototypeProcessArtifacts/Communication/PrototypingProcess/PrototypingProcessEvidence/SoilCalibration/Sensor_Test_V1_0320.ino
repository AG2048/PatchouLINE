// THIS WORKS NOW! we have to remember to use the 3.3V input not 5V

void setup(){

  Serial.begin(57600);

}

void loop(){

  Serial.print("Moisture Sensor Value:");
  Serial.println(analogRead(A0));
  delay(300);

}

/*
proxy testing for calibration

have 2.5 cups of soil, add 1/4 cup of water each time so each time
means increasing soil moisture capacity by 10% if it saturates when soil volume = water volume
however that might not be true but it doesn't matter, i can take percentages and test what the maximum is afterwards
then redo the test if necessary

READINGS:
initial: 3
0: 518
10: 571
20: 614
30: 640
40: 654
50: 678
60: 682
70: fluctuates between 500 and 700 really quickly, makes no sense
------saturated------
80:
90:
100:

fully in water: fluctuating but 764 ish
loose soil was between 415 and 450
*/