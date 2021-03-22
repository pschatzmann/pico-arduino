
It is usually better to use a TimerAlarmRepeating for blining the LED so that we do not impact any functionality which might need to be executed in the loop()

## Connections 

To test it - this time - we connect an external LED via a resistor at the signal end:

| LED      | Pico              
|----------|-----------
|  -       | GND 
|  Signal  | GP15
