  Buddleia Feb 2013
  Christina Chu, www.thinklovecreate.com
  Tested on a short strand of 25, WS2801 http://www.adafruit.com/products/322
  Uses Adafruit WS2801, NewPing sensor library

  Tech details:
  120 addressable LEDs on a strip with WS2801 ICs 
  15 Ultrasonic sensors 
  
  The 120 LEDs on this strip wrapped around a 10ft tall flower structure called Buddleia
  The flower structure is split into 15 sectors, each which has a single ultrasonic sensor 
  which acts as the trigger point for the initial point of a an led visual rippling effect. 
  Each sector has 8 LEDS in its region. The starting point will be the middle two leds in 
  every bank of 8 LEDS. For this initial version, and for the purpose of debugging, there will be different color
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
  
This means the pulse sequence will initiate at LED's # 19 and 20 and proceed in opposite
directions simultaneously:
  *) starting from LED # 20 and advancing to the end, LED #120
  *) starting from LED # 19 and advance to the opposite end, ending with LED #1

For Now, Strip colors coordinate with the N-1, where N is the sensor number. 
i.e. If sensor #1 is triggered, then sensor_colors[0] will be used

  sensor_colors[0] = 0xFF0000; //Bright Red
