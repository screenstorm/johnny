#include "clickButton/clickButton.h"

#include "neopixel/neopixel.h"

#include "application.h"

#include "LiquidCrystal/LiquidCrystal.h"

#include "elapsedMillis/elapsedMillis.h"

// Make sure to update these to match how you've wired your pins.
// pinout on LCD [RS, EN, D4, D5, D6, D7];
// pin nums LCD  [ 4,  6, 11, 12, 13, 14];
// Shield Shield [RS, EN, D4, D5, D6, D7];
// Spark Core    [D3, D5, D2, D4, D7, D8];
LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812

String ledMode;
int color1r = 0;
int color1g = 170;
int color1b = 187;
int color2r = 255;
int color2g = 175;
int color2b = 20;
int devMode = 1; // debug events

elapsedMillis timeElapsed;
unsigned int interval = 20000;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// the Button
const int buttonPin1 = A1;
ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);
int buttonClicked = 0; 
int val = 0;

void setup() {
    // set up the LCD's number of columns and rows: 
    lcd.begin(16,2);
    // Print a message to the LCD.
    lcd.print("hello world");
    
    strip.begin();
    strip.setBrightness(10);
    strip.show(); // Initialize all pixels to 'off'
    
    Particle.function("whisper",whisper);
    
    // btn setup
    Serial.begin(9600);
    pinMode(A1, INPUT_PULLUP);
    
    button1.debounceTime   = 10;   // Debounce timer in ms
    button1.multiclickTime = false;  // Time limit for multi clicks
    button1.longClickTime  = false; // time until "held-down clicks" register
}

void loop() {
    // process LED vars
    if(ledMode == "kit"){
        CylonBounce(color1r, color1g, color1b, 4, 15, 50);
    }else if(ledMode == "still"){
        colorAll(strip.Color(color1r, color1g, color1b), 1000*10);
    }else if(ledMode == "disco"){
        colorKit(strip.Color(color1r, color1g, color1b), strip.Color(color2r, color2g, color2b), 100);
    }else if(ledMode == "strobe"){
        Strobe(color1r, color1g, color1b, 4, 18, 100);   
    }
    
    if (timeElapsed > interval) {   
        ledMode = NULL;
        lcd.clear();
        FadeInOut(color1r, color1g, color1b, 4);
        // reset the counter to 0 so the counting starts over...
        timeElapsed = 0;
        buttonClicked = 0;
        debug("reset timer", 0);
    }
    
  // Update button state
  button1.Update();
  
  if(button1.clicks != 0){
        //buttonOneClick();
        buttonTwoClick();
        delay(20);
  }
}

// Log message to cloud, message is a printf-formatted string
void debug(String message, int value) {
    if(devMode == 0){
        return;
    }
    char msg [50];
    sprintf(msg, message.c_str(), value);
    Particle.publish("DEBUG", msg);
}

// process commands from cloud
int whisper(String command) {
    debug("whisper() called",0);
    timeElapsed = 0;
    
    //look for the matching argument <-- max of 64 characters long
    int p = 0;
    
    while (p<(int)command.length()) {
        int i = command.indexOf(',',p);
        if (i<0) i = command.length();
        int j = command.indexOf('=',p);
        if (j<0) break;
        String key = command.substring(p,j).toUpperCase();
        String value = command.substring(j+1,i);
        int val = value.toInt();
        
        if (key=="LCDLINE1"){
            // Set LCD line 1
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(value);
        }
        else if (key=="LCDLINE2"){
            // Set LCD line 2
            lcd.setCursor(0, 1);
            lcd.print(value);
        }
        else if (key=="LEDMODE") {
            ledMode = value;
        }
        else if (key=="COLOR1"){
            long number = (long) strtol( &value[1], NULL, 16);
            color1r = number >> 16;
            color1g = number >> 8 & 0xFF;
            color1b = number & 0xFF;
        }
        else if (key=="COLOR2"){
            long number = (long) strtol( &value[1], NULL, 16);
            color2r = number >> 16;
            color2g = number >> 8 & 0xFF;
            color2b = number & 0xFF;
        }
        
        p = i+1;
    }
    
    timeElapsed = 0;
    return 1;
}

