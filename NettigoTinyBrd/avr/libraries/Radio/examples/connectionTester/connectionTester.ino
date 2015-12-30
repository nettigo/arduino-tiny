#include <SPI.h>
#include <Radio.h>
#include <Battery.h>

#define HARD_RETRY_LIMIT  5

struct Payload
  {
  byte id;
  unsigned long battery;
  unsigned long seq;
  byte retry;
  unsigned long lost;
} data;

byte addressRemote[5] = { 0, 0, 3};

void setup()
{
  data.id = 100;
  data.seq = data.lost = 0;
  byte address[5] = {3,4,5};
  Radio.begin(address,100);
  
}

//**************************************************************


void radio_write(struct Payload &data, byte retry = 0)
{
  if (retry == HARD_RETRY_LIMIT) {
    //we have failed transmit..
    data.seq ++;
    data.lost ++;
    return;
  }
  data.retry = retry;
  Radio.write(addressRemote,data);
  while(true) {
    switch(Radio.flush()){
      case RADIO_SENT:
        data.seq++;
        return;
      case RADIO_LOST:
        radio_write(data,retry+1);
        return;
    }
  }
  
}


void loop()
{
  data.battery = batteryRead();
  
  //send data
  radio_write(data);

  //go to sleep
  Radio.off();
  sleep(15000);
//  wakeUp();
}

