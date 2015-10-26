// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    3    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 12

#define BRIGHTNESS 10
#define FADE true

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show(); // Initialize all pixels to 'off'
}


bool checkButton() {
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      // blink led 0 bright white to confirm the button click
      uint32_t currentColor = strip.getPixelColor(0);
      strip.setBrightness(64);
      strip.setPixelColor(0, strip.Color(127, 127, 127));
      strip.show();
      delay(100);
      strip.setBrightness(BRIGHTNESS);
      strip.setPixelColor(0, currentColor);
      strip.show();
      // Set the last button state to the old state.
      oldState = newState;
      return true;
    }
  }
  // Set the last button state to the old state.
  oldState = newState;
  return false;
}


void loop() {
  // Check if state changed from high to low (button press).
  bool buttonPressed = false;
  if (buttonPressed || checkButton()) {
    showType++;
    if (showType > 9) {
      showType=0;
    }
    buttonPressed = startShow(showType);
  }
}

bool startShow(int i) {
  switch(i){
    case 0: 
      return colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
    case 1: 
      return colorWipe(strip.Color(255, 0, 0), 50);  // Red
    case 2: 
      return colorWipe(strip.Color(0, 255, 0), 50);  // Green
    case 3:
      return colorWipe(strip.Color(0, 0, 255), 50);  // Blue
    case 4:
      return theaterChase(strip.Color(127, 127, 127), 50, FADE); // White
    case 5:
      return theaterChase(strip.Color(127,   0,   0), 50, FADE); // Red
    case 6:
      return theaterChase(strip.Color(  0,   0, 127), 50, FADE); // Blue
    case 7:
      return rainbow(20);
    case 8:
      return rainbowCycle(20);
    case 9: 
      return theaterChaseRainbow(50);
  }
}

// Fill the dots one after the other with a color
bool colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    if (checkButton()) {
      return true;
    }
  }
  return false;
}

bool rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 256; j++) {
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
    if (checkButton()) {
      return true;
    }
  }
  return false;
}

// Slightly different, this makes the rainbow equally distributed throughout
bool rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    if (checkButton()) {
      return true;
    }
  }
  return false;
}

//Theatre-style crawling lights.
bool theaterChase(uint32_t c, uint8_t wait, bool fade) {
  uint8_t brightness = 255;
  if (fade) {
    strip.setBrightness(brightness);
  }
  for (int j=0; j < 10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i + 3) {
        strip.setPixelColor(i + q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      if (checkButton()) {
        strip.setBrightness(BRIGHTNESS);
        return true;
      }
      if (fade && brightness > BRIGHTNESS + 10) {
        brightness = brightness - 10;
        strip.setBrightness(brightness);
      }
      for (int i=0; i < strip.numPixels(); i=i + 3) {
        strip.setPixelColor(i + q, 0);        //turn every third pixel off
      }
    }
  }
  strip.setBrightness(BRIGHTNESS);
  return false;
}

//Theatre-style crawling lights with rainbow effect
bool theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      if (checkButton()) {
        return true;
      }

      for (int i=0; i < strip.numPixels(); i=i + 3) {
        strip.setPixelColor(i + q, 0);        //turn every third pixel off
      }
    }
  }
  return false;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
