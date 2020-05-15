/*

  Udp NTP Client

  Get the time from a Network Time Protocol (NTP) time server
  Demonstrates use of UDP sendPacket and ReceivePacket
  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol

  created 4 Sep 2010
  by Michael Margolis
  modified 9 Apr 2012
  by Tom Igoe
  updated for the ESP8266 12 Apr 2015
  by Ivan Grokhotkov

  This code is in the public domain.

  GetNTPTime braucht ca. 1 Sec. Wenn die Zeit geholt wird, verliert
  der DCF-Algorithmus die   Gültigkeit. Damit fallen die
  angeschlossenen Uhren aus dem Sync. Das wird alle "SyncIntervall"-Sekunden
  passieren.

*/

#include <NTPClient.h>
#include <TimeLib.h>
#include <Timezone.h>     // from https://github.com/JChristensen/Timezone
#include <TimeLord.h> //https://github.com/probonopd/TimeLord
#include <ESP8266WiFi.h>
#include <WiFiudp.h>


ADC_MODE(ADC_VCC);
uint32_t EffektNow = 0;

#define HOSTNAME "ESP-Netztest"

#pragma message "ottO - Briesnitzr Hoehe 19a, Dresden"
float const LONGITUDE = 13.658055;
float const LATITUDE = 51.062369;


//WiFi
const char* ssid     = "******";
const char* password = "*******";

IPAddress ipno;


int Volt = 0;

// local time zone definition
// Central European Time (Frankfurt, Paris) from Timezone/examples/WorldClock/WorldClock.pde
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone LTZ(CEST, CET);    // this is the Timezone object that will be used to calculate local time
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
// what is our longitude (west values negative) and latitude (south values negative)
TimeLord City;


// A UDP instance to let us send and receive packets over UDP
WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "216.239.35.4", 0, 60000);
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 0, 60000);
#define SYNC_TIME 180

long int getNTPTime(void);
void printNumber(byte Number);

void printDigits(int digits);
void PrintFormattedTime(time_t TimeToPrint);
void PrintIPInfo (void);

time_t lastUpdate = 0, DisplayTime = 0, lastPrint = 0, NextSync = 0, ThisTime = 0, LastStart = 0;
time_t last_t = 0;
int LastSecond, LastMinute;

uint32_t LastReset = 0;
bool ShowOLED = false, ShowIP = false;
bool TimeShown = false;
bool SayWaitOnce = false;
byte Heute[6];
byte PreDay = 0;
bool SonneDa = true;
bool PreSonneDa = false;
bool NTPGood = false;
byte NTPMinute = 99;
char FloatString[30];
char ipno2[26] ;
int rc, ShowSwitch = 0;
long lastReconnectAttempt = 0;
long StartTime = 0;



void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Hello WLAN-World!");
  WiFiConnect();
  ipno = WiFi.localIP();
  sprintf(ipno2, "%d.%d.%d.%d", ipno[0], ipno[1], ipno[2], ipno[3]);

  timeClient.begin();
  while (!timeClient.update()) {
    Serial.print(".");
    delay(2500);
  }
  NTPGood = true;
  setTime(LTZ.toLocal(timeClient.getEpochTime()));
  NextSync = now() + SYNC_TIME;
  NTPMinute = minute(now());
  SayWaitOnce = true;

  // Sonnenauf- und -untergang bestimmen
  City.TimeZone(1 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  City.DstRules(3, 4, 10, 4, 60); //Umschaltung WZ/SZ am 4. Sonntag im ärz, zurück am 4. Sonntag im Oktober. SZ 60 min plus
  City.Position(LATITUDE, LONGITUDE); // tell TimeLord where in the world we are

  WiFi.disconnect();
  Serial.println("WLAN getrennt");

  Serial.print("Geht los mit:");
  DisplayTime = now();
  PrintFormattedTime(now());
  lastPrint = DisplayTime;

  LastReset = now();
  delay(200);
}

void loop() {
  if ((now() > NextSync) && (second(now()) == 30)) { // jede SYNC_TIME
    Serial.println("");
    Serial.println(F("*************************"));
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wieder WLAN verbinden");
      WiFiConnect();
    }

    if (timeClient.update()) {
      setTime(LTZ.toLocal(timeClient.getEpochTime(), &tcr));
      Serial.println("WLAN trennen");
      WiFi.disconnect();
      Serial.print(F("*** NTP erfolgreich ****"));
      NextSync = now() + SYNC_TIME;
      NTPGood = true;
    }
    else {
      Serial.print(F("-- NTP nicht erfolgreich --"));
      NextSync = now() + 45;
      NTPGood = false;
    }
    PrintFormattedTime(now());
    Serial.println(F("*************************"));
  }

  if (LastMinute != minute(now())) {
    PrintFormattedTime(now());
    SunUpDown();
    DisplayTime = now();
    LastMinute = minute(now());
  }

  if (LastSecond != second(now())) {
    LastSecond = second(now());
    //Serial.print(second(now()));
    if (second(now()) == 1) {
      Serial.print("\nx.");
    }
    else if ((second(now()) > 1) && (second(now()) < 30))
      Serial.print(".");

    else if (second(now()) == 30) {
      Serial.print("\n:");
      //Serial.println("");
      //Serial.print(":");
    }
    else if ((second(now()) > 30) && (second(now()) <= 59))
      Serial.print(":");
    else
      Serial.print("");
  }
}
