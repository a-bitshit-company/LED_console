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

      const uint16_t colors[] = {   matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };

void setup() {

  
  FastLED.addLeds<NEOPIXEL, PIN>(matrixleds, NUMMATRIX);
    FastLED.addLeds<NEOPIXEL, PIN2>(matrixleds2, NUMMATRIX);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(10);
    
  matrix2->begin();
  matrix2->setTextWrap(false);
  matrix2->setBrightness(10);
  FastLED.clear();
}
 
int x    = mw;
int pass = 0;
 
void loop() {
  
      matrix->fillScreen(0);
      matrix2->fillScreen(0);

  matrix->setCursor(0,0);
  matrix->print(F("Hallo"));
  matrix->setTextColor(matrix->Color(0, 0, 255));
 
  matrix2->setCursor(0,0);
  matrix2->print(F("Hi"));
  matrix2->setTextColor(matrix->Color(0, 0, 255));


  matrix->show();
  matrix2->show();


  delay(100);
}
