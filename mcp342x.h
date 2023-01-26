#ifndef MCP342X_H_
#define MCP342X_H_

#include <stdio.h>

#define MCP342X_INVALID_VALUE (-131072)  // error value

typedef uint8_t (*mcp342x_com_fptr_t)(uint8_t addr, uint8_t* data, uint8_t len);

typedef uint8_t (*mcp342x_delay_fptr_t)(uint32_t ms);

typedef enum mcp342x_channel_enum {
    MCP342X_CHANNEL_1,
    MCP342X_CHANNEL_2,
    MCP342X_CHANNEL_3,
    MCP342X_CHANNEL_4,
} mcp342x_channel_t;

typedef enum mcp342x_mode_enum {
    MCP342X_MODE_ONE_SHOT,
    MCP342X_MODE_CONTINUOUS,
} mcp342x_mode_t;

typedef enum mcp342x_gain_enum {
    MCP342X_GAIN_1X,
    MCP342X_GAIN_2X,
    MCP342X_GAIN_4X,
    MCP342X_GAIN_8X,
} mcp342x_gain_t;

typedef enum mcp342x_resolution_enum {
    MCP342X_RESOLUTION_12_BIT,
    MCP342X_RESOLUTION_14_BIT,
    MCP342X_RESOLUTION_16_BIT,
    MCP342X_RESOLUTION_18_BIT,
} mcp342x_resolution_t;

typedef enum mcp342x_err_enum {
    MCP342X_OK,
    MCP342X_ERR_READ,
    MCP342X_ERR_CONVERT,
    MCP342X_ERR_TIMEOUT,
    MCP342X_ERR_CONFIG,
} mcp342x_err_t;

typedef struct mcp342x_result_struct {
    int32_t raw;
    double value;
    uint8_t config;
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

    /* EXTRAS */
    uint32_t timeout_ms;
    float scale_factor;
} mcp342x_dev_t;

mcp342x_err_t mcp342x_read_channel(mcp342x_dev_t* dev,
                                   mcp342x_result_t* results);

#endif /* MCP342X_H_ */
