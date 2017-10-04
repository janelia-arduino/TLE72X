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

TLE72X::TLE72X(const size_t cs_pin, const int reset_pin) :
  cs_pin_(cs_pin),
  reset_pin_(reset_pin)
{
  initialized_ = false;

  pinMode(cs_pin_,OUTPUT);
  digitalWrite(cs_pin_,HIGH);

  if (reset_pin_ >= 0)
  {
    pinMode(reset_pin_,OUTPUT);
    digitalWrite(reset_pin_,HIGH);
  }
}

void TLE72X::setup(const size_t ic_count)
{
  if (ic_count <= IC_COUNT_MAX)
  {
    ic_count_ = ic_count;
  }
  else
  {
    ic_count_ = IC_COUNT_MIN;
  }
  SPI.begin();
  setAllChannelsMapTrue();
  setAllChannelsBooleanAnd();
  setAllChannelsOff();
  initialized_ = true;
}

void TLE72X::usingInterrupt(const IRQ_NUMBER_t interrupt_number)
{
  SPI.usingInterrupt(interrupt_number);
}

void TLE72X::notUsingInterrupt(const IRQ_NUMBER_t interrupt_number)
{
  SPI.notUsingInterrupt(interrupt_number);
}

void TLE72X::setChannels(const uint32_t channels)
{
  SPI.beginTransaction(SPISettings(spi_clock_,spi_bit_order_,spi_mode_));
  digitalWrite(cs_pin_,LOW);
  noInterrupts();
  channels_ = channels;
  interrupts();
  for (int ic = ((int)ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_CTL);
    SPI.transfer(channels_>>(ic*8));
  }
  digitalWrite(cs_pin_,HIGH);
  SPI.endTransaction();
}

void TLE72X::setChannelOn(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ |= bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelOff(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ &= ~bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOn(const uint32_t channels)
{
  noInterrupts();
  channels_ |= channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setChannelsOff(const uint32_t channels)
{
  noInterrupts();
  channels_ &= ~channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::toggleChannel(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ ^= bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::toggleChannels(const uint32_t channels)
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

void TLE72X::setChannelOnAllOthersOff(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelOffAllOthersOn(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = ~bit;
    interrupts();
    setChannels(channels_);
  }
}

void TLE72X::setChannelsOnAllOthersOff(const uint32_t channels)
{
  noInterrupts();
  channels_ = channels;
  interrupts();
  setChannels(channels_);
}

void TLE72X::setChannelsOffAllOthersOn(const uint32_t channels)
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

size_t TLE72X::getChannelCount()
{
  return ic_count_*CHANNEL_COUNT_PER_IC;
}

void TLE72X::reset()
{
  if (reset_pin_ >= 0)
  {
    digitalWrite(reset_pin_,LOW);
    delay(RESET_DELAY);
    digitalWrite(reset_pin_,HIGH);
  }
  noInterrupts();
  channels_ = 0;
  interrupts();
}

void TLE72X::setChannelsMap(const uint32_t channels)
{
  SPI.beginTransaction(SPISettings(spi_clock_,spi_bit_order_,spi_mode_));
  digitalWrite(cs_pin_,LOW);
  noInterrupts();
  mapped_ = channels;
  interrupts();
  for (int ic = ((int)ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_MAP);
    SPI.transfer(mapped_>>(ic*8));
  }
  digitalWrite(cs_pin_,HIGH);
  SPI.endTransaction();
}

void TLE72X::setChannelMapTrue(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    mapped_ |= bit;
    interrupts();
    setChannelsMap(mapped_);
  }
}

void TLE72X::setChannelMapFalse(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
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

void TLE72X::setChannelsBoolean(const uint32_t bool_state)
{
  SPI.beginTransaction(SPISettings(spi_clock_,spi_bit_order_,spi_mode_));
  digitalWrite(cs_pin_,LOW);
  bool_state_ = bool_state;
  for (int ic = ((int)ic_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_BOL);
    SPI.transfer(bool_state_>>(ic*8));
  }
  digitalWrite(cs_pin_,HIGH);
  SPI.endTransaction();
}

void TLE72X::setChannelBooleanAnd(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    bool_state_ |= bit;
    interrupts();
    setChannelsBoolean(bool_state_);
  }
}

void TLE72X::setChannelBooleanOr(const size_t channel)
{
  if (channel < CHANNEL_COUNT_MAX)
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
