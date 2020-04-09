// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance
EnergyMonitor emon2;                   // Create an instance

void setup()
{  
  Serial.begin(9600);
  
  emon1.current(2, 22);             // Current: input pin, calibration.
  emon2.current(3, 22);             // Current: input pin, calibration.
}

void loop()
{
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  double Irms_hp = emon2.calcIrms(1480);  // Calculate Irms only
  Serial.print(String(Irms * 230.0) + ",Power_7,");         // Apparent power
  delay(500);
  Serial.print(String(Irms_hp * 230.0) + ",Power_hp");         // Apparent power
  delay(5000);
}
