
#include "Arduino.h"
#include "SpektrumSatellite.h"

SpektrumSatellite<uint16_t> satellite(Serial2); // Assing satellite to Serial (use Serial1 or Serial2 if available!)


void setup() {
  Serial2.begin(SPEKTRUM_SATELLITE_BPS);
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  // Activate the loggin to the console only if SpektrumSatellite is not using Serial
  satellite.setLog(Serial);
  // we can define the requested binding mode
  satellite.setBindingMode(External_DSM2_11ms);

  //scale the values from 0 to 180 degrees for PWM
  satellite.setChannelValueRange(0, 180);

  // wait forever for data
  satellite.waitForData();
  
}


void loop() {
  if (satellite.getFrame()) {   
    for (int j=0;j<6; j++){
       Channel ch = static_cast<Channel>(j);   
       long value = satellite.getChannelValue(ch);
       Serial.print(value);
       Serial.print(" ");
    }        
    Serial.println();
  }   
}
