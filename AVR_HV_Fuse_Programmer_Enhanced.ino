// Greenonline - Reinstated buzzer and power LED from Ralph Bacon's code
//             - Replaced #define with const byte, or const int.
//
// AVR High-voltage Serial Fuse Reprogrammer
//
// Gadget Reboot - modified fuse programming options in menu system
//               - changed 12 volt reset circuit and control logic
//               - https://github.com/GadgetReboot/Arduino
//
// This version adapted from the version by Ralph Bacon
// https://github.com/RalphBacon/ATTiny85_Fuse_Resetter
// Other History:
// Adapted from code and design by Paul Willoughby 03/20/2010
// http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/
// Inspired by Jeff Keyzer mightyohm.com
// Serial Programming routines from ATtiny25/45/85 datasheet
//
// Fuse Calc:
//   http://www.engbedded.com/fusecalc/

// Comment this line out if you only wish to use the single transistor implementation of the RESET circuit
#define GADGETREBOOT2TRANSISTOR_12V
// Comment this line out if you only wish to use the single transistor implementation of the VCC circuit - leave in place for direct Vcc power from digital pin
#define GADGETREBOOT2TRANSISTOR_VCC

//#define  SCI     12    // Target Clock Input
//#define  SDO     11    // Target Data Output
//#define  SII     10    // Target Instruction Input
//#define  SDI      9    // Target Data Input
//#define  VCC      8    // Target VCC
//#define  RST      7    // Output to control 12V prog voltage to target Reset pin
const byte SCI   =  12;    // Target Clock Input
const byte SDO   =  11;    // Target Data Output
const byte SII   =  10;    // Target Instruction Input
const byte SDI   =   9;    // Target Data Input
const byte VCC   =   8;    // Target VCC
const byte RST   =   7;    // Output to control 12V prog voltage to target Reset pin

// required for accessing device fuse locations
//#define  LFUSE  0x646C
//#define  HFUSE  0x747C
//#define  EFUSE  0x666E
const int  LFUSE = 0x646C;
const int  HFUSE = 0x747C;
const int  EFUSE = 0x666E;

// Digispark default fuses
//#define  LVAL1  0xE1
//#define  HVAL1  0x5D
//#define  EVAL1  0xFE
const byte  LVAL1 = 0xE1;
const byte  HVAL1 = 0x5D;
const byte  EVAL1 = 0xFE;

// Digispark fuses with Reset pin enabled
//#define  LVAL2  0xE1
//#define  HVAL2  0xDD
//#define  EVAL2  0xFE
const byte LVAL2 = 0xE1;
const byte HVAL2 = 0xDD;
const byte EVAL2 = 0xFE;


byte targetHVal = 0;
byte targetLVal = 0;
byte targetEVal = 0;
bool readFuseOnly = true;  // just reading, not writing

// Define ATTiny series signatures
//#define  ATTINY13   0x9007  // L: 0x6A, H: 0xFF             8 pin
//#define  ATTINY24   0x910B  // L: 0x62, H: 0xDF, E: 0xFF   14 pin
//#define  ATTINY25   0x9108  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
//#define  ATTINY44   0x9207  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
//#define  ATTINY45   0x9206  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
//#define  ATTINY84   0x930C  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
//#define  ATTINY85   0x930B  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
const int  ATTINY13 =  0x9007;  // L: 0x6A, H: 0xFF             8 pin
const int  ATTINY24 =  0x910B;  // L: 0x62, H: 0xDF, E: 0xFF   14 pin
const int  ATTINY25 =  0x9108;  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
const int  ATTINY44 =  0x9207;  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
const int  ATTINY45 =  0x9206;  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
const int  ATTINY84 =  0x930C;  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
const int  ATTINY85 =  0x930B;  // L: 0x62, H: 0xDF, E: 0xFF    8 pin

// Added by Ralph S Bacon just for enhanced user experience - modified by Greenonline
//#define	 BUZZ	    4		// Beeper pin (was pin 7)
//#define  BUZZ_GND	    5		// GND for buzzer
//#define  PWRLED	  6		// Power on LED
const byte BUZZ	= 4;		// Beeper pin (was pin 7)
const byte BUZZ_GND = 5;		// GND for buzzer
const byte PWRLED = 6;		// Power on LED

int inData = 0;         // incoming serial byte AVR // n/u
int targetValue = HFUSE;  // n/u

