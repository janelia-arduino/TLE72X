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
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = 0;
  }
  spiBegin();
  setChannels(channels_);
  initialized_ = true;
}

void TLE72X::setChannels(uint32_t channels)
{
  if (spi_reset_)
  {
    spiBegin();
  }
  digitalWrite(cs_pin_,LOW);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = channels;
    for (int ic = (ic_count_ - 1); 0 <= ic; ic--) {
      SPI.transfer(CMD_WRITE + ADDR_CTL);
      SPI.transfer(channels_>>(ic*8));
    }
  }
  digitalWrite(cs_pin_,HIGH);
  digitalRead(cs_pin_);
}

void TLE72X::setChannelOn(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      channels_ |= bit;
    }
    setChannels(channels_);
  }
}

void TLE72X::setChannelOff(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      channels_ &= ~bit;
    }
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOn(uint32_t channels)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ |= channels;
  }
  setChannels(channels_);
}

void TLE72X::setChannelsOff(uint32_t channels)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ &= ~channels;
  }
  setChannels(channels_);
}

void TLE72X::toggleChannel(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      channels_ ^= bit;
    }
    setChannels(channels_);
  }
}

void TLE72X::toggleChannels(uint32_t channels)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ ^= channels;
  }
  setChannels(channels_);
}

void TLE72X::toggleAllChannels()
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = ~channels_;
  }
  setChannels(channels_);
}

void TLE72X::setAllChannelsOn()
{
  uint32_t bit = 1;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = (bit << getChannelCount()) - 1;
  }
  setChannels(channels_);
}

void TLE72X::setAllChannelsOff()
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = 0;
  }
  setChannels(channels_);
}

void TLE72X::setChannelOnAllOthersOff(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      channels_ = bit;
    }
    setChannels(channels_);
  }
}

void TLE72X::setChannelOffAllOthersOn(int channel)
{
  if ((0 <= channel) && (channel < CHANNEL_COUNT_MAX))
  {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      channels_ = ~bit;
    }
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOnAllOthersOff(uint32_t channels)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = channels;
  }
  setChannels(channels_);
}

void TLE72X::setChannelsOffAllOthersOn(uint32_t channels)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    channels_ = ~channels;
  }
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
}

void TLE72X::spiBegin()
{
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
}

