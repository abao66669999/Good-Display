#include <stdlib.h>
#include "epd.h"

Epd::~Epd()
{
};

Epd::Epd()
{
  reset_pin = RST_PIN;
  dc_pin = DC_PIN;
  cs_pin = CS_PIN;
  busy_pin = BUSY_PIN;
  width = EPD_WIDTH;
  height = EPD_HEIGHT;
};

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
  DigitalWrite(dc_pin, LOW);
  SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
  DigitalWrite(dc_pin, HIGH);
  SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void)
{
  while(DigitalRead(busy_pin) == 1) {      //LOW: idle, HIGH: busy
    delay(100);
  }
  delay(200);
}

int Epd::HDirInit(void)
{
  /* this calls the peripheral hardware interface, see epdif */
  if (IfInit() != 0) {
    return -1;
  }
  /* EPD hardware init start */
  Reset();

  WaitUntilIdle();
  SendCommand(0x12);  //SWRESET
  WaitUntilIdle();

  SendCommand(0x01); //Driver output control
  SendData(0xC7);
  SendData(0x00);
  SendData(0x00);

  SendCommand(0x11); //data entry mode
  SendData(0x01);

  SendCommand(0x44); //set Ram-X address start/end position
  SendData(0x00);
  SendData(0x18);    //0x0C-->(18+1)*8=200

  SendCommand(0x45); //set Ram-Y address start/end position
  SendData(0xC7);   //0xC7-->(199+1)=200
  SendData(0x00);
  SendData(0x00);
  SendData(0x00);

  SendCommand(0x3C); //BorderWavefrom
  SendData(0x01);

  SendCommand(0x18);
  SendData(0x80);
 
  SendCommand(0x4E);   // set RAM x address count to 0;
  SendData(0x00);
  SendCommand(0x4F);   // set RAM y address count to 0X199;
  SendData(0xC7);
  SendData(0x00);
  WaitUntilIdle();
  
/* EPD hardware init end */

  return 0;
}

int Epd::LDirInit(void)
{
  /* this calls the peripheral hardware interface, see epdif */
  if (IfInit() != 0) {
    return -1;
  }
  /* EPD hardware init start */
  Reset();

  WaitUntilIdle();
  SendCommand(0x12);  //SWRESET
  WaitUntilIdle();

  SendCommand(0x01); //Driver output control
  SendData(0xC7);
  SendData(0x00);
  SendData(0x00);

  SendCommand(0x11); //data entry mode
  SendData(0x03);

  SendCommand(0x44); //set Ram-X address start/end position
  SendData(0x00);
  SendData(0x18);    //0x0C-->(18+1)*8=200

  SendCommand(0x45); //set Ram-Y address start/end position
  SendData(0xC7);   //0xC7-->(199+1)=200
  SendData(0x00);
  SendData(0x00);
  SendData(0x00);

  SendCommand(0x3C); //BorderWavefrom
  SendData(0x05);

  SendCommand(0x18);
  SendData(0x80);
 
  SendCommand(0x4E);   // set RAM x address count to 0;
  SendData(0x00);
  SendCommand(0x4F);   // set RAM y address count to 0X199;
  SendData(0xC7);
  SendData(0x00);
  WaitUntilIdle();

  /* EPD hardware init end */

  return 0;
}


/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
  DigitalWrite(reset_pin, LOW);                //module reset
  delay(10);
  DigitalWrite(reset_pin, HIGH);
  delay(10);
}

void Epd::Clear(void)
{
  int w, h;
  w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
  h = EPD_HEIGHT;
  SendCommand(0x24);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      SendData(0xff);
    }
  }

}

void Epd::Display(const unsigned char* frame_buffer)
{
  int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;

  if (frame_buffer != NULL) {
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        SendData(frame_buffer[i + j * w]);
      }
    }
  }

  //DISPLAY REFRESH
  SendCommand(0x22);
  SendData(0xF7);
  SendCommand(0x20);
  WaitUntilIdle();
}

void Epd::DisplayPartBaseImage(const unsigned char* frame_buffer)
{
  int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;

  if (frame_buffer != NULL) {
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        SendData(frame_buffer[i + j * w]);
      }
    }

    SendCommand(0x26);
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        SendData(frame_buffer[i + j * w]);
      }
    }
  }

  //DISPLAY REFRESH
  SendCommand(0x22);
  SendData(0xFF);
  SendCommand(0x20);
  WaitUntilIdle();
}
void Epd::DisplayPartBaseWhiteImage(void)
{
  int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;

  SendCommand(0x24);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      SendData(0xff);
    }
  }

  SendCommand(0x26);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      SendData(0xff);
    }
  }


  //DISPLAY REFRESH
  SendCommand(0x22);
  SendData(0xFF);
  SendCommand(0x20);
  WaitUntilIdle();
}


void Epd::DisplayPart(const unsigned char* frame_buffer)
{
  int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;

  if (frame_buffer != NULL) {
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        SendData(frame_buffer[i + j * w]);
      }
    }
  }

  //DISPLAY REFRESH
  SendCommand(0x22);
  SendData(0xFF);
  SendCommand(0x20);
  WaitUntilIdle();
}


/**
 *  @brief: private function to specify the memory area for data R/W
 */
void Epd::SetMemoryArea(int x_start, int y_start, int x_end, int y_end)
{
  SendCommand(0x44);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  SendData((x_start >> 3) & 0xFF);
  SendData((x_end >> 3) & 0xFF);
  SendCommand(0x45);
  SendData(y_start & 0xFF);
  SendData((y_start >> 8) & 0xFF);
  SendData(y_end & 0xFF);
  SendData((y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void Epd::SetMemoryPointer(int x, int y)
{
  SendCommand(0x4e);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  SendData((x >> 3) & 0xFF);
  SendCommand(0x4F);
  SendData(y & 0xFF);
  SendData((y >> 8) & 0xFF);
  WaitUntilIdle();
}


/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void Epd::DisplayFrame(void)
{
  //DISPLAY REFRESH
  SendCommand(0x22);
  SendData(0xF7);
  SendCommand(0x20);
  WaitUntilIdle();
}

void Epd::DisplayPartFrame(void)
{
  SendCommand(0x22);
  SendData(0xFF);
  SendCommand(0x20);
  WaitUntilIdle();
}


void Epd::SetFrameMemory(
        const unsigned char* image_buffer,
        int x,
        int y,
        int image_width,
        int image_height
)
{
  int x_end;
  int y_end;

  if (
          image_buffer == NULL ||
          x < 0 || image_width < 0 ||
          y < 0 || image_height < 0
  ) {
    return;
  }
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  x &= 0xF8;
  image_width &= 0xF8;
  if (x + image_width >= this->width) {
    x_end = this->width - 1;
  } else {
    x_end = x + image_width - 1;
  }
  if (y + image_height >= this->height) {
    y_end = this->height - 1;
  } else {
    y_end = y + image_height - 1;
  }
  SetMemoryArea(x, y, x_end, y_end);
  SetMemoryPointer(x, y);
  SendCommand(0x24);
  /* send the image data */
  for (int j = 0; j < y_end - y + 1; j++) {
    for (int i = 0; i < (x_end - x + 1) / 8; i++) {
      SendData(image_buffer[i + j * (image_width / 8)]);
    }
  }
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5.
 *          You can use Epd::Init() to awaken
 */
void Epd::Sleep()
{
  SendCommand(0x10); //enter deep sleep
  SendData(0x01);
  delay(200);
  DigitalWrite(reset_pin, LOW);
}









/* END OF FILE */
