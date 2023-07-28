
#include <ESP8266WiFi.h> //wifi library is needed
#include <Adafruit_ADS1X15.h> //For the ADC.
#include <Wire.h> // wire library
#include <WiFiClientSecure.h> // Secure client with SSL for https conversations
#include <ESP8266WebServer.h> // Manage conversations with the web server
#include <ESP8266HTTPClient.h> // Client that handles the requests
#include <WiFiUdp.h> // 
#include <ArduinoJson.h> // help handle JSON formatting
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>




Adafruit_ADS1115 ads;     /* I'm Using thi for the 16-bit version */
Adafruit_BME280 bme; // I2C


#define rain_digital_pin D5 //Defining D5 as rain digital
#define SEALEVELPRESSURE_HPA (1013.25)


String apiKey = "WJA5RONUNUW18ELC"; //  Enter your Write API key from ThingSpeak - Removed secrets
const char *ssid = "Professor"; // replace with your wifi ssid.
const char *pass = "Professor"; //replace with your wifi password
const char* server = "api.thingspeak.com"; // We're using the thingspeak server
const char* tidbserver = "test-env.eba-y25evqtt.us-east-1.elasticbeanstalk.com";


// ...........................Connecting to WiFi.............................................................................................................................................

WiFiClient client;

void setup()
{
  Serial.begin(115200); // The Baud rate for reading serial monitor
  //while(!Serial) { } // Wait for serial to initialize. 
  
  delay(1000);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot to prevent hack ins


  delay(500);
  Serial.println("=== Checking BME sensor ===");

  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not detect a BME280 sensor, Fix wiring Connections!");
    while (1);
  }
    Serial.println("=== BME SENSOR Passed ===");

Serial.println("=== Setting up ADS.....Preparing to Initialize ===");

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
Serial.println("=== ADS is Initialized ===");
delay(500);


Serial.println("Connecting to ");
Serial.println(ssid);


WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");


}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("");
Serial.print("Connected to ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());  //IP address assigned to your ESP
delay(500);

}

// Function to send a POST request with JSON data
void sendPostRequest(float temperature, float humidity, int adc0, int adc1, float pressure, float altitude, int rain_digital_value) {

  if (client.connect(tidbserver, 80)) {
    StaticJsonDocument<200> jsonDoc;
    
    jsonDoc["device_name"] = "Agrobot_IOT_1";
    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    jsonDoc["pressure"] = pressure;
    jsonDoc["altitude"] = altitude;
    jsonDoc["soil_moisture"] = adc0;
    jsonDoc["analog_rain_sensor"] = adc1;
    jsonDoc["rain"] = rain_digital_value;

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    client.print("POST /save HTTP/1.1\n");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonData.length());
    client.println();
    client.println(jsonData);

    Serial.println("POST request sent to the website!");
  } else {
    Serial.println("Connection to the website failed!");
  }

  client.stop();
}



void loop()
{



Serial.println("Reading DATA from ADS pins");
delay(5000);
Serial.println("Passed brief delay");
  int16_t adc0, adc1, adc2, adc3;
  Serial.println("Setup the readouts");
  adc0 = ads.readADC_SingleEnded(0); // Soil moisture
  delay(500);
  Serial.println("Read A0 successfully");
  adc1 = ads.readADC_SingleEnded(1); // Rain
  delay(500);
   Serial.println("Read A1 successfully");

  delay(1000);

//--------------------------------- 

float temperature = bme.readTemperature();
float pressure = bme.readPressure() / 100.0F ;
float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
float humidity = bme.readHumidity();

// ------------------------------------


 // Read the digital pin values
 int rain_digital_value = digitalRead(rain_digital_pin);

//.......................................... Connecting to Thingspeak.......................................................................................................................
 
if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
String postStr = apiKey; //Make Sure You write To your Own API key.
postStr += "&field1=";
postStr += String(temperature);
postStr += "&field2=";
postStr += String(humidity);
postStr += "&field3=";
postStr += String(adc0); //soil moisture
postStr += "&field4=";
postStr += String(adc1); // Rain Sensor
postStr += "&field5=";
postStr += String(pressure);
postStr += "&field6=";
postStr += String(altitude);
postStr += "&field7=";
postStr += String(rain_digital_value);
   

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

//......................................................SERIAL MONITORING.....................................................................................................................
Serial.print("Readings from the sensors: ");
Serial.println();
Serial.print("Rain Sensor ");
Serial.println(adc1);
Serial.print("Soil moisture Sensor");
Serial.println(adc0);
Serial.print("Temperature = ");
Serial.print(temperature);
Serial.println(" ºC");
Serial.print("Pressure = ");
Serial.print(pressure);
Serial.println(" hPa");
Serial.print("Approx. Altitude = ");
Serial.print(altitude);
Serial.println(" m");
Serial.print("Humidity = ");
Serial.print(humidity);
Serial.println(" %");


Serial.println();
}


delay(500);
client.stop();
Serial.println("Waiting...");
 
// thingspeak needs minimum 15 sec delay between updates.
delay(16000);

sendPostRequest(temperature, humidity, adc0, adc1, pressure, altitude, rain_digital_value);
delay(5000);

}
