/*
   Red Hat Badge 2018 - Now with blinking!
   Badge turns on all 3 LEDs. If another badge is detected, begin flashing.
   August 5th, 2018
   By Alicia Gibb @ Lunchbox Electronics and Nathan Seidle @ SparkFun Electronics
   
   Once assembled a badge is on all the time, but if it sees another badge
   both badges start the blinky dance!
   
   9mA average current with 1ks on the red LEDs. CR2032 = roughly 220mAh.
   So should last for 20+ hours

   IR libraries are too big and/or break delay() or other timers. So we bit bang 
   the IR transmission.
*/

const byte LED1 = 0;
const byte IRLED = 1;
const byte RECV_PIN = 3;
const byte LED2 = 2;
const byte LED3 = 4;

#define MODE_SPARKLE 1
#define MODE_ON 2

byte currentMode = MODE_ON;
byte loopCounter = 0; //Counts the number of times we do the main loop. Assume it's 1s per loop

void setup()
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(IRLED, OUTPUT);
  pinMode(RECV_PIN, INPUT_PULLUP);

  bootHello(); //Blink a certain way at startup
  
  loopCounter = 0;
  currentMode = MODE_ON;
}

void loop() {

  //Depending on the mode, make the LEDs dance
  if (currentMode == MODE_SPARKLE)
    blinkHappyDance();
  else
    turnLEDsOn();

  //If we have no more IR from another badge for 3s, then go to standby/always on mode
  if (loopCounter++ > 3)
  {
    currentMode = MODE_ON;
    loopCounter = 4; //Force a non-overflow
  }

  delay(5); //Without a delay the RX will detect itself

  //If we detect any IR code then sparkle the LEDs!
  //Scan for max of 500ms
  for (byte x = 0 ; x < 5 ; x++)
  {
    if (irIncoming() == true)
    {
      currentMode = MODE_SPARKLE;
      loopCounter = 0; //Reset the loop/timeout
      break; //We have something! Leave the while loop
    }
  }

  //Blink the IR LED to announce our existance
  sendIR(0xAA);
  delay(5); //Without a delay the RX will detect itself
}

//Blink LEDs in chasing pattern
void blinkHappyDance()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  byte chaseDelay = 50;

  for (byte x = 0 ; x < 3 ; x++)
  {
    digitalWrite(LED1, HIGH);
    delay(chaseDelay);
    digitalWrite(LED1, LOW);

    digitalWrite(LED2, HIGH);
    delay(chaseDelay);
    digitalWrite(LED2, LOW);

    digitalWrite(LED3, HIGH);
    delay(chaseDelay);
    digitalWrite(LED3, LOW);

    //While we're dancing, blink IR LED to let the world know of our existance
    sendIR(0xAA);
  }
}

//Blink LEDs fast and simultaneously
void bootHello()
{
  byte chaseDelay = 50;

  for (byte x = 0 ; x < 3 ; x++)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    delay(chaseDelay);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }
}

//Turn the 3 LEDs on - this is the normal mode when no other badge is present
void turnLEDsOn()
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
}

//Checks to see if a character is coming in
//For 100ms, look for receiver to go low
boolean irIncoming()
{
  int counter = 0;
  while (1)
  {
    if (counter++ == 100) return (false);
    if (digitalRead(RECV_PIN) == LOW) return (true); //We see something!
    delay(1);
  }
}

//Sends a thing at 32kHz = 31.25ms per pulse
//At 8*8*10 = 6400, the receiver sees a 0 with low of 17.1ms
void sendIR(byte thingToSend)
{
  for (int x = 0 ; x < 8 * 8 * 10 ; x++)
  {
    digitalWrite(IRLED, HIGH);
    __asm__("nop\n\t");
    digitalWrite(IRLED, LOW);
    __asm__("nop\n\t");
  }
}

