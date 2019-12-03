#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>  // Wire library - used for I2C communication

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs

#if SIMULATED_DATA
#endif

void readMessage(int messageId, char *payload)
{
      // === Read acceleromter data === //
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
    X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
    X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
    Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
    Y_out = Y_out/256;
    Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
    Z_out = Z_out/256;
    //Serial.print("Xa= ");
    //Serial.print(X_out);
    //Serial.print("   Ya= ");
    //Serial.print(Y_out);
    //Serial.print("   Za= ");
    //Serial.println(Z_out);
  
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;

    // NAN is not the valid json, change it to NULL
    if (std::isnan(X_out))
    {
        root["X_out"] = NULL;
    }
    else
    {
        root["X_out"] = X_out;
    }

    if (std::isnan(Y_out))
    {
        root["Y_out"] = NULL;
    }
    else
    {
        root["Y_out"] = Y_out;
    }

    if (std::isnan(Z_out))
    {
        root["Z_out"] = NULL;
    }
    else
    {
        root["Z_out"] = Z_out;
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
