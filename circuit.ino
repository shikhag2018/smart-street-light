const int ldr = A1;
const int trigPin = 5;
const int echoPin = 6;
#define led 3
int printme;
int ldrVal = 300;
long duration;
int distance;
#include <SoftwareSerial.h> 
#define RX 10
#define TX 11
String AP = "princes";       // CHANGE ME
String PASS = "123456789"; // CHANGE ME
String API = "4PBL35S1GP0QC8YB";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
const int sensorIn = A0;
int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module


double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
 
void setup() {
  // put your setup code here, to run once:
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.begin(9600);
pinMode(3,OUTPUT);

  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  // put your main code here, to run repeatedly:
  Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; 
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS");Serial.println(String(AmpsRMS));

long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  ldrVal = analogRead(ldr);
  Serial.println(ldrVal);
  Serial.println(distance);
  if (distance < 30&&ldrVal<300)
  { 
    digitalWrite(led,HIGH);
    delay(500);
    if(led==HIGH&&AmpsRMS<0.42)
    { 
      printme=1;
    }
//   } digitalWrite(3, HIGH);
//    delay(500);
  }
  else
  {
    digitalWrite(3,LOW);
    delay(1000);
  }
  valSensor = getSensorData();
 // AmpsRMS=getCurrentsensorValue();
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor) +"&field2=" + String(AmpsRMS)+"&field3="+String(distance)+"&field4="+String(printme);
 //String getData = "GET /update?api_key="+ API +"&"+ field2 +"="+String(AmpsRMS);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
  
   
}
int getSensorData(){
  return ldrVal; // Replace with 
}
int getCurrentsensorValue()
{return AmpsRMS;
  }
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("Ok");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
               }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }

