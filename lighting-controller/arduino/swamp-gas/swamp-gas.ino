/*
 * swamp-gas.ino: lighting controller code for a flying saucer, based on Arduino & NeoPixel.
 * Copyright (C) 2020 Dan Crank (danno@danno.org)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NUM_PIXELS 60
#define PIXEL_PIN 4
#define PIXEL_BRIGHTNESS 100 // scale of 255

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
  Serial.println("SwampGas 1.0 Copyright (C) 2020 Dan Crank (danno@danno.org)");
  Serial.println("This program comes with ABSOLUTELY NO WARRANTY.");
  strip = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.setBrightness(PIXEL_BRIGHTNESS);
  strip.clear(); // Initialize all pixels to 'off'
  strip.show();
  pinMode(MUX0_PIN, OUTPUT);
  pinMode(MUX1_PIN, OUTPUT);
  pinMode(MUX2_PIN, OUTPUT);
  pinMode(MUXOUT_PIN, INPUT); //circuit has pull-ups on the input side of the mux
  //TODO: turn off the NeoPixel on the board
}

void loop() {
  int iDipswitch = readDipswitch();
  int iPattern = (iDipswitch >> 4 & 0x0F);
  int iColorIndex = (iDipswitch & 0x0F);
  switch (iPattern) {
    case 0:
      //clockwise sweep fast
      clockwiseSweep(iColorIndex, 12, 1);
      break;
    case 1:
      //clockwise sweep slow
      clockwiseSweep(iColorIndex, 12, 15);
      break;
    case 2:
      //3-way bounce
      bounce(iColorIndex, 5, 3, 20);
      break;
    case 3:
      //6-way bounce
      bounce(iColorIndex, 5, 6, 30);
      break;
    case 4:
      //pulse fast
      pulse(iColorIndex, 3);
      break;
    case 5:
      //pulse slow
      pulse(iColorIndex, 1);
      break;
    //TODO: more patterns
    default:
      delay(1000);
  }
}

// swampgas color constants (indexed to dip switch selections)
#define WHITE 0
#define RED 1
#define BLUE 2
#define GREEN 3
#define YELLOW 4
#define CYAN 5
#define PURPLE 6
// 7-13 TBD; will return white
#define RAINBOW_SLOW 14
#define RAINBOW_FAST 15

// color mapping
uint32_t mapColor(int iColorIndex) {
  switch (iColorIndex) {
    case WHITE: return strip.Color(255, 255, 255);
    case RED: return strip.Color(255, 0, 0);
    case BLUE: return strip.Color(0, 0, 255);
    case GREEN: return strip.Color(0, 255, 0);
    case YELLOW: return strip.Color(255, 150, 0);
    case CYAN: return strip.Color(0, 255, 255);
    case PURPLE: return strip.Color(180, 0, 255);
    case RAINBOW_SLOW: return rainbow(1.0);
    case RAINBOW_FAST: return rainbow(6.0);
    default: return strip.Color(255, 255, 255);
  }
}

// returns true if the index refers to a changing color (such as rainbow);
// animation functions can call this to determine whether they need to
// update the color during animations.
boolean isDynamicColor(int iColorIndex) {
  switch (iColorIndex) {
    case RAINBOW_SLOW:
    case RAINBOW_FAST:
      return true;
    default: return false;
  }
}

uint32_t rainbow(double dCyclesPerMinute) {
  // return a rainbow color based on the millis() value
  int i = ((int)(((double)(millis()) * 256.0 / (60000.0 / dCyclesPerMinute)))) % 256;
  // the lines below are adapted from the Adafruit NeoPixel demo code
  // https://github.com/adafruit/Adafruit_CircuitPython_NeoPixel (MIT license)
  // The colours are a transition r - g - b - back to r.
  if ((i < 0) || (i > 255))
    return strip.Color(0, 0, 0);
  if (i < 85)
    return strip.Color(255 - i * 3, i * 3, 0);
  if (i < 170)
  {
    i -= 85;
    return strip.Color(0, 255 - i * 3, i * 3);
  }
  i -= 170;
  return strip.Color(i * 3, 0, 255 - i * 3);
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

void clockwiseSweep(int iColorIndex, int iWidth, int iWait) {
  uint32_t color = mapColor(iColorIndex);
  for (int i = 0; i < NUM_PIXELS; i++) {
    if (isDynamicColor(iColorIndex)) color = mapColor(iColorIndex);
    for (int j = 0; j <= iWidth; j++)
      strip.setPixelColor(((i + j) % NUM_PIXELS), fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
}

void bounce(int iColorIndex, int iWidth, int iSides, int iWait) {
  int iPixelsPerSide = int(NUM_PIXELS / iSides);
  uint32_t color = mapColor(iColorIndex);
  for (int i = 0; i < iPixelsPerSide; i++) {
    // sweeping left to right
    if (isDynamicColor(iColorIndex)) color = mapColor(iColorIndex);
    for (int j = 0; j < (iWidth + 1); j++)
      for (int k = 0; k < NUM_PIXELS; k += iPixelsPerSide)
        strip.setPixelColor(bounceRight(i + j, iPixelsPerSide) + k, fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
  for (int i = iPixelsPerSide - 2; i > 1; i -= 1) {
    // sweeping right to left
    if (isDynamicColor(iColorIndex)) color = mapColor(iColorIndex);
    for (int j = 0; j <= iWidth; j++)
      for (int k = 0; k < NUM_PIXELS; k += iPixelsPerSide)
        strip.setPixelColor(bounceLeft(i - j) + k, fade(color, ((double)j / (double)iWidth)));
    strip.show();
    if (iWait > 0) delay(iWait);
  }
}

void pulse(int iColorIndex, int iSpeed) {
  uint32_t color = mapColor(iColorIndex);
  strip.clear();
  strip.show();
  for (int i = 0; i <= 100; i += iSpeed) {
    if (isDynamicColor(iColorIndex)) color = mapColor(iColorIndex);
    strip.fill(fade(color, (double)(i) / 100.0));
    strip.show();
  }
  for (int i = 100; i >= 0; i -= iSpeed) {
    if (isDynamicColor(iColorIndex)) color = mapColor(iColorIndex);
    strip.fill(fade(color, (double)(i) / 100.0));
    strip.show();
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
    iValue += iPinVal << i;
  }
  return iValue;
}
