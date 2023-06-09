#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "creds.h"
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "GxEPD2_display_selection_new_style.h"

U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

String dateTime;
int dayOfWeek;
int currentHour;
int currentMinute;
int minutesTillTomorrow;
int minutesTillMonday;
int currentPeriod;
DynamicJsonDocument doc(2048);

// config variables
String IP = API_BASE + "/api/getRoomInfo.php?id=";
IPAddress gateway(10, 0, 0, 1);
IPAddress dns(10, 0, 0, 1);
IPAddress subnet(255, 0, 0, 0);
IPAddress ip(10, 0, 144, 69);


boolean loadTimeTable() {
  boolean returnVal;
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin(IP + roomID);
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      deserializeJson(doc, payload);
      http.end();
      return true;
    }
    http.end();
  }
  return false;
}

void drawM(int x, int y, bool invert ) {
  int fcolor= invert?GxEPD_WHITE:GxEPD_BLACK;
  int bcolor= (!invert)?GxEPD_WHITE:GxEPD_BLACK;   
  
  display.fillRect(0, y-8, 12, 25, bcolor);
    
  display.drawLine(x, y, x, y + 7, fcolor);
  display.drawLine(x + 1, y + 1 , x + 3, y +4, fcolor);
  display.drawLine(x + 3, y +4, x + 5, y +1, fcolor);
  display.drawLine(x + 6, y+7, x +6, y, fcolor); 

}

void drawD(int x, int y, bool invert) {
int fcolor= invert?GxEPD_WHITE:GxEPD_BLACK;
int bcolor= (!invert)?GxEPD_WHITE:GxEPD_BLACK;   

display.fillRect(0, y-8, 12, 25, bcolor);

  display.drawLine(x, y, x, y + 7, fcolor);
  
  display.drawPixel(x + 1, y + 7, fcolor);
  display.drawPixel(x + 2, y + 7, fcolor);

  display.drawPixel(x + 3, y + 6, fcolor);

  display.drawLine(x + 4, y + 5, x + 4, y + 2, fcolor);

  display.drawPixel(x + 3, y + 1, fcolor);

  display.drawPixel(x + 1, y, fcolor);
  display.drawPixel(x + 2, y, fcolor);
 
}

void drawF(int x, int y, bool invert) {
  
int fcolor= invert?GxEPD_WHITE:GxEPD_BLACK;
int bcolor= (!invert)?GxEPD_WHITE:GxEPD_BLACK;  


  display.fillRect(0, y-8, 12, 26, bcolor);

  display.drawLine(x, y, x, y + 7, fcolor);

  display.drawLine(x + 1, y, x + 4, y, fcolor);

  display.drawPixel(x + 1, y + 3, fcolor);
  display.drawPixel(x + 2, y + 3, fcolor);
  display.drawPixel(x + 3, y + 3, fcolor);
  
}

void refreshDisplay(String room, String z1, String z2, String zr1, String zr2, String zr3, String zr4, int currentPeriod, String date, int day) {
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.begin(display);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_fub30_tf);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(0, 20);
  u8g2_for_adafruit_gfx.setCursor(30, 40);
  u8g2_for_adafruit_gfx.print(room);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR14_tf);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setCursor(30, 65);
  u8g2_for_adafruit_gfx.print(z1);
  u8g2_for_adafruit_gfx.setCursor(30, 90);
  u8g2_for_adafruit_gfx.print(z2);
  u8g2_for_adafruit_gfx.setCursor(165, 25);
  u8g2_for_adafruit_gfx.print(zr1);
  u8g2_for_adafruit_gfx.setCursor(165, 55);
  u8g2_for_adafruit_gfx.print(zr2);
  u8g2_for_adafruit_gfx.setCursor(165, 85);
  u8g2_for_adafruit_gfx.print(zr3);
  u8g2_for_adafruit_gfx.setCursor(165, 115);
  u8g2_for_adafruit_gfx.print(zr4);
  int h = 25;
  for (int i = 0; i < 4; i++) {
    u8g2_for_adafruit_gfx.setCursor(140, h + i * 30);
    u8g2_for_adafruit_gfx.print((String)(currentPeriod + i) + ".");
  }
  /* u8g2_for_adafruit_gfx.setCursor(140,25);
    u8g2_for_adafruit_gfx.print((String)h1+".");
    u8g2_for_adafruit_gfx.setCursor(140, 55);
    u8g2_for_adafruit_gfx.print((String)h2+".");
    u8g2_for_adafruit_gfx.setCursor(140, 85);
    u8g2_for_adafruit_gfx.print((String)h3+".");
    u8g2_for_adafruit_gfx.setCursor(140, 115);
    u8g2_for_adafruit_gfx.print((String)h4+"."); */
  u8g2_for_adafruit_gfx.setFont(u8g2_font_courB08_tf );
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setCursor(5 , 125);
  u8g2_for_adafruit_gfx.print(date);
  // Maybe useless?
  u8g2_for_adafruit_gfx.setFont(u8g2_font_battery19_tn);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setCursor(10 , 120);
  u8g2_for_adafruit_gfx.print("a");
  display.drawLine(160, 25, 270, 25, GxEPD_BLACK);
  display.drawLine(160, 55, 270, 55, GxEPD_BLACK);
  display.drawLine(160, 85, 270, 85, GxEPD_BLACK);
  display.drawLine(160, 115, 270, 115, GxEPD_BLACK);

  display.drawLine(12, 125, 12, 1, GxEPD_BLACK);

  drawM(2, 8, day == 1); 
  drawD(2, 33, day == 2); 
  drawM(2, 58,  day == 3); 
  drawD(2, 83,  day == 4); 
  drawF(2, 108,  day == 5); 
  display.display();
}