void setup() {
  Serial.begin(9600);

  // configure programming pins
  #ifdef GADGETREBOOT2TRANSISTOR_12V
    digitalWrite(RST, LOW);   // 12V Off - GadgetReboot circuit (two transistors)
  #else
    digitalWrite(RST, HIGH);  // 12V Off  - Bacon circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_12V */
  #ifdef GADGETREBOOT2TRANSISTOR_VCC
    digitalWrite(VCC, LOW);   // Vcc Off  - GadgetReboot circuit (two transistors), or direct
  #else
    digitalWrite(VCC, HIGH);  // Vcc Off  - GadgetReboot circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_VCC */
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  digitalWrite(SDO, LOW);
  pinMode(VCC, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(SII, OUTPUT);
  pinMode(SCI, OUTPUT);
  pinMode(SDO, OUTPUT);     // configured as input when in programming mode
  
  // RSB "enhancements"
	// Sink the Buzzer to ground
	pinMode(BUZZ_GND, OUTPUT);
	digitalWrite(BUZZ_GND, LOW);

	// Turn on power LED
	pinMode(PWRLED, OUTPUT);

	// Buzzer / beeper
	pinMode(BUZZ, OUTPUT);
	digitalWrite(PWRLED, HIGH);

	// Let user know we're ready to go
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);
	// End of RSB

}

void loop() {

  switch (getCommand()) {
    case '0':
      readFuseOnly = true;
      break;
    case '1':
      targetHVal = HVAL1;
      targetLVal = LVAL1;
      targetEVal = EVAL1;
      readFuseOnly = false;
      break;
    case '2':
      targetHVal = HVAL2;
      targetLVal = LVAL2;
      targetEVal = EVAL2;
      readFuseOnly = false;
      break;
    default:
      readFuseOnly = true;
  }

  /*
    From ATTiny85 Data Sheet
    ------------------------
    20.7.1 Enter High-voltage Serial Programming Mode

    The following algorithm puts the device in High-voltage Serial Programming mode:
    1. Set Prog_enable pins listed in Table 20-14 to “000”, RESET pin and VCC to 0V.
    2. Apply 4.5 - 5.5V between VCC and GND. Ensure that VCC reaches at least 1.8V within the next 20 µs.
    3. Wait 20 - 60 µs, and apply 11.5 - 12.5V to RESET.
    4. Keep the Prog_enable pins unchanged for at least 10 µs after the High-voltage has been applied to
    ensure the Prog_enable Signature has been latched.
    5. Release the Prog_enable[2] pin to avoid drive contention on the Prog_enable[2]/SDO pin.
    6. Wait at least 300 µs before giving any serial instructions on SDI/SII.
    7. Exit Programming mode by power the device down or by bringing RESET pin to 0V.

    If the rise time of the VCC is unable to fulfill the requirements listed above, the following alternative algorithm can be
    used:
    1. Set Prog_enable pins listed in Table 20-14 to “000”, RESET pin and VCC to 0V.
    2. Apply 4.5 - 5.5V between VCC and GND.
    3. Monitor VCC, and as soon as VCC reaches 0.9 - 1.1V, apply 11.5 - 12.5V to RESET.
    4. Keep the Prog_enable pins unchanged for at least 10 µs after the High-voltage has been applied to
    ensure the Prog_enable Signature has been latched.
    5. Release the Prog_enable[2] pin to avoid drive contention on the Prog_enable[2]/SDO pin.
    6. Wait until VCC actually reaches 4.5 - 5.5V before giving any serial instructions on SDI/SII.
    7. Exit Programming mode by power the device down or by bringing RESET pin to 0V
  */

  // Future enhancement:  monitor VCC with analog input and follow protocol more directly
  //                      for now, 100uS works to allow VCC to rise on Digispark board
  //                      powered by a high side P-ch FET switch

  // power up target device in high voltage programming mode
  pinMode(SDO, OUTPUT);    // Set SDO to output
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  digitalWrite(SDO, LOW);
  #ifdef GADGETREBOOT2TRANSISTOR_12V
    digitalWrite(RST, LOW);   // 12V Off - GadgetReboot circuit (two transistors)
  #else
    digitalWrite(RST, HIGH);  // 12V Off - Bacon circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_12V */
  #ifdef GADGETREBOOT2TRANSISTOR_VCC
    digitalWrite(VCC, HIGH);  // Vcc On  - GadgetReboot circuit (two transistors), or direct
  #else
    digitalWrite(VCC, LOW);   // Vcc On  - GadgetReboot circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_VCC */	
  delayMicroseconds(100);  // Ensure VCC has reached at least 1.1v before applying 12v to reset
  #ifdef GADGETREBOOT2TRANSISTOR_12V
    digitalWrite(RST, HIGH);  // 12V On  - GadgetReboot circuit (two transistors)
  #else
    digitalWrite(RST, LOW);   // 12V On  - Bacon circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_12V */
  delayMicroseconds(10);
  pinMode(SDO, INPUT);     // Set SDO to input
  delayMicroseconds(300);  // Ensure VCC has reached at least 4.5v before issuing instructions

  unsigned int sig = readSignature();
  Serial.print("Signature: ");
  Serial.println(sig, HEX);
  Serial.println("Reading Fuses...");
  readFuses();

  if (!readFuseOnly) {
    Serial.println("Writing Fuses");

    writeFuse(LFUSE, targetLVal);
    writeFuse(HFUSE, targetHVal);
    writeFuse(EFUSE, targetEVal);

    Serial.println("Reading Fuses...");
    readFuses();
  }

  digitalWrite(SCI, LOW);
  #ifdef GADGETREBOOT2TRANSISTOR_12V
    digitalWrite(RST, LOW);  // 12V Off - GadgetReboot circuit (two transistors)
  #else
    digitalWrite(RST, HIGH);  // 12V Off  - Bacon circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_12V */

  delayMicroseconds(40);
  #if GADGETREBOOT2TRANSISTOR_VCC
    digitalWrite(VCC, LOW);     // Vcc Off - GadgetReboot circuit (two transistors), or direct
  #else
    digitalWrite(VCC, HIGH);    // Vcc Off - GadgetReboot circuit (one transistor)
  #endif /* GADGETREBOOT2TRANSISTOR_VCC */  
  // Let user know we're done - RSB
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);
	delay(50);
	digitalWrite(BUZZ, HIGH);
	delay(50);
	digitalWrite(BUZZ, LOW);
  // End of RSB

}

