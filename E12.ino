/*************************************************************************************************************************************************
 *  TITLE: This sketch connects to a local WiFi network, connects to a network server and obtains the network time. It then converts this to a 
 *  readable format and displays it to the serial terminal and an OLED module. Refer to the video and post for more information. 
 *
 *  By Frenoy Osburn
 *  YouTube Video: https://youtu.be/3LkKYtQqzKo
 *  BnBe Post: https://www.bitsnblobs.com/network-time-using-esp8266
 *************************************************************************************************************************************************/

  /********************************************************************************************************************
 *  Board Settings:
 *  Board: "WeMos D1 R1 or Mini"
 *  Upload Speed: "921600"
 *  CPU Frequency: "80MHz"
 *  Flash Size: "4MB (FS:@MB OTA:~1019KB)"
 *  Debug Port: "Disabled"
 *  Debug Level: "None"
 *  VTables: "Flash"
 *  IwIP Variant: "v2 Lower Memory"
 *  Exception: "Legacy (new can return nullptr)"
 *  Erase Flash: "Only Sketch"
 *  SSL Support: "All SSL ciphers (most compatible)"
 *  COM Port: Depends *On Your System*
 *********************************************************************************************************************/
 
 /*
  This is an example file for using the time function in ESP8266 or ESP32 tu get NTP time
  It offers two functions:

  - getNTPtime(struct tm * info, uint32_t ms) where info is a structure which contains time
  information and ms is the time the service waits till it gets a response from NTP.
  Each time you cann this function it calls NTP over the net.

  If you do not want to call an NTP service every second, you can use
  - getTimeReducedTraffic(int ms) where ms is the the time between two physical NTP server calls. Betwwn these calls,
  the time structure is updated with the (inaccurate) timer. If you call NTP every few minutes you should be ok

  The time structure is called tm and has teh following values:

  Definition of struct tm:
  Member  Type  Meaning Range
  tm_sec  int seconds after the minute  0-61*
  tm_min  int minutes after the hour  0-59
  tm_hour int hours since midnight  0-23
  tm_mday int day of the month  1-31
  tm_mon  int months since January  0-11
  tm_year int years since 1900
  tm_wday int days since Sunday 0-6
  tm_yday int days since January 1  0-365
  tm_isdst  int Daylight Saving Time flag

  because the values are somhow akwardly defined, I introduce a function makeHumanreadable() where all values are adjusted according normal numbering.
  e.g. January is month 1 and not 0 And Sunday or monday is weekday 1 not 0 (according definition of MONDAYFIRST)

  Showtime is an example on how you can use the time in your sketch

  The functions are inspired by work of G6EJD ( https://www.youtube.com/channel/UCgtlqH_lkMdIa4jZLItcsTg )
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <time.h>
#include <U8g2lib.h>
#include "credentials.h"
//#include "u8g2_font_unifont_t_korean_NanumGothicCoding_16.h"

const char* ssid = mySSID;              //from credentials.h file
const char* password = myPASSWORD;      //from credentials.h file

const char* NTP_SERVER = "kr.pool.ntp.org"; //"time.bora.net"; //
const char* TZ_INFO    = "KST -09";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;


U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* cs=*/ U8X8_PIN_NONE, /* dc=*/ 2, /* reset=*/ 16 ); //성공 6pin
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 2, /* reset=*/ 16); //성공 7pin
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* reset=*/ U8X8_PIN_NONE); //성공



