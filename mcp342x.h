#ifndef MCP342X_INTOTO_H_
#define MCP342X_INTOTO_H_

#include <stdio.h>

typedef uint8_t (*mcp342x_com_fptr_t)(uint8_t addr, uint8_t* data, uint8_t len);

typedef uint8_t (*mcp342x_delay_fptr_t)(uint32_t ms);

typedef enum mcp342x_channel_enum {
    MCP342X_CHANNEL_1 = 0x00,
    MCP342X_CHANNEL_2 = 0x01,
    MCP342X_CHANNEL_3 = 0x02,
    MCP342X_CHANNEL_4 = 0x03,
} mcp342x_channel_t;

typedef enum mcp342x_mode_enum {
    MCP342X_MODE_ONE_SHOT   = 0x00,
    MCP342X_MODE_CONTINUOUS = 0x10,
} mcp342x_mode_t;

typedef enum mcp342x_gain_enum {
    MCP342X_GAIN_1X = 0x00,
    MCP342X_GAIN_2X = 0x01,
    MCP342X_GAIN_4X = 0x02,
    MCP342X_GAIN_8X = 0x03,
} mcp342x_gain_t;

typedef enum mcp342x_resolution_enum {
    MCP342X_RESOLUTION_12_BIT = 0x00,
    MCP342X_RESOLUTION_14_BIT = 0x01,
    MCP342X_RESOLUTION_16_BIT = 0x02,
    MCP342X_RESOLUTION_18_BIT = 0x03,
} mcp342x_resolution_t;

typedef enum mcp342x_err_enum {
    MCP342X_OK,
    MCP342X_ERR,
    MCP342X_ERR_CONVERT,
    MCP342X_ERR_TIMEOUT,
    MCP342X_ERR_CONFIG,
} mcp342x_err_t;

typedef struct mcp342x_result_struct {
    uint32_t raw;
    float voltage;
} mcp342x_result_t;

typedef struct mcp342x_dev_struct {
    /* I2C COMMUNICATION */
    uint8_t addr;
    mcp342x_com_fptr_t read;
    mcp342x_com_fptr_t write;
    mcp342x_delay_fptr_t delay;

    /* MCP342X CONFIGURATION */
    mcp342x_mode_t mode;
    mcp342x_gain_t gain;
    mcp342x_channel_t channel;
    mcp342x_resolution_t resolution;
    float scale_factor;
    float timeout_ms;
} mcp342x_dev_t;

mcp342x_err_t mcp342x_read_channel(mcp342x_dev_t* dev,
                                   mcp342x_result_t* result);

#endif /* MCP342X_INTOTO_H_ */
