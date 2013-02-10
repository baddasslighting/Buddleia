#include <NewPing.h>
#include "SPI.h"
#include "Adafruit_WS2801.h"

/*
  Modified for Buddleia Feb 2013
  Christina Chu, www.thinklovecreate.com
  Tested on a short strand of 25, WS2801 http://www.adafruit.com/products/322

  Buddleia Notes, Project Website: 
  
  Tech details:
  120 addressable LEDs on a strip with WS2801 ICs 
  15 Ultrasonic sensors 
  
  120/15 = 8  = NUMBER OF SECTORS
    
  The 120 LEDs on this strip wrapped around a 10ft tall flower structure called Buddleia
  The flower structure is split into 15 sectors, each which has a single ultrasonic sensor 
  which acts as the trigger point for the initial point of a an led visual rippling effect. 
  
  Each sector has 8 LEDS in its region. The starting point will be the middle two leds in 
  every bank of 8 LEDS.
  
  For this initial version, and for the purpose of debugging, there will be different color
  associated with each of the 15 sensors. See array called sensor_colors
 
  Future versions can have randomized or specific color sequences as so desired. 
  
  Let N = Sonar sensor # from [1 .... 15]
  Let S = Number of Sectors
  
  Middle Upper LED Trigger point = 8*(N)-4 = S*(N)-Ceiling(S/2)
  Middle Lower LED Trigger point = 8*(N)-5 = S*(N)-Ceiling(S/2)+1

  EXAMPLE: 
  25 leds / 5 sectors  --> 
  Upper:  5*1 - roundUpToNearestInteger(5/2) = 2
  Lower:  5*1 - roundUpToNearestInteger(5/2)+1 = 3
  
  5*2-roundUP(5/2) = 7
  5*2-roundUP(5/2)+1 = 8
  
  Example: If Sonar # 3 is triggered, let N=3, 
  MULTp = 8(3)-4 = 20, 
  MLLTp = 8(3)-5 = 19

This means the pulse sequence will initiate at LED's # 19 and 20 and proceed in opposite
directions simultaneously:
  *) starting from LED # 20 and advancing to the end, LED #120
  *) starting from LED # 19 and advance to the opposite end, ending with LED #1

For Now, Strip colors coordinate with the N-1, where N is the sensor number. 
i.e. If sensor #1 is triggered, then sensor_colors[0] will be used

  sensor_colors[0] = 0xFF0000; //Bright Red
  
  Change the colors as you wish
  
 */
 // ---------------------------------------------------------------------------
// NEW PING 15
// This example code was used to successfully communicate with 15 ultrasonic sensors. You can adjust
// the number of sensors in your project by changing SONAR_NUM and the number of NewPing objects in the
// "sonar" array. You also need to change the pins for each sensor for the NewPing objects. Each sensor
// is pinged at 33ms intervals. So, one cycle of all sensors takes 495ms (33 * 15 = 495ms). The results
// are sent to the "oneSensorCycle" function which currently just displays the distance data. Your project
// would normally process the sensor results in this function (for example, decide if a robot needs to
// turn and call the turn function). Keep in mind this example is event-driven. Your complete sketch needs
// to be written so there's no "delay" commands and the loop() cycles at faster than a 33ms rate. If other
// processes take longer than 33ms, you'll need to increase PING_INTERVAL so it doesn't get behind.
// ---------------------------------------------------------------------------

#define SONAR_NUM     15 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define TRIGGER_DISTANCE 20  // MINIMUM DISTANCE TO TRIGGER SONAR SENSOR. RIGHT NOW IT IS SET to 20cm

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(41, 42, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(43, 44, MAX_DISTANCE),
  NewPing(45, 20, MAX_DISTANCE),
  NewPing(21, 22, MAX_DISTANCE),
  NewPing(23, 24, MAX_DISTANCE),
  NewPing(25, 26, MAX_DISTANCE),
  NewPing(27, 28, MAX_DISTANCE),
  NewPing(29, 30, MAX_DISTANCE),
  NewPing(31, 32, MAX_DISTANCE),
  NewPing(34, 33, MAX_DISTANCE),
  NewPing(35, 36, MAX_DISTANCE),
  NewPing(37, 38, MAX_DISTANCE),
  NewPing(39, 40, MAX_DISTANCE),
  NewPing(50, 51, MAX_DISTANCE),
  NewPing(52, 53, MAX_DISTANCE)
};
 
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels
// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

