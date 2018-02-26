/*
 * Project MT-Demo
 * Description:
 * Author:

 * Date:
 */
 // DHT-22 Temp Sensor
 #include <Adafruit_DHT.h>
 #define DHTTYPE DHT22
 #define DHT_SENSOR_PIN D2
 DHT dht(DHT_SENSOR_PIN, DHTTYPE);

 // SW18020P VIBRATION Sensor
 #define VIBRATION_SENSOR_PIN A0


 // GROVE Loundess Sensor
 #define LOUDNESS_SENSOR_PIN A2


 // Alarm
 #define ALARM_PIN D5

 // IR Sensor
 #include "SharpIR.h"
 #define IR_SENSOR_PIN A3
 #define IR_SENSOR_MODEL 20150
 #include <SharpIR.h>
 SharpIR sharpIR(IR_SENSOR_PIN, IR_SENSOR_MODEL);




 // MQ-2 Air Quality Sensor
 #include "MQ2AirQuality.h"
 #define         MQ_PIN                       (1)     //define which analog input channel you are going to use
 MQ2AirQuality mq2AirQuality(MQ_PIN);



 #define ISALIVELED D6
 #define CONTROLLED D7

 #define DELAY 100
 #define ALARM_DELAY 300



 unsigned long start_time, last_time, last_temp_read_time, last_airquality_read_time, last_distance_read_time, last_sound_read_time, last_publish_time = 0;


 float last_temp_read, last_airquality_read = 0;
 int last_distance_read, last_sound_read = 0;

 struct sensorReadingsType {
     int vibration;
     float temperature;
     int distance;
     int sound;
     float airQuality;
 };

 sensorReadingsType collectSensorData() {
     sensorReadingsType result;

     result.vibration = readVibration();
     result.temperature = readTempAndHumidity();
     result.distance = readDistance();
     result.sound = readSound();
     result.airQuality = readAirQuality();

     return result;
 }

 void analyzeSensorData(sensorReadingsType data) {
     if (data.vibration < 1022) {
         Serial.println("Abnormal vibration detected.");
         Particle.publish("Vibration Alarm");
         soundAlarm();
     }

     if (data.temperature <= 0 || data.temperature >= 33) {
         Serial.println("Abnormal temperature detected.");
         Particle.publish("Temperature Alarm");
         soundAlarm();
     }

     if (data.distance <= 5) {
         Serial.println("Abnormal distance detected.");
         Particle.publish("Distance Alarm");
         soundAlarm();
     }


 }


 void publishDataToCloud(sensorReadingsType data) {
     if (millis() - last_publish_time < 60000){
         return;
     }
     last_publish_time = millis();
     Particle.publish("temperature", String(data.temperature), PRIVATE);
     Particle.publish("vibration", String(data.vibration), PRIVATE);
 }


 void setup() {
     Serial.begin(9600);
     start_time, last_time = millis();
     last_temp_read_time, last_airquality_read_time = -5000;


     Serial.printlnf("Startup time: %lu", start_time);
     Particle.publish("Starting MT-IOT-Pres");

     pinMode(ISALIVELED, OUTPUT);
     pinMode(CONTROLLED, OUTPUT);
     pinMode(ALARM_PIN, OUTPUT);

     dht.begin();
     mq2AirQuality.callibrate();

 }

 void loop() {
     cycleIsAliveLED();


     controlLEDOn();
     sensorReadingsType data = collectSensorData();
     analyzeSensorData(data);
     publishDataToCloud(data);
     controlLEDOff();

     delay(DELAY);
 }




 float readAirQuality() {
     if (millis() - last_airquality_read_time < 1000){
         return last_airquality_read;
     }
     last_airquality_read_time = millis();

     float val = mq2AirQuality.readSensorValue();
     last_airquality_read = val;

     //Serial.printlnf("Air Quality %f ", val);

     return val;
 }


 int readSound() {
     if (millis() - last_sound_read_time < 500){
         return last_sound_read;
     }
     last_sound_read_time = millis();

     int val = analogRead(LOUDNESS_SENSOR_PIN);
     last_sound_read = val;

     //Serial.printlnf("Sound %i ", val);

     return val;
 }


 float readTempAndHumidity() {

     if (millis() - last_temp_read_time < 5000){
         return last_temp_read;
     }
     last_temp_read_time = millis();


     float temperatureC = dht.getTempCelcius();
     float humidity = dht.getHumidity();
     float temperatureF = dht.getTempFarenheit();

     if (isnan(temperatureC) || isnan(humidity) || isnan(temperatureF)) {
         Serial.println("Failed to read DHT sensor!");
         return -100;
     }


     float heatIndex = dht.getHeatIndex();
     float dewPoint = dht.getDewPoint();
     float temperatureK = dht.getTempKelvin();


     last_temp_read = temperatureC;

     //Serial.printlnf("Temperature %f C", temperatureC);

     return temperatureC;
 }

 float readVibration() {
     float result = analogRead(VIBRATION_SENSOR_PIN);

     //Serial.printlnf("Vibration %f", result);

     return result;
 }

 int readDistance() {
     if (millis() - last_distance_read_time < 1000){
         return last_distance_read;
     }
     last_distance_read_time = millis();

     int distance = sharpIR.distance();
     last_distance_read = distance;

     //Serial.printlnf("Distance %i", distance);
     return distance;
 }


 void soundAlarm() {
     digitalWrite(ALARM_PIN, HIGH);
     delay(ALARM_DELAY);
     digitalWrite(ALARM_PIN, LOW);
 }

 void controlLEDOn() {
     //Serial.println("Control LED ON");
     digitalWrite(CONTROLLED, HIGH);
 }

 void controlLEDOff() {
     //Serial.println("Control LED OFF");
     digitalWrite(CONTROLLED, LOW);
 }

 void cycleIsAliveLED() {
     digitalWrite(ISALIVELED, HIGH);
     delay(DELAY);
     digitalWrite(ISALIVELED, LOW);
     delay(DELAY);

     if (millis() - last_time > 300000){
         unsigned long aliveTimeInMin = (((millis() - start_time) / 1000) / 60);

         Serial.printlnf("IsAlive for %lu minutes.", aliveTimeInMin);
         Particle.publish("MT-IOT-Pres IsAlive for minutes", aliveTimeInMin);
         last_time = millis();
     }

 }
