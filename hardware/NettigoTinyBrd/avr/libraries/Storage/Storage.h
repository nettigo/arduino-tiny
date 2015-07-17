#ifndef _Storage_h
#define _Storage_h

#include <Arduino.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

template <typename T>
uint16_t storageWrite(const uint16_t address, T &t)
{
  uint8_t *data = (uint8_t*) &t;
  uint16_t dataSize = sizeof(T);
  uint8_t *eeprom_ptr;
  eeprom_ptr = address;
  eeprom_update_block(data, eeprom_ptr, dataSize);
  return dataSize;
}

template <typename T>
uint16_t storageRead(const uint16_t address, T &t)
{
  uint8_t *data = (uint8_t*) &t;
  uint16_t dataSize = sizeof(T);
  uint8_t *eeprom_ptr;
  eeprom_ptr = address;
  eeprom_read_block(data, eeprom_ptr, dataSize);
  return dataSize;
}

template <typename T>
uint16_t flashRead(const void *address, T &t)
{
  void *data = (void*) &t;
  uint16_t dataSize = sizeof(T);
  memcpy_PF(data, address, dataSize);
  return dataSize;  
}

#endif