#define STRIP_LENGTH 120 // 120 LEDs on this strip
#define NUM_SECTORS 8    // # of sectors = STRIPLENGTH / SENSOR_NUMBER

//#define STRIP_LENGTH 25 // 25 LEDs on this strip
//#define NUM_SECTORS 5    // # of sectors = STRIPLENGTH / SENSOR_NUMBER

long sensor_colors[SONAR_NUM];

void setup() {
  
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
  
  // Pre-fill the color array with values for the sensors -
  // this is setup for the purpose of testing each sensor with a different color
  // - modify Color to your liking *****************
  sensor_colors[0] = 0x0000FF; //Bright Blue
  sensor_colors[1] = 0xFF0000; //Bright Red
  sensor_colors[2] = 0x00FF00; //Bright Green
  sensor_colors[3] = 0x8181F7; //Periwinkle
  sensor_colors[4] = 0xF2F5A9; //light yellow
  sensor_colors[5] = 0xFF00FF; // Purple
  sensor_colors[6] = 0xFFFF00; //Bright Yellow
 
  sensor_colors[7] = 0x61380B; // Brown 
  sensor_colors[8] = 0xFF8000; // Orange
  sensor_colors[9] = 0x80FF00; // lite green
  sensor_colors[10] = 0x00FFFF; // turqoise
  sensor_colors[11] = 0x0080FF; // aquamarine
  sensor_colors[12] = 0xBF00FF; // pinkish Purple
  sensor_colors[13] = 0xFF0080; // Puse
  sensor_colors[14] = 0xFFFFFF; // white

  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
  
  Serial.println("Hello Buddleia!");
}

void loop() {
  
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // The rest of your code would go here.
  
  
// colorTester();
 
}

void colorTester() { 
  //int currentSensor = 3; // test sensor number
  
  uint32_t c = sensor_colors[currentSensor];
  triggerColor(currentSensor, c);
  delay(1000);
    
  // default wipe strip to zero
  triggerColor(currentSensor, 0x000000);
  delay(1000);  
 
}


void triggerColor(int sensorNum, uint32_t c) { 

  int g = (int)ceil(NUM_SECTORS/2);  
  int idx = (NUM_SECTORS*sensorNum)-g+1; 
   
  // test sensor here
 //  Serial.print("G Value:");
 // Serial.println(g);    
  Serial.print("SENSOR number: "); 
  Serial.println(sensorNum);
  Serial.print("LED number: "); 
  Serial.println(idx);
  
  uint8_t wait = 20;
  int i;
  int z = idx-2;
  
  Serial.print("Number of pixels: ");
  Serial.println(strip.numPixels());
  
  for (i= 0; i < strip.numPixels(); i++) {
      if (idx+i < strip.numPixels()) {
        Serial.print("Increment PLUS : ");
        Serial.println(i+idx);      
        strip.setPixelColor(i+idx, c);
        strip.show();
        delay(wait);
      }

     int pix = strip.numPixels()-i-z;
     
     if (pix > -1) { 
        Serial.print("Increment MINUS : ");
        Serial.println(pix);      
        strip.setPixelColor(pix, c);
        strip.show();
        delay(wait);
     }
   }

 }


void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (cm[i] < TRIGGER_DISTANCE) { 
      Serial.print(i);
      Serial.print("=");
      Serial.print(cm[i]);
      Serial.print("cm ");
     
       // trigger lighting sequence here. 
              
        uint32_t c = sensor_colors[currentSensor];
        triggerColor(currentSensor, c);
        delay(1000);
    
        // default wipe strip to zero
        triggerColor(currentSensor, 0x000000);
        delay(1000);  
      
    }
  }
  Serial.println();
}


void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

