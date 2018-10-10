// ----------------------------------------------------------------------------
// TLE72X.cpp
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#include "TLE72X.h"


TLE72X::TLE72X()
{
  initialized_ = false;
}

TLE72X::TLE72X(size_t chip_select_pin,
               int reset_pin) :
chip_select_pin_(chip_select_pin),
reset_pin_(reset_pin)
{
  initialized_ = false;

  pinMode(chip_select_pin_,OUTPUT);
  digitalWrite(chip_select_pin_,HIGH);

  if (reset_pin_ >= 0)
  {
    pinMode(reset_pin_,OUTPUT);
    digitalWrite(reset_pin_,HIGH);
  }
}

void TLE72X::setup(size_t chip_count)
{
  if ((chip_count >= CHIP_COUNT_MIN) && (chip_count <= CHIP_COUNT_MAX))
  {
    chip_count_ = chip_count;
  }
  else
  {
    chip_count_ = CHIP_COUNT_MIN;
  }
  SPI.begin();
  setAllChannelsMapTrue();
  setAllChannelsBooleanAnd();
  setAllChannelsOff();
  initialized_ = true;
}

void TLE72X::usingInterrupt(IRQ_NUMBER_t interrupt_number)
{
  SPI.usingInterrupt(interrupt_number);
}

void TLE72X::notUsingInterrupt(IRQ_NUMBER_t interrupt_number)
{
  SPI.notUsingInterrupt(interrupt_number);
}

void TLE72X::setChannels(uint32_t channels)
{
  spiBeginTransaction();
  noInterrupts();
  channels_ = channels;
  interrupts();
  for (int ic = ((int)chip_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_CTL);
    SPI.transfer(channels_>>(ic*8));
  }
  spiEndTransaction();
}

void TLE72X::setChannelOn(size_t channel)
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

void TLE72X::setChannelOff(size_t channel)
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

void TLE72X::toggleChannel(size_t channel)
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

void TLE72X::setChannelOnAllOthersOff(size_t channel)
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

void TLE72X::setChannelOffAllOthersOn(size_t channel)
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

size_t TLE72X::getChannelCount()
{
  return chip_count_*CHANNEL_COUNT_PER_IC;
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

void TLE72X::setChannelsMap(uint32_t channels)
{
  spiBeginTransaction();
  noInterrupts();
  mapped_ = channels;
  interrupts();
  for (int ic = ((int)chip_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_MAP);
    SPI.transfer(mapped_>>(ic*8));
  }
  spiEndTransaction();
}

void TLE72X::setChannelMapTrue(size_t channel)
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

void TLE72X::setChannelMapFalse(size_t channel)
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

void TLE72X::setChannelsBoolean(uint32_t bool_state)
{
  spiBeginTransaction();
  bool_state_ = bool_state;
  for (int ic = ((int)chip_count_ - 1); ic >= 0; --ic)
  {
    SPI.transfer(CMD_WRITE + ADDR_BOL);
    SPI.transfer(bool_state_>>(ic*8));
  }
  spiEndTransaction();
}

void TLE72X::setChannelBooleanAnd(size_t channel)
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

void TLE72X::setChannelBooleanOr(size_t channel)
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

void TLE72X::enableClockSelect()
{
  digitalWrite(chip_select_pin_,LOW);
}

void TLE72X::disableClockSelect()
{
  digitalWrite(chip_select_pin_,HIGH);
}

void TLE72X::spiBeginTransaction()
{
  SPI.beginTransaction(SPISettings(SPI_CLOCK,SPI_BIT_ORDER,SPI_MODE));
  enableClockSelect();
}

void TLE72X::spiEndTransaction()
{
  disableClockSelect();
  SPI.endTransaction();
}
