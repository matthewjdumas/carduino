#define DEBOUNCE 10 
enum inputs { lightPin = 22, lBlinkerPin = 23, rBlinkerPin = 24, brakePin = 25, ignitionPin = 26, accPin = 27 };
enum outputs { headlights = 31, lBlink = 33, rBlink = 34, lStop = 35, rStop = 36, ignition = 37, acc = 38} ;

byte buttons[] = { lightPin, lBlinkerPin, rBlinkerPin, brakePin, ignitionPin, accPin};
#define NUMBUTTONS sizeof(buttons)
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

bool lBlinkState = false; 
bool lBlinker = false;
bool rBlinkState = false;
bool rBlinker = false;
bool lightsState = false; 
bool accState = false; 
bool brakeState = false;
bool ignitionState = false;


void setup() {
  // put your setup code here, to run once:

  // OUTPUTS pinout: 
  //     Headlights:  Pin 31
  //     L Blinker :  Pin 33
  //     R Blinker :  Pin 34
  //     L Stop    :  Pin 35
  //     R Stop    :  Pin 36

  //     Ignition  :  Pin 37
  //     ACC       :  Pin 38

 
  
  pinMode(headlights, OUTPUT);
  pinMode(lBlink, OUTPUT);
  pinMode(rBlink, OUTPUT);
  pinMode(lStop, OUTPUT);
  pinMode(rStop, OUTPUT);
  pinMode(ignition, OUTPUT);
  pinMode(acc, OUTPUT);


  // INPUTS Pinout: 
  //       headlights    :  Pin 22
  //       left blinker  :  Pin 23
  //       right blinker :  Pin 24
  //       stop          :  Pin 25
  //       ignition      :  Pin 26
  //       acc           :  Pin 27




  
  for (int i=0; i<NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3  = 0;

  OCR3A = 62500;            // compare match register 16MHz/256/2Hz
  TCCR3B |= (1 << WGM12);   // CTC mode
  TCCR3B |= (1 << CS12);    // 256 prescaler 
  TIMSK3 |= (1 << OCIE3A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}
ISR(TIMER3_COMPA_vect)          // timer compare interrupt service routine
{
   if (lBlinkState) { 
      lBlinker = !lBlinker;
      digitalWrite(lBlink, lBlinker);
      digitalWrite(lStop, lBlinker);
   }
   if (rBlinkState) {
      rBlinker = !rBlinker;
      digitalWrite(rBlink, rBlinker);
      digitalWrite(rStop, rBlinker);
   }
}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis()) { // we wrapped around, lets just try again
     lasttime = millis();
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  
  for (index = 0; index < NUMBUTTONS; index++) { // when we start, we clear out the "just" indicators
    justreleased[index] = 0;
     
    currentstate[index] = digitalRead(buttons[index]);   // read the button

    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
          // just pressed
          justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
          // just released
          justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}

void loop() {
  check_switches();      // when we check the switches we'll get the current state
  
  for (byte i = 0; i < NUMBUTTONS; i++) {
    if (justpressed[i]) {  
      
    }

    //enum outputs { headlights = 31, lBlink = 33, rBlink = 34, lStop = 35, rStop = 36, ignition = 37, acc = 38} ;
    // enum inputs { lightPin = 22, lBlinkerPin = 23, rBlinkerPin = 24, brakePin = 25, ignitionPin = 26, accPin = 27 };
    if (justreleased[i]) {  // This is for blinkers, light toggle, ACC
       switch(i) {
          case lightPin: 
            lightsState = !lightsState; 
            digitalWrite(headlights, lightsState);
            break;
          case accPin: 
            accState = !accState; 
            digitalWrite(acc, accState);
            break;
          case lBlinkerPin:
            lBlinkState != lBlinkState;
            if (!lBlinkState) {
              digitalWrite(lBlink, lBlinkState);
              digitalWrite(lStop, lightsState);
            }
            break;
          case rBlinkerPin:
            rBlinkState != rBlinkState;
            if (!rBlinkState) {
              digitalWrite(rBlink, rBlinkState);
              digitalWrite(rStop, lightsState);
            }
            break;

          case brakePin:
            brakeState = false; 
            digitalWrite(lStop, brakeState);
            digitalWrite(rStop, brakeState);
            break;

          case ignitionPin: 
            ignitionState = false;
            digitalWrite(ignition, ignitionState);
            break;
            
          default:
            break;
       }
    }
    if (pressed[i]) {  // brake lights and ignition
      switch (i) {
          case brakePin:
            brakeState = true;
            digitalWrite(lStop, brakeState);
            digitalWrite(rStop, brakeState);
            break;

          case ignitionPin:
            ignitionState = true;
            digitalWrite(ignition, ignitionState);
            break;

          default:
             break;
      }
      // is the button pressed down at this moment
    }
  }

}
