/****************************************************************************************************************************
  WiFiWebServer.ino

  A simple web server that shows the value of the analog input pins.

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:
   Analog inputs attached to pins A0 through A5 (optional)

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe

  Based on and modified from WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  to support nRF52, SAMD21/SAMD51, STM32F/L/H/G/WB/MP1, Teensy, etc. boards besides Nano-33 IoT, MKRWIFI1010, MKRVIDOR400, etc.

  Built by Khoi Hoang https://github.com/khoih-prog/WiFiNINA_Generic
  Licensed under MIT license

  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************************************************************/

#include "defines.h"
#include "arduino_secrets.h"

// To eliminate FW warning when using not latest nina-fw version
// To use whenever WiFi101-FirmwareUpdater-Plugin is not sync'ed with nina-fw version
#define WIFI_FIRMWARE_LATEST_VERSION        "1.4.8"

#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <Arduino_LSM6DS3.h>

// for the sensor library
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define MOTOR_PIN 12

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP), length must be 8+

int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
float ax, ay, az, gx, gy, gz;
float celsius, fahrenheit;
float motor_speed = 150;
int type = -1;
int autoMode = -1;

WiFiServer server(80);

// sensor setup
#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long delayTime;
Adafruit_BME280 bme; // I2C

void setup()
{
  //Initialize serial and wait for port to open:
  pinMode(12, OUTPUT);
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  unsigned status;
    
  // default settings
  status = bme.begin(0x76);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print(F("\nStart WiFiWebServer on ")); Serial.println(BOARD_NAME);
  //Serial.println(WIFININA_GENERIC_VERSION);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("Communication with WiFi module failed!"));
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.print(F("Your current firmware NINA FW v"));
    Serial.println(fv);
    Serial.print(F("Please upgrade the firmware to NINA FW v"));
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    Serial.print(F("Attempting to connect to SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    //delay(10000);
  }

  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
  }
  else {
    Serial.println("---------SUCCESS-----------");
  }
  
  Serial.println("-- Default Test --");
  delayTime = 1000;

  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();
}

//--------------------------------MAIN LOOP------------------------------
void loop()
{
  // listen for incoming clients
  int header_length = 0;
  char firstLine[500];
  WiFiClient client = server.available();
  if (client)
  {
    header_length = 0;
    //Serial.println(F("New client"));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        firstLine[header_length] = c;
        header_length ++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
          IMU.readAcceleration(ax, ay, az);
          IMU.readGyroscope(gx, gy, gz);
        }
        if (c == '\n' && currentLineIsBlank) 
        {
          break;
        }
        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    type = getPackageType(firstLine);
    if (type == 0) {
      response(client);
      //analogWrite(MOTOR_PIN, 140);  // default rpm
    }
    else if (type == 6) {
      Serial.println("Auto mode");
      autoMode = 1;
      motor_speed = celsius * 5.2;
      analogWrite(MOTOR_PIN, motor_speed);  // default rpm
    }
    else {
      autoMode = 0;
      Serial.println(type);
      if (type == 1) {
        motor_speed = 100;
        analogWrite(MOTOR_PIN, motor_speed); 
      }
      else if (type == 2) {
        motor_speed = 130;
        analogWrite(MOTOR_PIN, motor_speed);
      }
         
      else if (type == 3) {
        motor_speed = 170;
        analogWrite(MOTOR_PIN, motor_speed);
      }
      else if (type == 4) {
        motor_speed = 210;
        analogWrite(MOTOR_PIN, motor_speed); 
      }
      else {
        motor_speed = 250;
        analogWrite(MOTOR_PIN, motor_speed); 
      } 
    }
    client.stop();
    //Serial.println(F("Client disconnected"));
  }
}

void printWiFiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}

void response(WiFiClient client) {
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Refresh: 1");  // refresh the page automatically every 5 sec
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  
  // output the value of each analog input pin
  // for (int analogChannel = 0; analogChannel < 6; analogChannel++) 
  // {
  //   int sensorReading = analogRead(analogChannel);
  //   client.print("analog input ");
  //   client.print(analogChannel);
  //   client.print(" is ");
  //   client.print(sensorReading);
  //   client.println("<br />");
  // }

  // obtain sensor value
  celsius = bme.readTemperature();
  fahrenheit = convert_c_to_f_temp(celsius); 

  if (autoMode)
    motor_speed = celsius * 5.2;

  client.println(fahrenheit);
  client.println(celsius);
  client.println(motor_speed);
  
  client.println("</html>");
}

int getPackageType(char content[500]) {
  if (content[0] == 'G')
    return 0;
  else if (content[0] == 'P')
    return content[1] - '0';
  return -1;
}

float convert_c_to_f_temp(float c) {
  return c*1.8 + 32;
}
