#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

const char *ssid = "Zero-4G";    
const char *pass = "Asdf12g34h";

const char* mqtt_server = "54.250.207.187";
const char* mqttUser = "kh";
const char* mqttPassword = "12345678";

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
#define SENSOR  2 //D4
#define trig  12//trigger pin of ultrasonic sensor D6
#define echo   14 //Echo pin of ultrasonic sensor D5
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;
//////////////TANK LEVEL//////////
//Change   the values below to calibrate the meter
int MinLevel = 5; //This is the sensor   reading when the tank is empty
int MaxLevel = 7; //This is the sensor reading   when the tank is full
long dist_1 = 0;
long dist_2 = 0;


/////////////////////////////////
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

WiFiClient espClient;
PubSubClient client(espClient); 

void setup()
{
  Serial.begin(115200);

  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(trig, OUTPUT);
  pinMode(echo,   INPUT);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  
  WiFi.begin (ssid,pass);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
  }
  Serial.println ("Connected to the WiFi network");
  client.setServer(mqtt_server, 1883);
  while (!client.connected()){
    Serial.println("Connecting to MQTT ...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword ))
    {
      Serial.println("Connected");
    } else {
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(1000);
      }
    }
}

void loop()
{
  
  client.loop();
  
  currentMillis = millis();
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  
  if (currentMillis - previousMillis > interval) 
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
 
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
   
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;
    // print message
    lcd.print("Water Flow= ");
    lcd.setCursor(12, 0);
    lcd.print(flowRate);
    client.publish("flow", String(flowRate).c_str());
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    
    // Print the cumulative total of litres flowed since starting
    
    client.publish("flowTotal", String(totalLitres).c_str());
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalLitres);
    Serial.println("L");
  }
  ////////////TANK LEVEL//////////////////////
  
  // Standerd code for getting value from the ultrasonic   sensor
   digitalWrite(trig, LOW); 
   delayMicroseconds(5);
   digitalWrite(trig,   HIGH); 
   delayMicroseconds(10);
   digitalWrite(trig, LOW);
   // Reads the echoPin, returns the sound wave travel time in microseconds
   // Calculate the distance
    dist_1 = ( pulseIn(echo, HIGH))*0.034/2;
    int sent = map (dist_1, MinLevel, MaxLevel, 100, 0);
    lcd.setCursor(0,1);
    lcd.print("TanksLevel=");
    lcd.setCursor(12, 0);
    lcd.print(sent);
    client.publish("tank", String(sent).c_str());
    delay(1000);
    lcd.clear();
  }

 
