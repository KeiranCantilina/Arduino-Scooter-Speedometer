#include <TM1637Display.h>

//Definitions for display library
#define CLK 3
#define DIO 4

//Invoke display library
TM1637Display display(CLK, DIO);

//Variable declaration
int ledPin = 13;                //IR LED connected to digital pin 13
int powerpin = 11;              //Pin for powering display (cause I ran out of power bus pins)
int groundpin = 10;             //pin for grounding display

volatile boolean boolean_flag;
volatile long int time1;
volatile long int time2;
volatile long int time_delay;

float mph;

//Interrupt function
void triggered_interrupt() {
  boolean_flag = !boolean_flag; //Flip flag every time interrupt is triggered

  if (boolean_flag == true) {
    time1 = micros();
  }

  if (boolean_flag == false) {
    time2 = micros();
    time_delay = time2-time1;
  }
}


void setup() {

  //Initialize interrupt pin with internal pullup cause otherwise pin doesn't pull to 5V rail properly
  pinMode(2, INPUT_PULLUP);

  //Turns on IR LED
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, HIGH);

  //Configure pins for display
  pinMode(powerpin,OUTPUT);
  digitalWrite(powerpin, HIGH);
  pinMode(groundpin, OUTPUT);
  digitalWrite(groundpin, LOW);
  display.setBrightness(0x0f);

  //Sets default flag states
  boolean_flag = false;
  time_delay = 0;
  
}


void loop() {

  //Only look for interrupts in half second periods
  attachInterrupt(0, triggered_interrupt, FALLING);               //Using interrupt 0 (which is actually pin D2)
  delay(500);
  detachInterrupt(0);

  //If time_delay hasn't changed from default, assume negligible movemement
  if (time_delay == 0) {
    display.showNumberDecEx(0, (0x80 >> 1), true, 4, 0);          //Otherwise poorly documented syntax for display function is (ValueToDisplay, bitshift assigment to turn on decimal point, DoYouWantLeadingZeroes, HowManyDigits, DigitOffset)
  }

  //If time_delay has changed from default, assume movement
  if (time_delay > 0){
    mph = 996625.319/time_delay;                                  //Calculation of speed based on estimated wheel circumference; adjust if needed
    display.showNumberDecEx(mph*100, (0x80 >> 1), true, 4, 0);    //Display library doesn't like floating values, hence the "*100" and "0" offset
  }

  //Reset Variables
  boolean_flag = false;
  time_delay = 0;
}
