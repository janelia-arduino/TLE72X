#include <Arduino.h>
#include <SPI.h>
#include <Streaming.h>
#include <TLE72X.h>


const long BAUD = 115200;
const size_t LOOP_DELAY = 1000;
const size_t CS_PIN = 10;
const size_t ENABLE_PIN = 3;
const size_t RESET_PIN = 2;
const size_t IC_COUNT = 1;
// Number of power switch IC chips connected in a daisy chain on the
// pcb. There are 8 power switch channels per IC.

// Instantiate TLE72X
TLE72X power_switch = TLE72X(CS_PIN,RESET_PIN);

size_t channel_count;

void setup()
{
  // Setup serial communications
  Serial.begin(BAUD);

  power_switch.setup(IC_COUNT);

  channel_count = power_switch.getChannelCount();

  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,HIGH);
}

void loop()
{
  for (size_t channel = 0; channel < channel_count; ++channel)
  {
    power_switch.setChannelOn(channel);
    Serial << "set channel " << channel << " on" << endl;

    delay(LOOP_DELAY);
  }

  // Set all channels to off
  power_switch.setAllChannelsOff();
  Serial << "set all channels off" << endl << endl;

  delay(LOOP_DELAY);
}
