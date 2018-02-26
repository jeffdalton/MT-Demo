/*******************Demo for MQ-2 Gas Sensor Module V1.0*****************************
Support:  Tiequan Shao: support[at]sandboxelectronics.com

Lisence: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)

Note:    This piece of source code is supposed to be used as a demostration ONLY. More
         sophisticated calibration is required for industrial field application.

                                                    Sandbox Electronics    2011-04-25
************************************************************************************/

#ifndef MQ2AirQuality_h
#define MQ2AirQuality_h

#ifdef ARDUINO
  #include "Arduino.h"
#elif defined(SPARK)
  #include "Particle.h"
#endif

/************************Hardware Related Macros************************************/
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in
                                                     //normal operation

/**********************Application Related Macros**********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)



class MQ2AirQuality 
{
    public:
        MQ2AirQuality(int sensorPin);
        void callibrate();
        float readSensorValue();
        float readLPGppm();
        float readCOppm();
        float readSMOKEppm();

    private:
        float           Ro;
        float           LPGCurve[3];
        float           COCurve[3] ;
        float           SmokeCurve[3];
        int mq_pi;
        float MQRead(int mq_pin);
        float MQResistanceCalculation(int raw_adc);
        int  MQGetPercentage(float rs_ro_ratio, float *pcurve);
        int MQGetGasPercentage(float rs_ro_ratio, int gas_id);
};
#endif
