#include <Servo.h>
#include <Firmata.h>

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* servos */
Servo servo9, servo10; // one instance per pin
/* analog inputs */
int analogInputsToReport = 0; // bitwise array to store pin reporting
int analogPin = 0; // counter for reading analog pins
/* timer variables */
unsigned long currentMillis;     // store the current value from millis()
unsigned long previousMillis;    // for comparison with currentMillis

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void analogWriteCallback(byte pin, int value)
{
  switch (pin) {
    case 9: servo9.write(value); break;
    case 10: servo10.write(value); break;
    case 3:
    case 5:
    case 6:
    case 11: // PWM pins
      analogWrite(pin, value);
      break;
  }
}
// -----------------------------------------------------------------------------
// sets bits in a bit array (int) to toggle the reporting of the analogIns
void reportAnalogCallback(byte pin, int value)
{
  if (value == 0) {
    analogInputsToReport = analogInputsToReport & ~ (1 << pin);
  }
  else { // everything but 0 enables reporting of that pin
    analogInputsToReport = analogInputsToReport | (1 << pin);
  }
}
/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.attach(REPORT_ANALOG, reportAnalogCallback);

  servo9.attach(9);
  servo10.attach(10);
  Firmata.begin(57600);
}
/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  while (Firmata.available())
    Firmata.processInput();
  currentMillis = millis();
  if (currentMillis - previousMillis > 20) {
    previousMillis += 20;                   // run this every 20ms
    for (analogPin = 0; analogPin < TOTAL_ANALOG_PINS; analogPin++) {
      if ( analogInputsToReport & (1 << analogPin) )
        Firmata.sendAnalog(analogPin, analogRead(analogPin));
    }
  }
}
