/*
This sketch is for the Arduino Nano 33 IoT
It uses a BME280 sensor board
It uses a 0.96 inch 128 x 64 OLED display
It connects through WiFi to the Blynk cloud server for display of weather data on a smartphone
*/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>  // BME280 library
#include <avr/dtostrf.h>  // used to convert weather numbers to strings for display on OLED
// Next two includes required to make Nano 33 IoT work with Blynk
#include <WiFiNINA.h>

char auth[] = "YourBlynkProjectCode";
char ssid[] = "YourWifiID";
char pass[] = "YourWiFiPassword";

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

#define SDA_PIN 1
#define SCL_PIN 2
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 0
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void setup() {
  if (!bme.begin(0x76)) {  // wait until sensor is detected
    while (1) delay(10);
  }
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
}

void loop() {
  myupdate();
  delay(500);
}

// this routine is called by the Blynk timer to update both the OLED display and push data to Blynk
void myupdate(){
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  float ctemp = temp_event.temperature;  // get temp in centigrade
  float ftemp = 32 + (9*ctemp)/5;  // convert temp to fahrenheit
  float hum = humidity_event.relative_humidity;  // get relative humidity
  float ppress = pressure_event.pressure; // get pressure in mm
  float mpress = 1.3 + ppress/33.8939; // convert to inch Hg and add 1.3 to match actual locally reported barometric
                                       // pressure.  You may want to remove the 1.3 and/or add your own adjustment. 
  char mytemp[8]; 
  dtostrf(ftemp, 6, 2, mytemp); // convert temp to a string
  Serial.println(mytemp);
  char myhum[8]; 
  dtostrf(hum, 6, 2, myhum); // convert humidity to a string
  Serial.println(myhum);
  char mypress[8]; 
  dtostrf(mpress, 6, 2, mypress); // convert pressure to a string
  Serial.println(mypress);
}
  