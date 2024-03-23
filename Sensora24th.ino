#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define ldr_pin A0 //Analog pin to connect ldr 
#define rel_pin D3 //Any digital pin where the relay or LED is to be connected for actuating
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "ADAFRUIT_IO_USERNAME"
#define AIO_KEY  "ADAFRUIT_IO_KEY_HERE"
WiFiClient client;

int ctr=-1;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

boolean MQTT_connect();
boolean MQTT_connect() 
{  
  int8_t ret;
  if (mqtt.connected()) 
  {    return true; 
  }  
  uint8_t retries = 3;  
  while ((ret = mqtt.connect()) != 0) 
  { 
    mqtt.disconnect(); 
    delay(2000);  
    retries--;
    if (retries == 0) 
    { 
      return false; 
    }
  } 
  return true;
}
Adafruit_MQTT_Publish light_Intensity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/NAME_OF_FEED_CREATED");
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/NAME_OF_FEED_CREATED");
void setup()
{
  Serial.begin(115200);
  pinMode(ldr_pin,INPUT);
  pinMode(rel_pin,OUTPUT);
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("YOUR_SSID_HERE","YOUR_WIFI_PASSWORD_HERE");
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("..");
  }
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP()));
}
void send_val()
{
  if (MQTT_connect()) 
    {
      int intensity=analogRead(ldr_pin);
      Serial.print("Intensity : ");Serial.println(intensity);
      int value=0;
      if(intensity>500)
      {
        value=1;
      }
      else
      {
        value=0;
      }
      if (ctr!=value)
      {
        if(light_Intensity.publish(value))
          Serial.println("data sent");
        ctr=value;
      } 
      else 
      {
        Serial.println("data not sent");
      }

    } 
    else 
    {
      Serial.println("mqtt not connected");
    }
}
void recv_val()
{
  if(MQTT_connect())
  {
    mqtt.subscribe(&light);
    Adafruit_MQTT_Subscribe *subscription;
    while (subscription = mqtt.readSubscription(2500)) 
    {
      if(subscription==&light)
      {
        Serial.println((char*)light.lastread);
        int value = atoi((char *)light.lastread);
        digitalWrite(rel_pin, value);
      }
    }
  }
}
void loop()
{
    send_val();
    recv_val();
    delay(2500);
}
