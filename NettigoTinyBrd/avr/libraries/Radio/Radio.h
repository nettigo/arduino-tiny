#ifndef _Radio_h
#define _Radio_h

#include <Arduino.h>
#include <SPI.h>

#define RADIO_WAITS 2
#define RADIO_SENT 1
#define RADIO_LOST 0

#define RADIO_BLOCK 1
#define RADIO_NONBLOCK 0

struct NRFResponse
{
  uint8_t status;
  uint8_t value;
};

struct RadioLost
{
  uint8_t retr: 4;
  uint8_t lost: 4;
};

class NRFProtocol
{
private:
  uint8_t _CSNPin;

public:
  void begin(uint8_t CSNPin)
  {
    _CSNPin = CSNPin;
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_2XCLOCK_MASK);

    pinMode(_CSNPin, OUTPUT);
    digitalWrite(_CSNPin, HIGH);
  }

  uint8_t commandWrite0(uint8_t command)
  {
    digitalWrite(_CSNPin, LOW);
    uint8_t status = SPI.transfer(command);
    digitalWrite(_CSNPin, HIGH);
    return status;
  }

  uint8_t commandWrite1(uint8_t command, uint8_t value)
  {
    digitalWrite(_CSNPin, LOW);
    uint8_t status = SPI.transfer(command);
    SPI.transfer(value);
    digitalWrite(_CSNPin, HIGH);
    return status;
  }

  struct NRFResponse commandRead1(uint8_t command)
  {
    digitalWrite(_CSNPin, LOW);
    struct NRFResponse response;
    response.status = SPI.transfer(command);
    response.value = SPI.transfer(0);
    digitalWrite(_CSNPin, HIGH);
    return response;
  }

  uint8_t commandWriteMany(uint8_t command, uint8_t *values, uint8_t size)
  {
    digitalWrite(_CSNPin, LOW);
    uint8_t status = SPI.transfer(command);
    for (uint8_t i=0; i<size; i++)
    {
      SPI.transfer(values[i]);
    }
    digitalWrite(_CSNPin, HIGH);
    return status;
  }

  uint8_t commandReadMany(uint8_t command, uint8_t *values, uint8_t size)
  {
    digitalWrite(_CSNPin, LOW);
    uint8_t status = SPI.transfer(command);
    for (uint8_t i=0; i<size; i++)
    {
      values[i] = SPI.transfer(0);
    }
    digitalWrite(_CSNPin, HIGH);
    return status;
  }

  struct NRFResponse registerRead(uint8_t reg)
  {
    return commandRead1(reg);
  }

  uint8_t registerWrite(uint8_t reg, uint8_t value)
  {
    uint8_t command = reg | 0x20;
    return commandWrite1(command, value);
  }

  uint8_t addressWrite(uint8_t reg, uint8_t *address, uint8_t size)
  {
    uint8_t command = reg | 0x20;
    return commandWriteMany(command, address, size);
  }

  uint8_t addressRead(uint8_t reg, uint8_t *address, uint8_t size)
  {
    return commandWriteMany(reg, address, size);
  }
};


class CE
{
private:
  uint8_t _pin;

public:
  void begin(uint8_t pin)
  {
    _pin = pin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void activate()
  {
    digitalWrite(_pin, HIGH);
  }

  void deactivate()
  {
    digitalWrite(_pin, LOW);
  }

  void pulse()
  {
    digitalWrite(_pin, HIGH);
    digitalWrite(_pin, LOW);
  }
};


class NRF24L01P
{
private:
  NRFProtocol _protocol;

  void _reset()
  {
    // disable interrupts, enable CRC, CRC 16 bits, power off
    _protocol.registerWrite(0x00, 0b01111100);
    // enable ACK on pipes 0, 1
    _protocol.registerWrite(0x01, 0b00000011);
    // enable data pipes 0, 1
    _protocol.registerWrite(0x02, 0b00000011);
    // address width 3 bytes
    _protocol.registerWrite(0x03, 0b00000001);
    // retransmission delay 4 ms, 15 times
    _protocol.registerWrite(0x04, 0b11111111);
    // rf channel 1
    _protocol.registerWrite(0x05, 0b00000001);
    // speed 256 kbps, power 0 dBm
    _protocol.registerWrite(0x06, 0b00100110);
    // reset status bits
    _protocol.registerWrite(0x07, 0b01110000);
    // error counter - read only
    //_protocol.registerWrite(0x08, 0b);
    // power detector - read only
    //_protocol.registerWrite(0x09, 0b);
    // rx address for pipes 0..5
    byte address1[3] = {0xe7, 0xe7, 0xe7};
    byte address2[3] = {0xc2, 0xc2, 0xc2};
    _protocol.addressWrite(0x0a, address1, 3);
    _protocol.addressWrite(0x0b, address2, 3);
    _protocol.registerWrite(0x0c, 0xc3);
    _protocol.registerWrite(0x0d, 0xc4);
    _protocol.registerWrite(0x0e, 0xc5);
    _protocol.registerWrite(0x0f, 0xc6);
    // tx address
    _protocol.addressWrite(0x10, address1, 3);
    // paylo9ad size 0..5
    _protocol.registerWrite(0x11, 0);
    _protocol.registerWrite(0x12, 0);
    _protocol.registerWrite(0x13, 0);
    _protocol.registerWrite(0x14, 0);
    _protocol.registerWrite(0x15, 0);
    _protocol.registerWrite(0x16, 0);
    // fifo status - read only
    //_protocol.registerWrite(0x17, 0);
    // dynamic payload on pipes 0, 1
    _protocol.registerWrite(0x1c, 0b00000011);
    // enable dynamic payload
    _protocol.registerWrite(0x1d, 0b00000100);
    // flush tx
    _protocol.commandWrite0(0b11100001);
    // flush rx
    _protocol.commandWrite0(0b11100010);
  }

public:
  void begin(uint8_t csnPin)
  {
    _protocol.begin(csnPin);
    _reset();
  }

