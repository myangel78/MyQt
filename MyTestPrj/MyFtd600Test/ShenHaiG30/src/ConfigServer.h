﻿#pragma once

constexpr char LVDS_FRAMEHEAD_FIRST_2D = 0x5A;
constexpr char LVDS_FRAMEHEAD_SECOND_5A = 0x5A;

#define BUFFER_SIZE 4096

#define BUF_SIZE 1024
#define USB_CMD_READ_BUF_SIZE 14
#define USB_CMD_VALID_LENGTH 12
#define MULTI_ASYNC_NUM             8
#define WHOLE_FRAME_BUF_SIZE 4128


#define COMPLETE_FRAME_SIZE WHOLE_FRAME_BUF_SIZE
#define COMPLETE_FRAME_64CH_SIZE 258
#define CHANNELS_IN_ONE_FRAME 64

constexpr unsigned char TAGSTART[16] = {
    0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
};