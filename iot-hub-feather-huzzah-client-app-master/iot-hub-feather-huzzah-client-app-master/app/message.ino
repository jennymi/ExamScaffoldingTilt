#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>  // Wire library - used for I2C communication
#include <time.h>
#include <string.h>
#include <stdio.h>


// ADXL345 I2C address is 0x53(83)
#define Addr 0x53
float xAccl, yAccl, zAccl;
float roll,pitch,rollF,pitchF=0;

#if SIMULATED_DATA
#endif

void readMessage(char *payload)
{
    unsigned int data[6];
  
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select bandwidth rate register
    Wire.write(0x2C);
    // Normal mode, Output data rate = 100 Hz
    Wire.write(0x0A);
    // Stop I2C transmission
    Wire.endTransmission();
  
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select power control register
    Wire.write(0x2D);
    // Auto-sleep disable
    Wire.write(0x08);
    // Stop I2C transmission
    Wire.endTransmission();
  
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data format register
    Wire.write(0x31);
    // Self test disabled, 4-wire interface, Full resolution, Range = +/-2g
    Wire.write(0x08);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(300);
  
    for (int i = 0; i < 6; i++)
    {
      // Start I2C Transmission
      Wire.beginTransmission(Addr);
      // Select data register
      Wire.write((50 + i));
      // Stop I2C transmission
      Wire.endTransmission();
  
      // Request 1 byte of data
      Wire.requestFrom(Addr, 1);
  
      // Read 6 bytes of data
      // xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
      if (Wire.available() == 1)
      {
        data[i] = Wire.read();
      }
    }
    
    // Convert the data to 10-bits
    int xAccl = (((data[1] & 0x03) * 256) + data[0]);
    if (xAccl > 511)
    {
      xAccl -= 1024;
    }
    int yAccl = (((data[3] & 0x03) * 256) + data[2]);
    if (yAccl > 511)
    {
      yAccl -= 1024;
    }
    int zAccl = (((data[5] & 0x03) * 256) + data[4]);
    if (zAccl > 511)
    {
      zAccl -= 1024;
    }

    roll = atan(yAccl / sqrt(pow(xAccl, 2) + pow(zAccl, 2))) * 180 / PI;
    pitch = atan(-1 * xAccl / sqrt(pow(yAccl, 2) + pow(zAccl, 2))) * 180 / PI;
    // Low-pass filter
    rollF = 0.94 * rollF + 0.06 * roll;
    pitchF = 0.94 * pitchF + 0.06 * pitch;
    Serial.print(rollF);
    Serial.print("/");
    Serial.println(pitchF);

    //Get currentTime
    time_t dateTime = time(NULL);
    char * time_str = ctime(&dateTime);
    time_str[strlen(time_str)-1] = '\0';
  
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
   root["dateTime"] = time_str;

    // NAN is not the valid json, change it to NULL
    if (std::isnan(rollF))
    {
        root["rollF"] = NULL;
    }
    else
    {
        root["rollF"] = rollF;
    }

    if (std::isnan(pitchF))
    {
        root["pitchF"] = NULL;
    }
    else
    {
        root["pitchF"] = pitchF;
    }

    root.printTo(payload, MESSAGE_MAX_LEN);
}

void parseTwinMessage(char *message)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(message);
    if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }

    if (root["desired"]["interval"].success())
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
