#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#define PIN 4
#define PIN2 2

#define mw 32
#define mh 8
#define NUMMATRIX (mw*mh)
 
CRGB matrixleds[NUMMATRIX];
CRGB matrixleds2[NUMMATRIX];


FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, mw, mh, 1, 1,     
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );
      
      FastLED_NeoMatrix *matrix2 = new FastLED_NeoMatrix(matrixleds2, mw, mh, 1, 1,     
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );


// Replace with your network credentials
const char* ssid       = "testnet";
const char* password   = "marchuter";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

String Time()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
return(timeStamp);
delay(1000);
}

String Date()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(5, splitT);
  // Extract time
return(dayStamp);
delay(1000);
}




void setup() {
  timeClient.setTimeOffset(3600);

  FastLED.addLeds<NEOPIXEL, PIN>(matrixleds, NUMMATRIX);
    FastLED.addLeds<NEOPIXEL, PIN2>(matrixleds2, NUMMATRIX);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(10);
  matrix2->begin();
  matrix2->setTextWrap(false);
  matrix2->setBrightness(10);
  
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
}
void loop() {
      matrix->fillScreen(0);

  matrix->setCursor(0,0);
  matrix->print(Date());
  matrix->setTextColor(matrix->Color(0, 0, 255));

    matrix->show();


  matrix2->fillScreen(0);

  matrix2->setCursor(0,0);
  matrix2->print(Date());
  matrix2->setTextColor(matrix->Color(0, 0, 255));

    matrix2->show();

    delay(1000);

}
