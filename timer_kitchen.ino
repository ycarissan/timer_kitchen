#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DEBUG 1

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//
//     Temps
//
int volatile secondes=120;

//
//  Affichage
//
boolean volatile updateScreen = false;

//
//   Buzzer
//
int buzzerPin = 6;

void setup()
{
  pinMode(buzzerPin, OUTPUT);
//  tone(buzzerPin, 1000, 500);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Initialisation");
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Initialisation");
  lcd.print(".");
  lcd.setCursor(0,1);
  printTime();
  cli();          // turn interrupts off
                  // set timer1 interrupt at 1Hz
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  TCNT1  = 0;     // initialize counter value to 0
                  // set compare match register for 1hz increments
  OCR1A = 15624;  // = (16*10^6) / (1*1024) - 1 (must be <65536)
                  // turn on CTC mode
  TCCR1B |= (1 << WGM12);               // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();           //allow interrupts
  

}

void loop()
{
  if (secondes<0) {
    TCCR1B = 0;//stop timer
    updateScreen = false;
    secondes=0;
  beep();
  delay(500);
  beep();
  delay(500);
  beep();

  }
  if (updateScreen) {
    printTime();
    updateScreen = false;
  }
}

void printTime() {
  if (secondes<0) return;
  char msg[9];
  int hh = secondes / 3600;
  int mm = (secondes - 3600*hh) / 60;
  int ss = secondes - 3600*hh - 60*mm;
  sprintf(msg,"%02d:%02d:%02d",hh,mm,ss);
  lcd.setCursor(0,1);
  lcd.print(msg);
#ifdef DEBUG
  Serial.print("printTime : ");
  Serial.println(msg);
#endif
}

void beep() {
  tone(buzzerPin,2200); // then buzz by going high
    tone(buzzerPin,1000);
    tone(buzzerPin,500);
    tone(buzzerPin,200);
    tone(buzzerPin,500);
    delayMicroseconds(10000);    // waiting
    noTone(buzzerPin);  // going low
    delayMicroseconds(10000);    // and waiting more
    tone(buzzerPin,2200);
    tone(buzzerPin,1000);
    delayMicroseconds(10000);    // waiting
    noTone(buzzerPin);  // going low
    delayMicroseconds(10000);    // and waiting more
    tone(buzzerPin,100);
    delayMicroseconds(10000);    // waiting
    noTone(buzzerPin);  // going low
    delayMicroseconds(10000);    // and waiting more
    tone(buzzerPin,100);
    delayMicroseconds(10000);    // waiting
    noTone(buzzerPin);  // going low
    delayMicroseconds(10000);    // and waiting more
}

ISR(TIMER1_COMPA_vect){   //timer1 interrupt 1Hz
  secondes = secondes-1;
  updateScreen = true;
}
