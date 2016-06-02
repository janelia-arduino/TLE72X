// ----------------------------------------------------------------------------
// TLE72X.cpp
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------

#include "TLE72X.h"


TLE72X::TLE72X()
{
  initialized_ = false;
}

TLE72X::TLE72X(const int cs_pin) :
  cs_pin_(cs_pin)
{
  initialized_ = false;

  pinMode(cs_pin_,OUTPUT);
  digitalWrite(cs_pin_,HIGH);
}

TLE72X::TLE72X(const int cs_pin, const int reset_pin) :
  cs_pin_(cs_pin),
  reset_pin_(reset_pin)
{
  initialized_ = false;

  pinMode(cs_pin,OUTPUT);
  digitalWrite(cs_pin,HIGH);

  pinMode(reset_pin,OUTPUT);
  digitalWrite(reset_pin,HIGH);
}

void TLE72X::setup(const int ic_count, const boolean spi_reset)
{
  spi_reset_ = spi_reset;
  if ((0 < ic_count) && (ic_count <= IC_COUNT_MAX))
  {
    ic_count_ = ic_count;
  }
  else
  {
    ic_count_ = IC_COUNT_MIN;
  }
  spiBegin();
  setAllChannelsMapTrue();
  setAllChannelsBooleanAnd();
  setAllChannelsOff();
  initialized_ = true;
}

void TLE72X::setChannels(uint32_t channels)
{
  if (spi_reset_)
  {
    spiBegin();
  }
  digitalWrite(cs_pin_,LOW);
  noInterrupts();
  channels_ = channels;
  for (int ic = (ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_CTL);
    SPI.transfer(channels_>>(ic*8));
  }
  interrupts();
  digitalWrite(cs_pin_,HIGH);
  digitalRead(cs_pin_);
}

void TLE72X::setChannelOn(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ |= bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelOff(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ &= ~bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOn(uint32_t channels)
{
  noInterrupts();
  channels_ |= channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setChannelsOff(uint32_t channels)
{
  noInterrupts();
  channels_ &= ~channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::toggleChannel(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ ^= bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::toggleChannels(uint32_t channels)
{
  noInterrupts();
  channels_ ^= channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::toggleAllChannels()
{
  noInterrupts();
  channels_ = ~channels_;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setAllChannelsOn()
{
  uint32_t bit = 1;
  noInterrupts();
  channels_ = (bit << getChannelCount()) - 1;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setAllChannelsOff()
{
  noInterrupts();
  channels_ = 0;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setChannelOnAllOthersOff(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelOffAllOthersOn(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = ~bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOnAllOthersOff(uint32_t channels)
{
  noInterrupts();
  channels_ = channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setChannelsOffAllOthersOn(uint32_t channels)
{
  noInterrupts();
  channels_ = ~channels;
  interrupts();
  setChannels(channels_);
}

uint32_t TLE72X::getChannelsOn()
{
  return channels_;
}

int TLE72X::getChannelCount()
{
  return ic_count_*CHANNEL_COUNT_PER_IC;
}

void TLE72X::reset()
{
  digitalWrite(reset_pin_,LOW);
  delay(RESET_DELAY);
  digitalWrite(reset_pin_,HIGH);
  noInterrupts();
  channels_ = 0;
  interrupts();
}

void TLE72X::spiBegin()
{
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
}

void TLE72X::setChannelsMap(uint32_t channels)
{
  if (spi_reset_)
  {
    spiBegin();
  }
  digitalWrite(cs_pin_,LOW);
  noInterrupts();
  mapped_ = channels;
  for (int ic = (ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_MAP);
    SPI.transfer(mapped_>>(ic*8));
  }
  interrupts();
  digitalWrite(cs_pin_,HIGH);
  digitalRead(cs_pin_);
}

void TLE72X::setChannelMapTrue(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    mapped_ |= bit;
    interrupts();
    setChannelsMap(mapped_);
  }
}

void TLE72X::setChannelMapFalse(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    mapped_ &= ~bit;
    interrupts();
    setChannelsMap(mapped_);
  }
}

void TLE72X::setAllChannelsMapTrue()
{
  uint32_t bit = 1;
  noInterrupts();
  mapped_ = (bit << getChannelCount()) - 1;
  interrupts();
  setChannelsMap(mapped_);
}

void TLE72X::setAllChannelsMapFalse()
{
  noInterrupts();
  mapped_ = 0;
  interrupts();
  setChannelsMap(mapped_);
}

void TLE72X::setChannelsBoolean(uint32_t bool_state)
{
  if (spi_reset_)
  {
    spiBegin();
  }
  digitalWrite(cs_pin_,LOW);
  noInterrupts();
  bool_state_ = bool_state;
  for (int ic = (ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_BOL);
    SPI.transfer(bool_state_>>(ic*8));
  }
  interrupts();
  digitalWrite(cs_pin_,HIGH);
  digitalRead(cs_pin_);
}

void TLE72X::setChannelBooleanAnd(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    bool_state_ |= bit;
    interrupts();
    setChannelsBoolean(bool_state_);
  }
}

void TLE72X::setChannelBooleanOr(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    bool_state_ &= ~bit;
    interrupts();
    setChannelsBoolean(bool_state_);
  }
}

void TLE72X::setAllChannelsBooleanAnd()
{
  uint32_t bit = 1;
  noInterrupts();
  bool_state_ = (bit << getChannelCount()) - 1;
  interrupts();
  setChannelsBoolean(bool_state_);
}

void TLE72X::setAllChannelsBooleanOr()
{
  noInterrupts();
  bool_state_ = 0;
  interrupts();
  setChannelsBoolean(bool_state_);
}
