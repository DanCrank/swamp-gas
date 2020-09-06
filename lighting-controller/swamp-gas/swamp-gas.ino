#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NUM_PIXELS 60
#define PIXEL_PIN 4
#define PIXEL_BRIGHTNESS 50

// multiplexer A (LSB) to trinket A0 (Arduino pin 1!) as digital out
#define MUX0_PIN 1
// multiplexer B       to trinket A1 (Arduino pin 2!) as digital out
#define MUX1_PIN 2
// multiplexer C (MSB) to trinket A2 (Arduino pin 0!) as digital out
#define MUX2_PIN 0
// multiplexer out     to trinket A3 (pin labeled 3) as digital in
#define MUXOUT_PIN 3

Adafruit_NeoPixel strip;

void setup() {
  Serial.println("SwampGas 1.0 startup");
  strip = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.setBrightness(PIXEL_BRIGHTNESS);
  strip.show(); // Initialize all pixels to 'off'
  pinMode(MUX0_PIN, OUTPUT);
  pinMode(MUX1_PIN, OUTPUT);
  pinMode(MUX2_PIN, OUTPUT);
  pinMode(MUXOUT_PIN, INPUT); //circuit has pull-ups on the input side of the mux
  //TODO: turn off the NeoPixel on the board
}

void loop() {
  int iDipswitch = readDipswitch();
  int iPattern = (iDipswitch >> 4 & 0x0F);
  int iColor = color(iDipswitch & 0x0F);
  switch (iPattern) {
    case 0:
      //clockwise sweep fast
      clockwiseSweep(iColor, 12, 5);
      break;
    case 1:
      //clockwise sweep slow
      clockwiseSweep(iColor, 12, 15);
      break;
    case 2:
      //3-way bounce
      bounce(iColor, 5, 3, 35);
      break;
    case 3:
      //6-way bounce
      bounce(iColor, 5, 6, 35);
      break;
    //TODO: more patterns
    default:
      delay(1000);

  }
  taste_the_rainbow();
}

//never apologize, never explain
#define RAINBOW_SPEED 16
int iRainbowMagic = 0;
void taste_the_rainbow() {
    iRainbowMagic = (iRainbowMagic + RAINBOW_SPEED) % 256;
}

// swampgas color constants (indexed to dip switch selections)
#define WHITE 0
#define RED 1
#define BLUE 2
#define GREEN 3
#define YELLOW 4
#define CYAN 5
#define PURPLE 6
// 7-14 TBD; will return white
#define RAINBOW 15

// color mapping
uint32_t color(int iSelect) {
  switch (iSelect) {
    case WHITE: return strip.Color(255, 255, 255);
    case RED: return strip.Color(255, 0, 0);
    case BLUE: return strip.Color(0, 0, 255);
    case GREEN: return strip.Color(0, 255, 0);
    case YELLOW: return strip.Color(255, 150, 0);
    case CYAN: return strip.Color(0, 255, 255);
    case PURPLE: return strip.Color(180, 0, 255);
    case RAINBOW: return wheel(iRainbowMagic);
    default: return strip.Color(255, 255, 255);
  }
}

uint32_t wheel(int iPos) {
  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  if ((iPos < 0) || (iPos > 255))
    return strip.Color(0, 0, 0);
  if (iPos < 85)
    return strip.Color(255 - iPos * 3, iPos * 3, 0);
  if (iPos < 170)
  {
    iPos -= 85;
    return strip.Color(0, 255 - iPos * 3, iPos * 3);
  }
  iPos -= 170;
  return strip.Color(iPos * 3, 0, 255 - iPos * 3);
}

uint32_t fade(uint32_t color, double frac) {
  if (frac == 0.0) return strip.Color(0, 0, 0);
  int red = color >> 16 & 0xFF;
  int green = color >> 8 & 0xFF;
  int blue = color & 0xFF;
  return strip.Color(int(red * frac * frac),
                     int(green * frac * frac),
                     int(blue * frac * frac));
}

int bounceRight(int iPos, int iPixelsPerSide) {
  if (iPos >= iPixelsPerSide)
    return (iPixelsPerSide - (iPos - iPixelsPerSide) - 2);
  return iPos;
}

int bounceLeft(int iPos) {
  if (iPos < 0)
    return (iPos * -1);
  return iPos;
}

void clockwiseSweep(uint32_t color, int iWidth, int iWait) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    for (int j = 0; j <= iWidth; j++)
      strip.setPixelColor(((i + j) % NUM_PIXELS), fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
}

void bounce(uint32_t color, int iWidth, int iSides, int iWait) {
  int iPixelsPerSide = int(NUM_PIXELS / iSides);
  for (int i = 0; i < iPixelsPerSide; i++) {
    // sweeping left to right
    for (int j = 0; j < (iWidth + 1); j++)
      for (int k = 0; k < NUM_PIXELS; k += iPixelsPerSide)
        strip.setPixelColor(bounceRight(i + j, iPixelsPerSide) + k, fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
  for (int i = iPixelsPerSide - 2; i > 1; i -= 1) {
    // sweeping right to left
    for (int j = 0; j <= iWidth; j++)
      for (int k = 0; k < NUM_PIXELS; k += iPixelsPerSide)
        strip.setPixelColor(bounceLeft(i - j) + k, fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
}

int readDipswitchPin(int iPin) {
  if ((iPin < 0) || (iPin > 7)) { Serial.println("WARN: dipswitch pin index out of range"); return false; }
  // address the mux
  if (iPin & 0x4) digitalWrite(MUX2_PIN, HIGH); else digitalWrite(MUX2_PIN, LOW); //really?
  if (iPin & 0x2) digitalWrite(MUX1_PIN, HIGH); else digitalWrite(MUX1_PIN, LOW);
  if (iPin & 0x1) digitalWrite(MUX0_PIN, HIGH); else digitalWrite(MUX0_PIN, LOW);
  // settle
  delay(1);
  //read the mux output (NOTE: active LOW)
  if (digitalRead(MUXOUT_PIN) == LOW) return 1; else return 0; //REALLY really?
}

int readDipswitch() {
  int iValue = 0;
  for (int i = 0; i < 8; i++) {
    int iPinVal = readDipswitchPin(i);
    Serial.print("Pin ");
    Serial.print(i);
    Serial.print(" is ");
    Serial.println(iPinVal);
    iValue += iPinVal << i;
  }
  return iValue;
}
