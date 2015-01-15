//#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>

#include "Timer.h"
//#include "TomerOne.h"


#define SLAVE_ADDRESS 0x04

#define PIN_LED_PWM        5
#define PIN_BTN_PWR        6
#define PIN_SERVO_PWM_X    9
#define PIN_SERVO_PWM_Y   10
#define PIN_MASTER_EN      7
#define PIN_MASTER_RX      3
#define PIN_MASTER_TX      4
#define PIN_MASTER_GPIO_0  2
#define PIN_MASTER_GPIO_1 11
#define PIN_SERVO_EN      12
#define PIN_INT_LED       13
#define PIN_LED_EN        A0
#define PIN_HOST_12V      A1


#define SERVO_PULSE_MIN  500
#define SERVO_PULSE_MAX 2400


enum SDevice {led, servo};
enum SType {en, pwm, pwm_x, pwm_y};

char i2c_ret;
volatile int led_en = 0;
volatile int led_pwm = 255;
volatile int servo_en = 0;
volatile int servo_pwm_x = 1480;
volatile int servo_pwm_y = 1150;
volatile int master_en = 0;
SDevice device;
SType type;
int get_bytes_c;

Servo servo_x, servo_y;
Timer timer;
//SoftwareSerial master_serial(PIN_MASTER_RX, PIN_MASTER_TX);


