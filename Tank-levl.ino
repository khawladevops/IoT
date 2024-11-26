#include <ESP8266WiFi.h>


const char *ssid = "Zero-4G";    
const char *pass = "Asdf12g34h";



#define trig 12//trigger pin of ultrasonic sensor D6
#define echo   14 //Echo pin of ultrasonic sensor D5


//////////////TANK LEVEL//////////
//Change   the values below to calibrate the meter
int MinLevel = 5; //This is the sensor   reading when the tank is empty
int MaxLevel = 7; //This is the sensor reading   when the tank is full
long dist_1 = 0;
long dist_2 = 0;
int power = 6;
int Time = 0;

void setup()
{
  Serial.begin(115200);


  pinMode(trig, OUTPUT);
  pinMode(echo,   INPUT);
  
  
  
}

void loop()
{
 Time++;
  // Standerd code for getting value from the ultrasonic   sensor
   digitalWrite(trig, LOW); 
   delayMicroseconds(2);
   digitalWrite(trig,   HIGH); 
   delayMicroseconds(10);
   digitalWrite(trig, LOW);

   
  // Reads the echoPin, returns the sound wave travel time in microseconds
  
  // Calculate the distance
  dist_1 = ( pulseIn(echo, HIGH))*0.034/2;
  dist_2 = dist_1 - MinLevel;
    int sent = map (dist_2, MinLevel, MaxLevel, 100, 0);
  }

  
  