  void powerOff()
  {
    _protocol.registerWrite(0x00, 0b01111100);
  }

  void rxMode()
  {
    _protocol.registerWrite(0x00, 0b01111111);
  }

  void txMode()
  {
    _protocol.registerWrite(0x00, 0b01111110);
  }

  void channel(uint8_t channelNumber)
  {
    _protocol.registerWrite(0x05, channelNumber);
  }

  void rxStatusReset()
  {
    _protocol.registerWrite(0x07, 0b01000000);
  }

  void txStatusReset()
  {
    _protocol.registerWrite(0x07, 0b00110000);
  }

  bool rxStatus()
  {
    uint8_t status = _protocol.commandWrite0(0xff);
    uint8_t rxFifoNumber = (status >> 1) & 0x07;
    if (rxFifoNumber != 7)
    {
      return true;
    }

    return false;
  }

  uint8_t txStatus()
  {
    uint8_t status = _protocol.commandWrite0(0xff);
    if (bitRead(status, 4))
    {
      return RADIO_LOST;
    }

    if (bitRead(status, 5))
    {
      return RADIO_SENT;
    }

    return RADIO_WAITS;
  }

  void txAddress(uint8_t address[3])
  {
    _protocol.addressWrite(0x0a, address, 3);
    _protocol.addressWrite(0x10, address, 3);
  }

  void rxAddress(uint8_t address[3])
  {
    _protocol.addressWrite(0x0b, address, 3);
  }

  void read(uint8_t *data, uint8_t size)
  {
    _protocol.commandReadMany(0b01100001, data, size);
  }

  void write(uint8_t *data, uint8_t size)
  {
    _protocol.commandWriteMany(0b10100000, data, size);
  }

  void txFlush()
  {
    _protocol.commandWrite0(0b11100001);
  }

  void rxFlush()
  {
    _protocol.commandWrite0(0b11100010);
  }

  uint8_t rxSize()
  {
    struct NRFResponse response = _protocol.commandRead1(0b01100000);
    uint8_t size = response.value;
    return size;
  }

  void speedPower(uint8_t speed, uint8_t power)
  {
    uint8_t regValue = 0;
    bitWrite(regValue, 3, bitRead(speed, 0));
    bitWrite(regValue, 5, bitRead(speed, 1));

    bitWrite(regValue, 1, bitRead(power, 0));
    bitWrite(regValue, 2, bitRead(power, 1));
    _protocol.registerWrite(0x06, regValue);
  }

  struct RadioLost txDataLost()
  {
    struct RadioLost radiolost;
    struct NRFResponse response = _protocol.registerRead(0x08);
    uint8_t registerValue = response.value;
    radiolost.lost = registerValue >> 4;
    radiolost.retr = registerValue & 0x0f;
    return radiolost;
  }

  uint8_t rxSignalStrength()
  {
    struct NRFResponse response = _protocol.registerRead(0x09);
    uint8_t registerValue = response.value;
    return registerValue;
  }
};


class RadioClass
{
private:
  CE _ce;
  bool _is_rx_mode : 1;
  bool _is_power_on : 1;

  void _rxMode()
  {
    _ce.deactivate();
    device.rxMode();
    _powerOnDelay();
    _is_rx_mode = true;
    device.rxFlush();
    device.rxStatusReset();
    _ce.activate();
  }

  void _powerOnDelay()
  {
    if (!_is_power_on)
    {
      delayMicroseconds(1500);
      _is_power_on = true;
    }
  }

public:
  NRF24L01P device;

  void begin(uint8_t address[3], uint8_t channel, uint8_t csnPin=3, uint8_t cePin=7)
  {
    device.begin(csnPin);
    _ce.begin(cePin);
    device.rxAddress(address);
    device.channel(channel);
    _is_rx_mode = false;
    _is_power_on = false;
  }

  void write(uint8_t address[3], uint8_t *data, uint8_t size)
  {
    _ce.deactivate();
    device.txMode();
    _powerOnDelay();
    _is_rx_mode = false;
    device.txFlush();
    device.txAddress(address);
    device.write(data, size);
    device.txStatusReset();
    _ce.pulse();
  }

  template <typename T>
  void write(uint8_t address[3], T &t)
  {
    uint8_t *data = (uint8_t*) &t;
    write(address, data, sizeof(T));
  }

  uint8_t flush(uint8_t blocking=RADIO_BLOCK)
  {
    uint8_t status = RADIO_WAITS;
    do
    {
      status = device.txStatus();
    } while (blocking && status == RADIO_WAITS);
    return status;
  }

  uint8_t available()
  {
    if (!_is_rx_mode)
    {
      _rxMode();
    }

    bool status = device.rxStatus();
    if (status)
    {
      uint8_t rxDataSize = device.rxSize();
      return rxDataSize;
    }

    return 0;
  }

  void read(uint8_t *data)
  {
    uint8_t size = device.rxSize();
    device.read(data, size);
    device.rxStatusReset();
  }

  void off()
  {
    _ce.deactivate();
    device.powerOff();
    _is_rx_mode = false;
    _is_power_on = false;
  }
};

static RadioClass Radio;

#endif