void resetInterval(String str){
    // reset timer by moving it along to the next interval
    interval = 20000;
    ledMode = NULL;
    lcd.clear();
    debug("reset timer "+str+" = ", interval);
}

void buttonOneClick(){  
    String value = "";
    int key = random(1, 11);
    if(key == 1) value = "= ipv ==?";
    else if(key == 2) value = "Cache probleem?";
    else if(key == 3) value = "Bestandsrechten?";
    else if(key == 4) value = "HTTP - HTTPS redirect?";
    else if(key == 5) value = "Lokale DNS cache?";
    else if(key == 6) value = "Werkt cronjob wel?";
    else if(key == 7) value = "PICNIC";
    else if(key == 8) value = "Oude code van Jan?";
    else if(key == 9) value = "2 gelijke titels in dropdown EXT";
    else if(key == 10) value = "verbergen aangevinkt?";
    else if(key == 11) value = "parent::beforeFilter mist?";
    
    whisper("LCDLINE1="+value.substring(0,16)+",LCDLINE2="+value.substring(16)+",LEDMODE=still,COLOR1=#008a16");
}

void buttonTwoClick(){
    String value = String(random(1, 100));
    whisper("LCDLINE1=Ureninschatting:,LCDLINE2="+value+" uur,LEDMODE=still,COLOR1=#8a0008");
}

/*
================ COLOR FUNCTIONS ==================
*/

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;

  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void colorKit(uint32_t c1, uint32_t c2, uint8_t wait) {
    colorWipe(c1, 50); // firstcolor
    delay(wait);
    colorWipe(c2, 50); // secondcolor
    delay(wait);
}

void FadeInOut(byte red, byte green, byte blue, int SpeedDelay)
{
    float r, g, b;
    for(int k = 0;k < 256;k=k+1)
    {
        r = (k/256.0)*red;
        g = (k/256.0)*green;
        b = (k/256.0)*blue;
        setAll(r,g,b);
        showStrip();
        delay(SpeedDelay);
    }
    for(int k = 255;k >= 0;k=k-2)
    {
        r = (k/256.0)*red;
        g = (k/256.0)*green;
        b = (k/256.0)*blue;
        setAll(r,g,b);
        showStrip();
        delay(SpeedDelay*2);
    }
}

// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<(strip.numPixels()/2); i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(strip.numPixels()-i, c);
        strip.show();
        delay(wait);
    }
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for(int i = 0;i < PIXEL_COUNT-EyeSize-2;i++)
    {
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1;j <= EyeSize;j++)
        {
            setPixel(i+j, red, green, blue);
        }
        setPixel(i+EyeSize+1, red/10, green/10, blue/10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
    for(int i = PIXEL_COUNT-EyeSize-2;i > 0;i--)
    {
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1;j <= EyeSize;j++)
        {
            setPixel(i+j, red, green, blue);
        }
        setPixel(i+EyeSize+1, red/10, green/10, blue/10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause)
{
    for(int j = 0;j < StrobeCount;j++)
    {
        setAllLeft(red,green,blue);
        showStrip();
        delay(FlashDelay);
        setAllLeft(0,0,0);
        showStrip();
        delay(FlashDelay);
    }
    delay(EndPause);
    
    for(int j = 0;j < StrobeCount;j++)
    {
        setAllRight(red,green,blue);
        showStrip();
        delay(FlashDelay);
        setAllRight(0,0,0);
        showStrip();
        delay(FlashDelay);
    }
    delay(EndPause);
}

void setAll(byte red, byte green, byte blue)
{
    for(int i = 0;i < PIXEL_COUNT;i++ ){
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void setAllLeft(byte red, byte green, byte blue)
{
    for(int i = 0;i < PIXEL_COUNT/2;i++ ){
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void setAllRight(byte red, byte green, byte blue)
{
    for(int i = PIXEL_COUNT/2;i < PIXEL_COUNT;i++ ){
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void showStrip()
{
    strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}