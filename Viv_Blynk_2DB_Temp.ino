#define BLYNK_PRINT Serial  // Comment this out to disable prints and save space
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <SPI.h>
//#include <DHT.h>    		
#define ONE_WIRE_BUS 2		//Data wire plugged to pin GPIO2 (DS18B20 sensor)
//#define DHTPIN 12 			//Data wire plugged to pin GPIO12(DHT22 sensor)
//#define DHTTYPE DHT22   	//DHT 22 Change this if you have a DHT11
//DHT dht(DHTPIN, DHTTYPE);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char auth[] = "1a133371bf9a44bc8be9e053babd5248"; //Put your Auth Token in the Blynk App here

SimpleTimer timer;

OneWire  ds(2);  // on pin 7 (a 4.7K resistor is necessary) make sure you change this from the original pin 10 to an unused pin.
int adr;
float s1;
float s2;

void setup()
{
 Serial.begin(9600);
 Blynk.begin(auth, "ParentNet", "florence33"); //insert here your SSID and password
 sensors.begin();
 sensors.setResolution(10); 	//More on resolution: http://www.homautomation.org/2015/11/17/ds18b20-how-to-change-resolution-9101112-bits/
// timer.setInterval(3000, readTemp);   //Data reading interval
}

void loop()
{
  Blynk.run(); // All the Blynk Magic happens here...

  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
    byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
//  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {           //we need to drop 8 bytes of data
  }
  adr = (addr[7]);

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes to drop off
    data[i] = ds.read();
  }
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  if(adr == 101)  {         //replace ??? with value of sensor number 1    
    s1 = celsius;           //change celsius to fahrenheit if you prefer output in Fahrenheit
  }

  if(adr == 185)  {         //replace ??? with value of sensor number 2
    s2 = celsius;           //change celsius to fahrenheit if you prefer output in Fahrenheit
  }


Serial.print(" Sensor 1 = ");
Serial.print(s1);
Serial.print(" Sensor 2 = ");
Serial.print(s2);
//add or delete if you need more or less

  Blynk.virtualWrite(V5, s1);
  Blynk.virtualWrite(V6, s2);
    //add or delete if you need more or less
}