void setup() 
{
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  Serial.begin(115200);
  Serial.println("\n\nNTP Time Test\n");
  WiFi.begin(ssid, password);

  Serial.print("Connecting to network");

//      u8g2.setFont(u8g2_font_unifont_t_korean_NanumGothicCoding_16);
//      u8g2.setFontDirection(0);
//      u8g2.clearBuffer();
//      u8g2.setCursor(0, 16);
//      u8g2.print("안녕1 H ☞ ☯.");
//      u8g2.setFont(u8g2_font_unifont_t_korean2);
//      u8g2.setCursor(0,40);
//      u8g2.print("안녕2 H ☞ ☯.");  
//      u8g2.sendBuffer();
//      delay(2000);

      

  u8g2.firstPage(); 
  do{
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.drawStr(0,40,"Connecting WIFI");
  }while(u8g2.nextPage()); 
  
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);    
    if (++counter > 100) 
      ESP.restart();
    Serial.print( "." );
  }
  Serial.println("\nWiFi connected\n\n");


    
  u8g2.firstPage(); 
  do{
    u8g2.setFont(u8g2_font_helvR12_tr );
    u8g2.drawStr(0,40,"Connecting NTP");      
  }while(u8g2.nextPage());
    
  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);

  if (getNTPtime(10)) 
  {  
    // wait up to 10sec to sync

  } 
  else 
  {
    Serial.println("Time not set");
    ESP.restart();
  }
  showTime(&timeinfo);
  lastNTPtime = time(&now);
  lastEntryTime = millis();
}

void loop() 
{
  getNTPtime(10);
  showTime(&timeinfo);
  delay(10000);
}

bool getNTPtime(int sec) 
{
  {
    uint32_t start = millis();
    do
    {
      time(&now);
      localtime_r(&now, &timeinfo);
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    
    if (timeinfo.tm_year <= (2016 - 1900)) 
        return false;  // the NTP call was not successful
    
    Serial.print("Time Now: ");  
    Serial.println(now); 
  }
  return true;
}

void showTime(tm *localTime) 
{
  //print to serial terminal
  Serial.print(localTime->tm_mday);
  Serial.print('/');
  Serial.print(localTime->tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime->tm_year - 100);
  Serial.print('-');
  Serial.print(localTime->tm_hour);
  Serial.print(':');
  Serial.print(localTime->tm_min);
  Serial.print(':');
  Serial.print(localTime->tm_sec);
  Serial.print(" Day of Week ");
  Serial.println(localTime->tm_wday);
  Serial.println();

  //display on OLED
  char time_output[30];
  int offset_oled;

  offset_oled = rand();
  Serial.print("rand() : ");
  Serial.println(offset_oled);
  offset_oled = offset_oled % 10;
  Serial.print("offset_oled : ");
  Serial.println(offset_oled);
  
  
  
 u8g2.firstPage();
 do{
  u8g2.setFont(u8g2_font_logisoso38_tn      );
  //sprintf(time_output, "%02d:%02d:%02d", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
  sprintf(time_output, "%02d:%02d", localTime->tm_hour, localTime->tm_min);
  u8g2.drawStr(0 + offset_oled ,60,time_output);
  
  u8g2.setFont(u8g2_font_helvR12_tr  );
  //sprintf(time_output, "%02d/%02d/%02d", localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year - 100);
  sprintf(time_output, "%02d/%02d", localTime->tm_mon + 1, localTime->tm_mday);
  u8g2.drawStr(2+ offset_oled,15,time_output);

  //u8g2.setFont(u8g2_font_unifont_t_korean2); 
  //sprintf( time_output, "%s", getDOW(localTime->tm_wday));
  //u8g2.drawStr(46+ offset_oled,15,time_output);
  
  u8g2.setFont(u8g2_font_unifont_t_korean2); //한글폰트는 u8g2.drawStr()로 안되고, u8g2.print() 이용해야함 .
  u8g2.setCursor(46+ offset_oled,15); 
  u8g2.print(getDOW(localTime->tm_wday)); 

  
 }while (u8g2.nextPage());

}

char * getDOW(uint8_t tm_wday)
{
  switch(tm_wday)
  {
    case 1:
      return "(월)";
      break;

    case 2:
      return "(화)";
      break;

    case 3:
      return "(수)";
      break;

    case 4:
      return "(목)";
      break;

    case 5:
      return "(금)";
      break;

    case 6:
      return "(토)";
      break;

    case 7:
      return "(일)";
      break;

    default:
      return "Error";
      break;
  }
}
