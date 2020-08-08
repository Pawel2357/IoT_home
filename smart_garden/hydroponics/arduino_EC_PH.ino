    /***************************************************
     DFRobot Gravity: Analog Spear Tip pH Sensor / Meter Kit (For Soil And Food Applications)
     <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_Spear_Tip_pH_Sensor_/_Meter_Kit_(For_Soil_And_Food_Applications)_SKU:_SEN0249>

     ***************************************************
     This product is used to measure the pH value of the semisolid,such as meat,fruit,moist soil and so on.
     This sample code reads the pH value.

     Created 2017-9-10
     By Jason <jason.ling@dfrobot.com@dfrobot.com>

     GNU Lesser General Public License.
     See <http://www.gnu.org/licenses/> for details.
     All above must be included in any redistribution
     ****************************************************/

     /***********Notice and Trouble shooting***************
     1. This code is tested on Arduino Uno and Leonardo with Arduino IDE 1.0.5 r2 and 1.8.2.
     2. More details, please click this link: <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_Spear_Tip_pH_Sensor_/_Meter_Kit_(For_Soil_And_Food_Applications)_SKU:_SEN0249>
     ****************************************************/

    //*************EC meter variables ****************************************************************************************//
    #define PHSensorPin  A2    //dissolved oxygen sensor analog output pin to arduino mainboard
    #define VREF 5.0    //for arduino uno, the ADC reference is the AVCC, that is 5.0V(TYP)
    #define OFFSET 0.00  //zero drift compensation

    int R1= 1000;
    int Ra=25; //Resistance of powering Pins
    int ECPin= A0;
    int ECGround=A1;
    int ECPower =A4;
  
    float PPMconversion=0.7;
   
    //*************Compensating for temperature ************************************//
    //The value below will change depending on what chemical solution we are measuring
    //0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
    float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
    
    //********************** Cell Constant For Ec Measurements *********************//
    //Mine was around 2.9 with plugs being a standard size they should all be around the same
    //But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
    float K=2.88;

    float Temperature=10;
    float EC=0;
    float EC25 =0;
    int ppm =0;
    float raw= 0;
    float Vin= 5;
    float Vdrop= 0;
    float Rc= 0;
    float buffer=0;

    //*************PH meter variables ****************************************************************************************//
    #define SCOUNT  30           // sum of sample point
    int analogBuffer[SCOUNT];    //store the analog value in the array, readed from ADC
    int analogBufferTemp[SCOUNT];
    int analogBufferIndex = 0,copyIndex = 0;

    float averageVoltage,phValue;

    void setup()
    {
        Serial.begin(9600);
        pinMode(PHSensorPin,INPUT);
        
        pinMode(ECPin,INPUT);
        pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
        pinMode(ECGround,OUTPUT);//setting pin for sinking current
        digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
        R1=(R1+Ra);// Taking into acount Powering Pin Resitance
    }

    void loop()
    {
       static unsigned long analogSampleTimepoint = millis();
       if(millis()-analogSampleTimepoint > 30U)     //every 30 milliseconds,read the analog value from the ADC
       {
         analogSampleTimepoint = millis();
         analogBuffer[analogBufferIndex] = analogRead(PHSensorPin);    //read the analog value and store into the buffer
         //Serial.print(analogBuffer[analogBufferIndex]);
         analogBufferIndex++;
         if(analogBufferIndex == SCOUNT)
             analogBufferIndex = 0;
       }
       static unsigned long printTimepoint = millis();
       if(millis()-printTimepoint > 6000U)
       {
          printTimepoint = millis();
          for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
          {
            analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
          }
          averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the value more stable by the median filtering algorithm
          phValue = 3.5 * averageVoltage+OFFSET;

          GetEC(); 
          PrintReadings();
       }
    }

    int getMedianNum(int bArray[], int iFilterLen)
    {
          int bTab[iFilterLen];
          for (byte i = 0; i<iFilterLen; i++)
          {
          bTab[i] = bArray[i];
          }
          int i, j, bTemp;
          for (j = 0; j < iFilterLen - 1; j++)
          {
          for (i = 0; i < iFilterLen - j - 1; i++)
              {
            if (bTab[i] > bTab[i+1])
                {
            bTemp = bTab[i];
                bTab[i] = bTab[i+1];
            bTab[i+1] = bTemp;
             }
          }
          }
          if ((iFilterLen & 1) > 0)
        bTemp = bTab[(iFilterLen - 1) / 2];
          else
        bTemp = (bTab[iFilterLen / 2]+bTab[iFilterLen / 2 - 1]) / 2;
          return bTemp;
    }

    //************ This Loop Is called From Main Loop************************//
    void GetEC(){
        //*********Reading Temperature Of Solution *******************//
        // sensors.requestTemperatures();// Send the command to get temperatures
        // Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable

        //************Estimates Resistance of Liquid ****************//
        digitalWrite(ECPower,HIGH);
        raw= analogRead(ECPin);
        raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
        digitalWrite(ECPower,LOW);
       
       
       
        //***************** Converts to EC **************************//
        Vdrop= (Vin*raw)/1024.0;
        Rc=(Vdrop*R1)/(Vin-Vdrop);
        Rc=Rc-Ra; //acounting for Digital Pin Resitance
        EC = 1000/(Rc*K);
         
         
        //*************Compensating For Temperaure********************//
        EC25  =  EC; // / (1+ TemperatureCoef*(Temperature-25.0));
        ppm=(EC25)*(PPMconversion*1000);
    };
    //************************** End OF EC Function ***************************//


    //***This Loop Is called From Main Loop- Prints to serial usefull info ***//
    void PrintReadings(){
        Serial.print(String(ppm) + ",ppm,");        
        delay(500);
        Serial.print(String(phValue) + ",ph");         
         
        /*
        //********** Usued for Debugging ************
        Serial.print("Vdrop: ");
        Serial.println(Vdrop);
        Serial.print("Rc: ");
        Serial.println(Rc);
        Serial.print(EC);
        Serial.println("Siemens");
        //********** end of Debugging Prints *********
        */
    };