byte shiftOut (byte val1, byte val2) {
  int inBits = 0;
  //Wait until SDO goes high
  while (!digitalRead(SDO))
    ;
  unsigned int dout = (unsigned int) val1 << 2;
  unsigned int iout = (unsigned int) val2 << 2;
  for (int ii = 10; ii >= 0; ii--)  {
    digitalWrite(SDI, !!(dout & (1 << ii)));
    digitalWrite(SII, !!(iout & (1 << ii)));
    inBits <<= 1;
    inBits |= digitalRead(SDO);
    digitalWrite(SCI, HIGH);
    digitalWrite(SCI, LOW);
  }
  return inBits >> 2;
}

void writeFuse (unsigned int fuse, byte val) {
  shiftOut(0x40, 0x4C);
  shiftOut( val, 0x2C);
  shiftOut(0x00, (byte) (fuse >> 8));
  shiftOut(0x00, (byte) fuse);
}

void readFuses () {
  byte val;

  shiftOut(0x04, 0x4C);  // LFuse
  shiftOut(0x00, 0x68);
  val = shiftOut(0x00, 0x6C);
  Serial.print("LFuse:");
  Serial.print(val, HEX);

  shiftOut(0x04, 0x4C);  // HFuse
  shiftOut(0x00, 0x7A);
  val = shiftOut(0x00, 0x7E);
  Serial.print(", HFuse: ");
  Serial.print(val, HEX);

  shiftOut(0x04, 0x4C);  // EFuse
  shiftOut(0x00, 0x6A);
  val = shiftOut(0x00, 0x6E);
  Serial.print(", EFuse: ");
  Serial.println(val, HEX);
}

unsigned int readSignature () {
  unsigned int sig = 0;
  byte val;
  for (int ii = 1; ii < 3; ii++) {
    shiftOut(0x08, 0x4C);
    shiftOut(  ii, 0x0C);
    shiftOut(0x00, 0x68);
    val = shiftOut(0x00, 0x6C);
    sig = (sig << 8) + val;
  }
  return sig;
}

int getCommand() {

  // We must get a valid input to proceed
  int reply;

  do {
    Serial.println();
    Serial.println("Enter 0 to read fuses (no writing)");
    Serial.println("Enter 1 to use Digispark defaults");
    Serial.println("Enter 2 to use P5 as Reset pin again");
    Serial.println();
    while (!Serial.available()) {
      // Wait for user input
    }
    reply = Serial.read();
  }
  while (!(reply == '0' || reply == '1' || reply == '2'));

  return reply;
}