void setup() {
  int v_in;
  Serial.begin(9600);
  //master_serial.begin(9600);
  sssBegin();
  
  pinMode(PIN_INT_LED, OUTPUT);
  digitalWrite(PIN_INT_LED, HIGH);
  
  pinMode(PIN_HOST_12V, INPUT);
  //digitalWrite(PIN_HOST_12V, LOW);
  pinMode(PIN_SERVO_EN, OUTPUT);
  servo_x.attach(PIN_SERVO_PWM_X, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo_y.attach(PIN_SERVO_PWM_Y, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo_x.writeMicroseconds(servo_pwm_x);
  servo_y.writeMicroseconds(servo_pwm_y);
  v_in = digitalRead(PIN_HOST_12V);
  if (v_in == HIGH) {
    digitalWrite(PIN_SERVO_EN, HIGH);
  } else {
    digitalWrite(PIN_SERVO_EN, LOW);
  }
  
  pinMode(PIN_BTN_PWR, INPUT_PULLUP);
  digitalWrite(PIN_BTN_PWR, HIGH);
  
  pinMode(PIN_MASTER_GPIO_0, INPUT);
  digitalWrite(PIN_MASTER_GPIO_0, LOW);
  pinMode(PIN_MASTER_GPIO_1, INPUT);
  digitalWrite(PIN_MASTER_GPIO_1, LOW);
  if (digitalRead(PIN_MASTER_GPIO_0) == LOW) {
    master_en = 1;
    pinMode(PIN_MASTER_EN, OUTPUT);
    digitalWrite(PIN_MASTER_EN, master_en);
  } else {
    pinMode(PIN_MASTER_EN, OUTPUT);
    digitalWrite(PIN_MASTER_EN, LOW);
  }
  
  pinMode(PIN_LED_PWM, OUTPUT);
  analogWrite(PIN_LED_PWM, 127);
  
  pinMode(PIN_LED_EN, OUTPUT);
  digitalWrite(PIN_LED_EN, HIGH);
  
  delay(1000);
  
  digitalWrite(PIN_SERVO_EN, LOW);
  timer.every(125, pollingTimer);
  //Timer1.initialize(50000);
  //Timer1.pwm(9, 512); 
  //Timer1.attachInterrupt(pollingTimer);
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveI2c);
  Wire.onRequest(sendI2c);
  
  Serial.println("== SuperIO initialized ==");
  digitalWrite(PIN_LED_EN, led_en);
  analogWrite(PIN_LED_PWM, led_pwm);
  digitalWrite(PIN_INT_LED, master_en);
}


void loop()
{
  char c;
  //if (master_serial.available()) {
  if (sssAvailable()) {
     //c = master_serial.read();
     c = sssRead();
     Serial.write(c);
  }
  if (Serial.available()) {
    c = Serial.read();
    //master_serial.write(c);
    sssWrite(c);
  }
  
  timer.update();
}


#pragma Polling_Timer

void pollingTimer()
{
  static int flash_mode = 0, led_state, v_in, confirm = 40;
  static int btn_state, pwr_state, pwr_last = 1, is_reboot, is_reboot_try, counter = 0;
  
  //digitalWrite(PIN_HOST_12V, LOW);
  v_in = digitalRead(PIN_HOST_12V);
  if (v_in == HIGH) {
    if (flash_mode == 1) {
      confirm = 0;
      flash_mode = 0;
    }
    if (confirm == 2) {
      digitalWrite(PIN_LED_EN, LOW);
      led_state = 0;
      confirm = 3;
    } else if (confirm < 2){
      confirm += 1;
    }
  } else {
     if (flash_mode == 0) {
      confirm = 1;
      flash_mode = 1;
    }
    if (confirm == 2) {
      digitalWrite(PIN_INT_LED, HIGH);
      led_state = 0;
      confirm = 3;
    } else if (confirm < 2){
      confirm += 1;
    } else {
      if (led_state % 16  == 0) {
        digitalWrite(PIN_INT_LED, HIGH);
        led_state = 0;
      } else if (led_state % 8  == 0) {
        digitalWrite(PIN_INT_LED, LOW);
      }
      led_state ++;
    }
  }
  
  if (flash_mode == 0) {
    pwr_state = digitalRead(PIN_MASTER_GPIO_0);
    btn_state = digitalRead(PIN_BTN_PWR);
    if (pwr_state == 0) {
      is_reboot_try = digitalRead(PIN_MASTER_GPIO_1);
      pwr_state = digitalRead(PIN_MASTER_GPIO_0);
      if (pwr_state == 0) {
        is_reboot = is_reboot_try;
      }
    }
    if (pwr_state == 1 && pwr_last == 0) {
      if (is_reboot == 0) {
        master_en = 0;
      }
      is_reboot = 0;
    } else if (btn_state == 1) {
      if (counter > 1 && counter < 40) {
        master_en = digitalRead(PIN_MASTER_EN);
        if (master_en == 0) {
          master_en = 1;
          pwr_state == 1;
        }
      }
      digitalWrite(PIN_MASTER_EN, master_en);
      digitalWrite(PIN_INT_LED, master_en);
      counter = 0;
    } else {
      counter += 1;
      if (counter > 40) {
        counter = 40;
        master_en = 0;
        digitalWrite(PIN_MASTER_EN, master_en);
        digitalWrite(PIN_INT_LED, master_en);
      } else if (counter > 23) {
        if (counter % 2 == 0) {
          digitalWrite(PIN_INT_LED, LOW);
        } else {
          digitalWrite(PIN_INT_LED, HIGH);
        }
      }
    }
    pwr_last = pwr_state;
  }
}


#pragma I2c_Communication

void receiveI2c(int byte_count)
{
  static int input, need_flags ,more_flag;
  static int val_tmp_0, val_tmp_1, *val_tmp_ptr;
  static String tmp_str;
  
  i2c_ret    = 'N';
  need_flags = 0;
  more_flag  = 0;
  tmp_str = "";
  val_tmp_0 = -1;
  val_tmp_1 = -1;
  val_tmp_ptr = &val_tmp_0;
  
  while (Wire.available()) {
    input = Wire.read();
    if (input <= 32) {
      continue;
    }
    if (i2c_ret == 'F') {
      continue;
    }
    
    if (need_flags == 0) {
      if (toupper(input) == 'G') {
        i2c_ret = 'G'; // Get method.
        need_flags = 1;
      } else if (toupper(input) == 'S') {
        i2c_ret = 'S'; // Set method.
        need_flags = 1;
      } else {
        i2c_ret = 'F';
      }
    } else if (need_flags == 1) {
      if (toupper(input) == 'L') {
        device = led;
        need_flags = 2;
      } else if (toupper(input) == 'S') {
        device = servo;
        need_flags = 2;
      } else {
        i2c_ret = 'F';
      }
    } else if (need_flags == 2) {
      if (toupper(input) == 'E') {
        type = en;
        need_flags = 3;
      } else if (toupper(input) == 'P') {
        type = pwm;
        need_flags = 3;
      } else if (toupper(input) == 'X') {
        type = pwm_x;
        need_flags = 3;
      } else if (toupper(input) == 'Y') {
        type = pwm_y;
        need_flags = 3;
      } else {
        i2c_ret = 'F';
      }
    } else if (need_flags == 3) {
      if (isDigit(input)) {
        tmp_str += (char)input;
      } else if (input == ',') {
        val_tmp_0 = tmp_str.toInt();
        need_flags = 4;
        val_tmp_ptr = &val_tmp_1;
        tmp_str = "";
      } else {
        i2c_ret = 'F';
      }
    } else if (need_flags == 4) {
      if (isDigit(input)) {
        tmp_str += (char)input;
      } else {
        i2c_ret = 'F';
      }
    } else {
      i2c_ret = 'F';
    }
  }
  
  if (i2c_ret == 'F') {
    return;
  }
  
  *val_tmp_ptr = tmp_str.toInt();
  if (need_flags == 3) {
    if (device == led) {
      if (type == en) {
        if (i2c_ret == 'G') {
          get_bytes_c = 1;
        } else if (val_tmp_0 == 0 || val_tmp_0 == 1) {
          led_en = val_tmp_0;
          digitalWrite(PIN_LED_EN, led_en);
        } else {
          i2c_ret = 'F';
        }
      } else if (type == pwm) {
        if (i2c_ret == 'G') {
          get_bytes_c = 3;
        } else if (val_tmp_0 >= 0 && val_tmp_0 <= 255) {
          led_pwm = val_tmp_0;
          analogWrite(PIN_LED_PWM, led_pwm);
        } else {
          i2c_ret = 'F';
        }
      } else {
        i2c_ret = 'F';
      }
    } else if (device == servo) {
      if (type == en) {
        if (i2c_ret == 'G') {
          get_bytes_c = 1;
        } else if (val_tmp_0 == 0 || val_tmp_0 == 1) {
          servo_en = val_tmp_0;
          digitalWrite(PIN_SERVO_EN, servo_en);
        } else {
          i2c_ret = 'F';
        }
      } else if (type == pwm_x) {
        if (i2c_ret == 'G') {
          get_bytes_c = 4;
        } else if (val_tmp_0 > SERVO_PULSE_MIN && val_tmp_0 < SERVO_PULSE_MAX) {
          servo_pwm_x = val_tmp_0;
          servo_x.writeMicroseconds(servo_pwm_x);
        } else {
          i2c_ret = 'F';
        }
      } else if (type == pwm_y) {
        if (i2c_ret == 'G') {
          get_bytes_c = 4;
        } else if (val_tmp_0 > SERVO_PULSE_MIN && val_tmp_0 < SERVO_PULSE_MAX) {
          servo_pwm_y = val_tmp_0;
          servo_y.writeMicroseconds(servo_pwm_y);
        } else {
          i2c_ret = 'F';
        }
      } else {
        i2c_ret = 'F';
      }
    } else {
      i2c_ret = 'F';
    }
  } else if (need_flags == 4) {
    if (device == servo && type == pwm && i2c_ret == 'S') {
      if (val_tmp_0 > SERVO_PULSE_MIN && val_tmp_0 < SERVO_PULSE_MAX) {
        servo_pwm_x = val_tmp_0;
        servo_x.writeMicroseconds(servo_pwm_x);
      } else if (val_tmp_0 != -1) {
        i2c_ret = 'F';
      }
      if (val_tmp_1 > SERVO_PULSE_MIN && val_tmp_1 < SERVO_PULSE_MAX) {
        servo_pwm_y = val_tmp_1;
        servo_y.writeMicroseconds(servo_pwm_y);
      } else if (val_tmp_1 != -1) {
        i2c_ret = 'F';
      }
    } else {
      i2c_ret = 'F';
    }
  } else {
    i2c_ret = 'F';
  }
}


void sendI2c()
{
  static char ret_num;
  static int num_divider, i;
  if (i2c_ret == 'G') {
    Wire.write('R');
    i2c_ret = 'D';
  } else if (i2c_ret == 'D') {
    if (device == led) {
      Wire.write('L');
    } else if (device == servo) {
      Wire.write('S');
    } else {
      Wire.write('F');
    }
    i2c_ret = 'W';
  } else if (i2c_ret == 'W') {
    if (type == en) {
      Wire.write('E');
    } else if (type == pwm) {
      Wire.write('P');
    } else if (type == pwm_x) {
      Wire.write('X');
    } else if (type == pwm_y) {
      Wire.write('Y');
    } else {
      Wire.write('F');
    }
    i2c_ret = 'B';
  } else if (i2c_ret == 'B') {
    num_divider = 1;
    for (i = 1; i < get_bytes_c; i ++) {
      num_divider *= 10;
    }
    if (device == led) {
      if (type == en) {
        itoa((led_en / num_divider) % 10, &ret_num, 10);
      } else if (type == pwm) {
        itoa((led_pwm / num_divider) % 10, &ret_num, 10);
      } else {
        i2c_ret = '0';
      }
    } else if (device == servo) {
      if (type == en) {
        itoa((servo_en / num_divider) % 10, &ret_num, 10);
      } else if (type == pwm_x) {
        itoa((servo_pwm_x / num_divider) % 10, &ret_num, 10);
      } else if (type == pwm_y) {
        itoa((servo_pwm_y / num_divider) % 10, &ret_num, 10);
      } else {
        i2c_ret = '0';
      }
    }
    
    if (i2c_ret == '0') {
      Wire.write('F');
    } else {
      Wire.write(ret_num);
    }
    get_bytes_c --;
    
    if (get_bytes_c == 0) {
      i2c_ret = '0';
    }
  } else if (i2c_ret == 'S') {
    Wire.write('T');
    i2c_ret = '0';
  } else if (i2c_ret == 'F') {
    Wire.write('F');
    i2c_ret = '0';
  } else if (i2c_ret == '0') {
    Wire.write('\0');
  } else {
    Wire.write('F');
    i2c_ret = '0';
  }
}


#pragma Timer2_Software_Serial

// This file sss.ino is a very simple implementation of Software Serial intended for use on
//   an Arduino Uno.
//
// It is intended to be included in the same directory as the Arduino program that
//    uses it. It will then appear as a second tab in the Arduino IDE.
//
// This code is written for the Uno or an Atmega 328
//  it assumes a clock speed of 16MHz
//  it works at 9600 baud and probably also at 4800 and 19200
//  it is only designed for the 8N1 protocol
//
//  it can only Listen or Talk - not both at the same time
//
//  Rx is pin 3  (INT 1)
//  Tx is pin 4
//
//  The user functions are
//     char sssBegin()  
//     void sssEnd()
//     byte sssAvailable()             // returns number of bytes in buffer
//     int  sssRead()                  // returns next byte or -1 if buffer is empty
//     char sssWrite( byte sssInByte)  // sends 1 byte or returns -1 if it can't send
//                                     //   won't accept byte if previous send is not finished
//     char sssWrite( byte sssInByte[], byte sssNumInBytes)  // sends NumInBytes from byte array
//
//  Calling any other function will likely cause problems
//
//  Read and Write share the same buffer
//  A write will clear the buffer before writing
//  A first read after a write will clear the buffer before starting to listen
//
//  Arduino Resources used ...
//    Pin 3 and INT1  -- Rx
//    Pin 4              Tx
//    Timer2A
//    Timer2 prescaler


//=====================================
//    variables - not accessible by user

const int sssBaudRate = 9600;
const byte sssBufSize = 32;
const unsigned long sssIdleTimeoutMillis = 10;

const boolean sssdebugging = false; // controls printing of debug information

const byte sssRxPin = 3; // the pin for INT1
const byte sssTxPin = 4;

byte sssBufTail = 0;
volatile byte sssBufHead = 0;
volatile byte sssBuffer[sssBufSize]; // use same buffer for send and receive
byte sssBaudTimerCount;
byte sssBaudTimerFirstCount;

volatile byte sssBitCount = 0; // counts bits as they are received
volatile byte sssRecvByte = 0; // holder for received byte

volatile boolean sssIsListening = false;
boolean sssReady = false;

byte sssNumBytesToSend = 0;
byte sssBufferPos = 0; // keeps track of byte to send within the buffer
volatile byte sssNextByteToSend;
volatile byte sssNextBit;
volatile byte sssBitPos = 0; // position of bit within byte being sent

void (*sssTimerFunctionP)(void); // function pointer for use with ISR(TIMER2_COMPA_vect)

volatile unsigned long sssCurIdleMicros;
volatile unsigned long sssPrevIdleMicros;
unsigned long sssIdleIntervalMicros;


//=====================================
//    user accessible functions

//===========
char sssBegin() {

  sssDbg("Beginning  ", 0);

  sssStopAll();
  
  //  prepare Rx and Tx pins
  pinMode(sssRxPin, INPUT);
  pinMode(sssTxPin, OUTPUT);

  // prepare Timer2
  TCCR2A = B00000000; // set normal mode (not fast PWM)
                       // with fast PWM the clock resets when it matches
                       // and gives the wrong timing
                       
  TCCR2B = B00000011;  // set the prescaler to 32 - this gives counts at 2usec intervals
  
  // set baud rate timing
  sssBaudTimerCount = 1000000UL / sssBaudRate / 2; // number of counts per baud at 2usecs per count
  sssBaudTimerFirstCount = sssBaudTimerCount; // I thought a longer period might be needed to get into
                                              // the first bit after the start bit - but apparently not
  // normally an interrupt occurs after sssBaudTimerCount * 2 usecs (104usecs for 9600 baud)

  // length of required idle period to synchronize start bit detection
  sssIdleIntervalMicros = 1000000UL / 9600 * 25 ; // 2.5 byte lengths
  
  sssDbg("idleCount ", sssIdleIntervalMicros);
  sssDbg("baudTimer ",sssBaudTimerCount);
  
  sssPrepareToListen();
  sssReady = true;

}

//============

void sssEnd() {
  sssStopAll();
  sssReady = false;
}

//============

int sssAvailable() {

  if (! sssReady) {
     return -1;
  }

  if (sssNumBytesToSend > 0) { // only accept it if sender is idle
      return -1;
  }

  if (! sssIsListening) {
    char ok = sssPrepareToListen();
    if (! ok) {
      return -1;
    }
  }

  char sssByteCount = sssBufHead - sssBufTail;
  if (sssByteCount < 0) {
    sssByteCount += sssBufSize;
  }

  return sssByteCount;
}

//============

int sssRead() {

  if (! sssReady) {
     return -1;
  }

  if (sssNumBytesToSend > 0) { // only accept it if sender is idle
      return -1;
  }
  
  if (! sssIsListening) {
    char ok = sssPrepareToListen();
    if (! ok) {
      return -1;
    }
  }
  
  if (sssBufTail == sssBufHead) {
    return -1;
  }
  else {
    sssBufTail = (sssBufTail + 1) % sssBufSize;
    return sssBuffer[sssBufTail];
  }
}


//============

char sssWrite( byte sssInByte) { // User calls this

  if (! sssReady) {
     return -1;
  }

   if (sssNumBytesToSend > 0) { // only accept it if sender is idle
      return -1;
   }
   
   sssPrepareToTalk();
   
   sssBuffer[0] = sssInByte;
   sssBufferPos = 0;
   sssNumBytesToSend = 1; // this is the statement that triggers the ISR to send
                       //  so it should be the last thing here
   return 0;
}

//============

char sssWrite( byte sssInByte[], byte sssNumInBytes) { 

  if (! sssReady) {
     return -1;
  }

   if (sssNumBytesToSend > 0) { // only accept it if sender is idle
      return -1;
   }
   
   sssPrepareToTalk();

   if (sssNumInBytes > sssBufSize) {
      return -1;
   }
   
   for (byte n = 0; n < sssNumInBytes; n++) {
      sssBuffer[n] = sssInByte[n];
   }
   sssBufferPos = 0;
   sssNumBytesToSend = sssNumInBytes;
   
   return 0;
}


//============================
//     Internal Functions


//============

void sssStopAll() {
  TIMSK2 &= B11111100;  // Disable Timer2 Compare Match A Interrupt
  EIMSK &=  B11111101;  // Disable Interrupt 1

  sssIsListening = false;
}

//============

char sssPrepareToListen() {

  sssStopAll();
  
// check for idle period
  unsigned long sssIdleDelay = 1000000UL / sssBaudRate / 2; // usecs between checks - half the baud interval
  unsigned long sssStartMillis = millis();
  byte sssIdleCount = 0;
  byte sssMinIdleCount = 44; // 20 bits checked at half the bit interval plus a little extra
  
  sssDbg("idleDelay ", sssIdleDelay);
  sssDbg("baudTimer ",sssBaudTimerCount);
  
  sssDbg("PIND ", PIND & B00001000);
    
  while (millis() - sssStartMillis < sssIdleTimeoutMillis) {
    sssIdleCount ++;
    if ( ! (PIND & B00001000) ) {
     sssIdleCount = 0; // start counting again
    }
    //  if a valid idle period is found
    if (sssIdleCount >= sssMinIdleCount) {
      // reset receive buffer
      sssBufHead = 0;
      sssBufTail = 0;
      // set startBit interrupt
      EICRA |= B0001000; // External Interrupt 1 FALLING
      EIFR |=  B00000010; // clear interrupt 1 flag - prevents any spurious carry-over
      EIMSK |= B0000010; // enable Interrupt 1
      
      sssTimerFunctionP = &sssTimerGetBitsISR;

      sssIsListening = true;
    }
    delayMicroseconds(sssIdleDelay);
  }
  sssDbg("LISTENING?  ", sssIsListening);
  return sssIsListening;
}

//============

void sssPrepareToTalk() {

  sssDbg("Preparing to Talk", 0);

  sssStopAll();
  
  // get stuff ready
  digitalWrite(sssTxPin, HIGH); // high = idle
  
  // set the appropriate interrupt function
  sssTimerFunctionP = &sssSendNextByteISR;
  
  // start the bitInterval timer
  noInterrupts();
    OCR2A = TCNT2 + sssBaudTimerCount;
    TIMSK2 &= B11111100; // Disable compareMatchA and Overflow                                     
    TIFR2  |= B00000010;  // Clear  Timer2 Compare Match A Flag - not sure this is essential
    TIMSK2 |= B00000010;  // Enable Timer2 Compare Match A Interrupt
  interrupts();
  
}


//============

void sssDbg( char dbgStr[], unsigned long dbgVal) {
  if (sssdebugging) {
    Serial.print(dbgStr);
    Serial.println(dbgVal);
  }
}

//=====================================
//       Interrupt Routines


ISR(INT1_vect) {

   // this is called when a start bit is detected
  
  // turn off the startBit interrupt
  EIMSK &=  B11111101;  // Disable Interrupt 1
  
  // start the bitInterval timer
  OCR2A = TCNT2 + (sssBaudTimerFirstCount); // sets up Timer2A to run for the 1.5 times the baud period
                                       //   for the first sample after the start bit
                                       //   the idea is to take the samples in the middle of the bits
  TIMSK2 &= B11111100; // Disable compareMatchA and Overflow                                     
  TIFR2  |= B00000010;  // Clear  Timer2 Compare Match A Flag - not sure this is essential
  TIMSK2 |= B00000010;  // Enable Timer2 Compare Match A Interrupt
    
  // set counters and buffer index
  sssBitCount = 0;
  sssRecvByte = 0;
  
}


//=============

ISR(TIMER2_COMPA_vect) {
  // this is called by the bitInterval timer
  sssTimerFunctionP(); // this is a function pointer and will call different functions
                       // Points to one of sssTimerGetBitsISR()
                       //                  sssSendNextByteISR()
                       //                  sssSendBitsISR()
}


//=============

void sssTimerGetBitsISR() {
  // This is one of the functions called by TimerFunctionP()
  // read the bit value first
  byte sssNewBit = PIND & B00001000; // Arduino Pin 3 is pin3 in Port D
  
  // update the counter for the next bit interval
  OCR2A = TCNT2 + sssBaudTimerCount;
  
  if (sssBitCount == 8) {   // this is the stop bit
     // stop the bitIntervalTimer
     TIMSK2 &= B11111100;  // Disable Timer2 Compare Match A  and overflow Interrupts
     // enable Interrupt 0
     EIFR |= B00000010; // clear interrupt 1 flag - prevents any spurious carry-over
     EIMSK |=  B00000010;  // Enable Interrupt 1 to detect the next start bit
  }

  sssNewBit = sssNewBit >> 3; // gets the bit into posn 0
  sssNewBit = sssNewBit << sssBitCount; // moves it to the correct place
  sssRecvByte += sssNewBit;


  if (sssBitCount == 7) {
     // update the bufHead index
     // but prevent overwriting the tail of the buffer
     byte sssTestHead = (sssBufHead + 1) % sssBufSize;
     if (sssTestHead != sssBufTail) { // otherwise sssBufHead is unchanged
       sssBufHead = sssTestHead;
     }
     // and save the byte
     sssBuffer[sssBufHead] = sssRecvByte;
  }
  
  sssBitCount ++;

}

//============


void sssSendNextByteISR() {

    // this all happens while the line is idle

    OCR2A = TCNT2 + sssBaudTimerCount; 

    if (sssNumBytesToSend > 0) {
    
      sssNextByteToSend = sssBuffer[sssBufferPos]; 

      sssBitPos = 0;
      
      noInterrupts();
        sssTimerFunctionP = &sssSendBitsISR; // switch interrupt to the sendBits function
      interrupts();
    }
    // otherwise let interrupt keep checking if there are bytes to send
}

//===========

void sssSendBitsISR() {

     OCR2A = TCNT2 + sssBaudTimerCount;

     if (sssBitPos == 9) { // this is the  stop bit
       PORTD |= B00010000;
       
       noInterrupts();
         sssTimerFunctionP = &sssSendNextByteISR; // switch interrupt to the sendNextByte function
       interrupts();
       
       sssBufferPos ++; // update the byte index
       sssNumBytesToSend --;
       
     }
     else if (sssBitPos > 0) {
         sssNextBit = sssNextByteToSend & B00000001;
     
         if (sssNextBit) {
            PORTD |= B00010000;
         }
         else {
            PORTD &= B11101111;
         }

         sssNextByteToSend = sssNextByteToSend >> 1;
         sssBitPos ++;
     }
     else { // this is the start bit
        PORTD &= B11101111;
        sssBitPos ++;
     }
}


//========END=============



