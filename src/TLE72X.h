// ----------------------------------------------------------------------------
// TLE72X.h
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------

#ifndef TLE72X_H
#define TLE72X_H
#include <Arduino.h>
#include <SPI.h>


class TLE72X
{
public:
  TLE72X();
  TLE72X(size_t chip_select_pin,
    int reset_pin=-1);

  void setup(size_t chip_count=1);

  static void usingInterrupt(IRQ_NUMBER_t interrupt_number);
  static void notUsingInterrupt(IRQ_NUMBER_t interrupt_number);

  void setChannels(uint32_t channels);
  void setChannelOn(size_t channel);
  void setChannelOff(size_t channel);
  void setChannelsOn(uint32_t channels);
  void setChannelsOff(uint32_t channels);
  void toggleChannel(size_t channel);
  void toggleChannels(uint32_t channels);
  void toggleAllChannels();
  void setAllChannelsOn();
  void setAllChannelsOff();
  void setChannelOnAllOthersOff(size_t channel);
  void setChannelOffAllOthersOn(size_t channel);
  void setChannelsOnAllOthersOff(uint32_t channels);
  void setChannelsOffAllOthersOn(uint32_t channels);
  uint32_t getChannelsOn();
  size_t getChannelCount();
  void reset();
  void setChannelsMap(uint32_t channels);
  void setChannelMapTrue(size_t channel);
  void setChannelMapFalse(size_t channel);
  void setAllChannelsMapTrue();
  void setAllChannelsMapFalse();
  void setChannelsBoolean(uint32_t bool_state);
  void setChannelBooleanAnd(size_t channel);
  void setChannelBooleanOr(size_t channel);
  void setAllChannelsBooleanAnd();
  void setAllChannelsBooleanOr();

private:
  const static uint32_t SPI_CLOCK = 4000000;
  const static uint8_t SPI_BIT_ORDER = MSBFIRST;
  const static uint8_t SPI_MODE = SPI_MODE1;
  const static size_t CHIP_COUNT_MIN = 1;
  const static size_t CHIP_COUNT_MAX = 4;

  const static size_t CHANNEL_COUNT_PER_IC = 8;
  const static size_t CHANNEL_COUNT_MAX = 32;

  const static size_t RESET_DELAY = 200;

  const static byte CMD_DIAGNOSIS = 0b11<<6;
  const static byte CMD_READ = 0b01<<6;
  const static byte CMD_RESET = 0b10<<6;
  const static byte CMD_WRITE = 0b11<<6;

  const static byte ADDR_MAP = 0b001; // Input Mapping Register
  const static byte ADDR_BOL = 0b010; // Boolean Operation Register
  const static byte ADDR_OVL = 0b011; // Overload Behavior Register
  const static byte ADDR_OVT = 0b100; // Overtemperature Behavior Register
  const static byte ADDR_SLE = 0b101; // Switching Speed / Slew Rate Register
  const static byte ADDR_STA = 0b110; // Output State Register
  const static byte ADDR_CTL = 0b111; // Serial Output Control Register

  size_t chip_select_pin_;
  int reset_pin_;
  boolean initialized_;
  uint32_t channels_;
  uint32_t mapped_;
  uint32_t bool_state_;
  size_t chip_count_;

  void enableClockSelect();
  void disableClockSelect();
  void spiBeginTransaction();
  void spiEndTransaction();
};

#endif
