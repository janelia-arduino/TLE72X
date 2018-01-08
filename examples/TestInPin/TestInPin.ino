#include <Arduino.h>
#include <SPI.h>
#include <Streaming.h>
#include <TLE72X.h>


const long BAUD = 115200;
const size_t LOOP_DELAY = 1000;
const size_t CS_PIN = 10;
const size_t IN_PIN = 3;
const size_t RESET_PIN = 2;
const size_t IC_COUNT = 1;
// IC_COUNT is the number of power switch IC chips connected in a
// daisy chain on the pcb. There are 8 power switch channels per IC.

// Instantiate TLE72X
TLE72X power_switch = TLE72X(CS_PIN,RESET_PIN);

int channel_count;
uint32_t channels;
uint32_t bit;

void setup()
{
  // Setup serial communications
  Serial.begin(BAUD);

  power_switch.setup(IC_COUNT);

  channel_count = power_switch.getChannelCount();

  pinMode(IN_PIN,OUTPUT);
  digitalWrite(IN_PIN,LOW);

  power_switch.setAllChannelsOn();
}

void blinkInPin()
{
  for (int i=0; i<4; ++i)
  {
    digitalWrite(IN_PIN,LOW);
    Serial << "IN_PIN LOW " << endl;
    delay(LOOP_DELAY);
    digitalWrite(IN_PIN,HIGH);
    Serial << "IN_PIN HIGH " << endl;
    delay(2*LOOP_DELAY);
  }
}
void loop()
{
  for (int channel=0; channel<channel_count; ++channel)
  {
    if ((channel % 2) == 0)
    {
      power_switch.setChannelMapFalse(channel);
    }
  }
  blinkInPin();

  power_switch.setAllChannelsMapFalse();
  blinkInPin();

  power_switch.setAllChannelsMapTrue();
  blinkInPin();
}
