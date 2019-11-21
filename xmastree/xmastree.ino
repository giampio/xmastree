#include <Adafruit_NeoPixel.h>
#include <IRremote.h>

#define NUM_LEDS 60
#define PIN_LED 6

#define PIN_PIR 2
#define PIN_IR 3

#define EYE_SIZE 4
#define SPEED_DELAY 100
#define RETURN_DELAY 30

byte red = random(255);
byte green = random(255);
byte blue = random(255);

IRrecv irrecv(PIN_IR);
decode_results results;
int effect = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

void setup() 
{  
  // Setup serial
  Serial.begin(9600);
  
  // Setup PIR pin
  pinMode(PIN_PIR, INPUT); 
  attachInterrupt(0, RandomEffect, CHANGE);  
  
  // Inizialize ir receive
  irrecv.enableIRIn();
  attachInterrupt(1, GenerateEffect, CHANGE);  
    
  // Inizialize led strip  
  pixels.begin();
  pixels.show();
}

void loop() 
{
}

/* Function for set all led */
void SetAll(byte red, byte green, byte blue) 
{
  for(int i = 0; i < NUM_LEDS; i++ ) 
  {
    pixels.setPixelColor(i, red, green, blue); 
  }
  pixels.show();
}

/* Function for rotation colors */
byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

/* Rainbow Cycle*/
void RainbowCycle(int SpeedDelay)
{
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      pixels.setPixelColor(i, *c, *(c+1), *(c+2));
    }
    pixels.show();
  }
}

/* Larson Scanner */
void Kitt(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{

  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) 
  {
    SetAll(0,0,0);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    
    for(int j = 1; j <= EyeSize; j++) 
    {
      pixels.setPixelColor(i+j, red, green, blue); 
    }
    
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) 
  {
    SetAll(0,0,0);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    
    for(int j = 1; j <= EyeSize; j++) 
    {
      pixels.setPixelColor(i+j, red, green, blue); 
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }
  
  delay(ReturnDelay);
}

/* RGB loop */
void RGBLoop(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: SetAll(k,0,0); break;
        case 1: SetAll(0,k,0); break;
        case 2: SetAll(0,0,k); break;
      }
      pixels.show();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: SetAll(k,0,0); break;
        case 1: SetAll(0,k,0); break;
        case 2: SetAll(0,0,k); break;
      }
      pixels.show();
      delay(3);
    }
  }
}

/* Fade In/Out */
void FadeInOut(byte red, byte green, byte blue)
{
  float r, g, b;
  
  for(int i = 0; i < 7; i++)
  {    
    for(int k = 0; k < 256; k=k+1) { 
      r = (k/256.0)*red;
      g = (k/256.0)*green;
      b = (k/256.0)*blue;
      SetAll(r,g,b);
      pixels.show();
    }
       
    for(int k = 255; k >= 0; k=k-2) {
      r = (k/256.0)*red;
      g = (k/256.0)*green;
      b = (k/256.0)*blue;
      SetAll(r,g,b);
      pixels.show();
    }
  }
}

/* Sparkle */
void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  for(int i = 0; i < 6000; i++)
  {
    int Pixel = random(NUM_LEDS);
    pixels.setPixelColor(Pixel,red,green,blue);
    pixels.show();
    delay(SpeedDelay);
    pixels.setPixelColor(Pixel,0,0,0);
  }
  SetAll(0,0,0);
}

/* Snow Sparkle */
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
    
  for(int i = 0; i < 100; i++)
  {
    SetAll(red,green,blue);
    int Pixel = random(NUM_LEDS);
    pixels.setPixelColor(Pixel,0xff,0xff,0xff);
    pixels.show();
    delay(SparkleDelay);
    pixels.setPixelColor(Pixel,red,green,blue);
    pixels.show();
    delay(SpeedDelay);
  }
  
  SetAll(0,0,0);
}

/* Running Lights */
void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  
  for(int j=0; j<NUM_LEDS*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        pixels.setPixelColor(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
      
      pixels.show();
      delay(WaveDelay);
  }
  
  SetAll(0,0,0);
}

/* Theater ChaseRainbow */
void TheaterChaseRainbow(int SpeedDelay) {
  byte *c;
  
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          pixels.setPixelColor(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
        }
        pixels.show();
       
        delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          pixels.setPixelColor(i+q, 0,0,0);        //turn every third pixel off
        }
    }
  }
  
  SetAll(0,0,0);
}

/* Random Effect */
void RandomEffect() 
{
  int pir_value = digitalRead(PIN_PIR);
  Serial.println(pir_value);
    
  if (pir_value == HIGH)  
    effect = random(1, 7);
  
  GenerateEffect();
}

/* Generate Effect */
void GenerateEffect() 
{
  if (irrecv.decode(&results)) 
  {  
    //Serial.println(results.value, HEX);  
    
    if (results.value == 0xE8829187)  // Button 1
      effect = 1;
    else if (results.value == 0x6FEFC667)  // Button 2
      effect = 2;
    else if (results.value == 0x60D500EB)  // Button 3
      effect = 3;
    else if (results.value == 0xAFB36707)  // Button 4
      effect = 4;
    else if (results.value == 0x960BF0B)  // Button 5
      effect = 7;
    else if (results.value == 0x5D60B0A3)  // Button 6
      effect = 6;
    else if (results.value == 0x4E45EB27)  // Button 7
      effect = 7;
    else if (results.value == 0x580E3F8B)  // Button 0
      effect = 0;
    
    irrecv.resume();
  }
  
  switch (effect)
  {
    case 1:
        RainbowCycle(200);
        break;
    case 2:
        Kitt(0xff, 0, 0, EYE_SIZE, SPEED_DELAY, RETURN_DELAY);
        break;
    case 3:
        RGBLoop();
        break;
    case 4:
        Sparkle(red, green, blue, 0);
        break;
    case 5:
        SnowSparkle(0x10, 0x10, 0x10, 50, 100);  
        break;
    case 6:
        RunningLights(red, green, blue, 50);
        break;
    case 7:
        TheaterChaseRainbow(35);
        break;
  }   
  //Serial.println(effect);  
  
}
