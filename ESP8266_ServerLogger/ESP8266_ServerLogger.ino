/*

*/
#include <ESP8266WiFi.h>
#include <PubSubClientShashank.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
// Update these with values suitable for your network.

SoftwareSerial OVCD(4, 0);  //rx,tx
#define Ledd D5
// Define NTP settings
const char* ntpServer = "pool.ntp.org";
const long timeZoneOffset = 19800;  // Time zone offset for India (in seconds, +5:30)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, timeZoneOffset);

const char* ssid = "Fervid Smart";
const char* password = "64646464";
const char* mqtt_server = "broker.hivemq.com";
const char* topic = "FSS2022500033J";
const char* projectId = "21515642890";
const char* deviceId = "1001";
const char* PubTopic = "sensor/input/21515642890";
const char* clientId = "F21515642890T";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
boolean powerOnindicator = true;

String DataJson = "";
String OVCDData = "";
const int BUFFER_SIZE = 64;

const int Buzzer = 13;

float Vcc = 0, peak = 0, Vrms = 0, Irms = 0, PreIrms = 0, PreVrms = 0;

char Realystatus = '0', PreRealystatus = '0';

char Opstatus = '0', PreOpstatus = '0';

uint8_t OCcount = 0;

uint32_t PubTime = 0;
unsigned long SendTime = 0;
const int watchdogTimeout = 10;
/*-2 means Offline
    1  means Reseted
    -1 means Network reconnect
  */

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected

  DataJson = "";

  DataJson = DataJson + "{\"projectId\" : \"" + projectId + "\",\"deviceId\" : \"" + deviceId + "\",\"St\" : \"Offline\"}";
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    
    // Attempt to connect
    ESP.wdtFeed();
    if (client.connect(clientId, PubTopic, 1, 0, DataJson.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      ESP.wdtFeed();
    } else {
      ESP.wdtFeed();
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  if (powerOnindicator) {

    powerOnindicator = false;
    
    String reason = ESP.getResetReason();

    Serial.println(DataJson);
    DataJson.clear();
    DataJson = DataJson + "{\"projectId\" : \"" + projectId + "\",\"deviceId\" : \"" + deviceId + "\",\"St\" : \"";
    DataJson = DataJson + reason +"\"}";
    Serial.println(DataJson);
  } else {

    
    DataJson.clear();
    DataJson = DataJson + "{\"projectId\" : \"" + projectId + "\",\"deviceId\" : \"" + deviceId + "\",\"St\" : \"Network reconnect\"}";
    Serial.println(DataJson);
  }

  if (!client.publish(PubTopic, DataJson.c_str())) {

    if (!client.publish(PubTopic, DataJson.c_str())) {

      if (!client.publish(PubTopic, DataJson.c_str())) {

        Serial.print("not publish");
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  OVCD.begin(9600);
  Serial.begin(9600);
  // pinMode(Ledd, OUTPUT); 
  // //digitalWrite(Ledd, HIGH); 
  setup_wifi();
  timeClient.begin();
  client.setServer(mqtt_server, 1883);
    // Initialize the watchdog timer
  ESP.wdtDisable(); // Disable the watchdog timer initially
  ESP.wdtEnable(WDTO_8S); // Watchdog timeout set to 8 seconds\

  // client.setCallback(callback);
}

void loop() {

  // put your main code here, to run repeatedly:
  ESP.wdtFeed();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  

  uint8_t index = 0, index2 = 0;

  char CharTemp;

  while (OVCD.available() > 0) {
    ESP.wdtFeed();
    // delayMicroseconds(50);
    // CharTemp = OVCD.read();
    OVCDData.concat((char)OVCD.read());
    // Serial.println(OVCDData);
    if (OVCDData.indexOf("\n") > -1) {
      // UARTData = UARTData + '\0';
      
      Serial.print("UART = ");
      Serial.println(OVCDData);
      OVCD.flush();
      if ((OVCDData.indexOf("S") > -1) && (OVCDData.indexOf("E") > -1)) {
        Serial.println("Verfied");  //S,5.00,N,N,0,2.15,222.69,0.00,E

        // Serial.flush();

        // Serial.print(UARTData);

        //Vcc

        index = OVCDData.indexOf(",");

        index2 = OVCDData.indexOf(",", index + 1);

        Vcc = atof(OVCDData.substring(index + 1, index2).c_str());

        // Serial.println(UARTData.substring(index + 1, index2));

        //Realy

        index = index2;

        index2 = OVCDData.indexOf(",", index + 1);

        Realystatus = (OVCDData.substring(index + 1, index2))[0];

        // Serial.println(UARTData.substring(index + 1, index2)[0]);

        //OpStatus

        index = index2;

        index2 = OVCDData.indexOf(",", index + 1);

        Opstatus = (OVCDData.substring(index + 1, index2))[0];

        // Serial.println(UARTData.substring(index + 1, index2)[0]);

        //OCcount

        index = index2;

        index2 = OVCDData.indexOf(",", index + 1);

        OCcount = atoi(OVCDData.substring(index + 1, index2).c_str());

        // Serial.println(UARTData.substring(index + 1, index2));

        //peak

        index = index2;

        index2 = OVCDData.indexOf(",", index + 1);
        peak = atof(OVCDData.substring(index + 1, index2).c_str());

        // Serial.println(UARTData.substring(index + 1, index2));
        //vrms
        index = index2;
        index2 = OVCDData.indexOf(",", index + 1);
        Vrms = atof(OVCDData.substring(index + 1, index2).c_str());
        // Serial.println(UARTData.substring(index + 1, index2));

        //Irms
        index = index2;
        index2 = OVCDData.indexOf(",", index + 1);
        Irms = atof(OVCDData.substring(index + 1, index2).c_str());
        // Serial.println(UARTData.substring(index + 1, index2));

        DataJson.clear();
        if (((PreVrms - 30) > Vrms) || ((PreVrms + 30) < Vrms) || ((PreIrms - 5) > Irms) || ((PreIrms + 5) < Irms) || (Realystatus != PreRealystatus) || (Opstatus != PreOpstatus) || ((millis() - SendTime) > 300000)) {
          SendTime = millis();
          PreVrms = Vrms;
          PreIrms = Irms;
          PreRealystatus = Realystatus;
          PreOpstatus = Opstatus;

          timeClient.update();
          time_t epochTime = timeClient.getEpochTime();

          unsigned short seconds = timeClient.getSeconds();
          unsigned short minutes = timeClient.getMinutes();
          unsigned short hours = timeClient.getHours();

          // Get current date
          time_t currentTime = now();  // Use the now() function from TimeLib.h
          struct tm* currentDate = gmtime ((time_t *)&epochTime); 
          unsigned short DayS = currentDate->tm_mday;       // Use the day() function from TimeLib.h
          unsigned short MonthS = currentDate->tm_mon + 1;  // Use the month() function from TimeLib.h
          unsigned short YearS = (currentDate->tm_year + 1900) % 100;


          // Format the date and time with leading zeros
          char formattedDateTime[20];
          sprintf(formattedDateTime, "%02hu-%02hu-%02hu %02hu:%02hu:%02hu",
                  DayS, MonthS, YearS, hours, minutes, seconds);



          DataJson = DataJson + "{\"projectId\" : \"" + projectId + "\",\"deviceId\" : \"" + deviceId;

          DataJson = DataJson + "\",\"Irms\" : \"" + Irms + "\",\"Vrms\" : \"" + Vrms + "\",\"Peak\" : \"" + peak + "\",\"TimeStamp\" : \"";


          DataJson = DataJson + formattedDateTime + "\",\"Vcc\" : \"" + Vcc + "\",\"OCcount\" : \"" + OCcount + "\",\"OPStarus\" : \"" + Opstatus + "\",\"Relay\" : \"" + Realystatus + "\",\"St\" : \"LIVE\"}";
          Serial.println(DataJson);
          // Serial.flush();

          if (!client.publish(PubTopic, DataJson.c_str())) {
            if (!client.publish(PubTopic, DataJson.c_str())) {
              if (!client.publish(PubTopic, DataJson.c_str())) {
                Serial.print("not publish");
                //digitalWrite(Ledd, LOW); 
              }
              else
          {
             //digitalWrite(Ledd, HIGH);
             Serial.print(" publish"); 
          }
            }
            else
          {
             //digitalWrite(Ledd, HIGH); 
             Serial.print(" publish"); 
          }
          }
          else
          {
             //digitalWrite(Ledd, HIGH); 
             Serial.print(" publish"); 
          }
        }
      }


      OVCDData.clear();
    }

    // }
  }
}
