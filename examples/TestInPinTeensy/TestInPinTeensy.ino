#include <TLE72X.h>
#include <Streaming.h>


const long BAUD = 115200;
const size_t LOOP_DELAY = 1000;
const size_t CHIP_SELECT_PIN = 10;
const size_t IN_PIN = 3;
const size_t RESET_PIN = 2;
const size_t CHIP_COUNT = 1;
// CHIP_COUNT is the number of power switch chips connected in a
// daisy chain on the pcb. There are 8 power switch channels per chip.

// Instantiate TLE72X
TLE72X power_switch = TLE72X(CHIP_SELECT_PIN,RESET_PIN);

int min_power = 0;
int max_power = 128;
int hold_power = 10;

void setup()
{
  // Setup serial communications
  Serial.begin(BAUD);

  power_switch.setup(CHIP_COUNT);

  analogWriteResolution(7);

  power_switch.setAllChannelsOn();
}

void loop()
{
  analogWrite(IN_PIN,min_power);
  Serial << "IN_PIN LOW " << endl;
  delay(LOOP_DELAY);

  analogWrite(IN_PIN,max_power);
  Serial << "IN_PIN HIGH " << endl;
  delay(LOOP_DELAY);

  analogWrite(IN_PIN,hold_power);
  Serial << "IN_PIN HOLD " << endl << endl;
  delay(2*LOOP_DELAY);
}