/*
  void setClock() {
  dayOfWeek = int(doc["time"]["day"]);
  currentHour = int(doc["time"]["hour"]);
  currentMinute = int(doc["time"]["minute"]);
  currentPeriod = int(doc["current_period"]);

  minutesTillTomorrow = getMinutesTill750(currentHour, currentMinute); //Minutes till 7:50 AM the next day
  minutesTillMonday = minutesTillTomorrow + ((7-dayOfWeek) * 1440); //Minutes till Mo 7:50 AM

  Serial.print("Wochentag: ");
  Serial.println(dayOfWeek);
  Serial.print("Stunde: ");
  Serial.println(currentHour);
  Serial.print("Minute: ");
  Serial.println(currentMinute);
  Serial.print("Minuten bis Morgen: ");
  Serial.println(minutesTillTomorrow);
  Serial.print("Minuten bis Montag: ");
  Serial.println(minutesTillMonday);
  }


  int getMinutesTill750(int cHour, int cMinute) {
  int hours;

  if(cHour <= 7) {
    hours=7-cHour;
  } else {
    hours=(24-cHour + 7);
  }

  return hours*60 + (50-cMinute);
  }

  void maybeSleep() {
  if(((dayOfWeek >= 1 || dayOfWeek <= 4) && currentHour >= 15)) { //Monday to Thursday
    Serial.println("Long sleep");
    doSleep(minutesTillTomorrow);
  }

  if(dayOfWeek == 5) { //Friday
    if(currentHour >= 15){
      Serial.println("Long sleep");
      doSleep(minutesTillMonday);
    }
  }

  if(dayOfWeek >= 6) { //Saturday and Sunday
    Serial.println("Long sleep");
    doSleep(minutesTillMonday);
  }

  }
*/



void doSleep(uint64_t min) {
  esp_sleep_enable_timer_wakeup(min * 60 * 1000 * 1000ull);
  esp_deep_sleep_start();
}

void setup()
{
  Serial.begin(115200);

  long t = millis();

  WiFi.persistent(false);
  // WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  int connection_attempts = 0;
  while ((WiFi.status() != WL_CONNECTED)) {
    if (connection_attempts > 10) {
      doSleep(95);
    }
    delay(1000);
    Serial.print(".");
    Serial.println("");
    connection_attempts++;
  }
  Serial.println(" connected to wifi");


  if (!loadTimeTable()) {
    Serial.println("ERROR LOL");
    Serial.println((millis() - t) / 1000.0);
    doSleep(95);
  }

  display.init();
  Serial.println(doc["room_no"].as<String>());

  Serial.println((millis() - t) / 1000.0);
  refreshDisplay(doc["room_no"].as<String>(),
                 doc["room_z1"].as<String>(),
                 doc["room_z2"].as<String>(),
                 doc["timetable_z1"].as<String>(),
                 doc["timetable_z2"].as<String>(),
                 doc["timetable_z3"].as<String>(),
                 doc["timetable_z4"].as<String>(),
                 doc["current_period"].as<int>(),
                 "",
                 doc["day_of_week"].as<int>());

  Serial.println((millis() - t) / 1000.0);
  if (doc["sleep_time"].as<int>() != NULL) {
    doSleep(doc["sleep_time"].as<int>());
  } else {
    doSleep(95);
  }

}



void loop() {};
