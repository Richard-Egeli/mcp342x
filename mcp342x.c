#include "mcp342x.h"

static float conversion_factors[] = {
    0.001,          // 12-bit, 1X Gain
    0.0005,         // 12-bit, 2X Gain
    0.00025,        // 12-bit, 4X Gain
    0.000125,       // 12-bit, 8X Gain
    0.00025,        // 14-bit, 1X Gain
    0.000125,       // 14-bit, 2X Gain
    0.0000625,      // 14-bit, 4X Gain
    0.00003125,     // 14-bit, 8X Gain
    0.0000625,      // 16-bit, 1X Gain
    0.00003125,     // 16-bit, 2X Gain
    0.000015625,    // 16-bit, 4X Gain
    0.0000078125,   // 16-bit, 8X Gain
    0.000015625,    // 18-bit, 1X Gain
    0.0000078125,   // 18-bit, 2X Gain
    0.00000390625,  // 18-bit, 4X Gain
    0.000001953125  // 18-bit, 8X Gain
};

static float mcp342x_convert_raw_to_voltage(mcp342x_resolution_t res,
                                            mcp342x_gain_t gain, uint32_t raw) {
    return (float)raw * conversion_factors[(res * 4) + gain];
}

static mcp342x_err_t mcp342x_convert_raw(mcp342x_resolution_t res,
                                         uint8_t* readings, uint32_t* raw) {
    switch (res) {
        case MCP342X_RESOLUTION_12_BIT:
            *raw = ((readings[0] & 0x0f) << 8) | readings[1];
            break;
        case MCP342X_RESOLUTION_14_BIT:
            *raw = ((readings[0] & 0x3f) << 8) | readings[1];
            break;
        case MCP342X_RESOLUTION_16_BIT:
            *raw = (readings[0] << 8) | readings[1];
            break;
        case MCP342X_RESOLUTION_18_BIT:
            *raw =
                ((readings[0] & 0x03) << 16) | (readings[1] << 8) | readings[2];
            break;
        default:
            return MCP342X_ERR;
    }

    return MCP342X_OK;
}

static mcp342x_err_t mcp342x_write_config(mcp342x_dev_t* dev) {
    uint8_t config_reg = 0;

    config_reg |= (dev->channel << 5);
    config_reg |= (dev->mode << 4);
    config_reg |= (dev->resolution << 2);
    config_reg |= (dev->gain << 0);

    if (dev->mode == MCP342X_MODE_ONE_SHOT) {
        config_reg |= (1 << 7);
    }

    if (dev->write(dev->addr, &config_reg, 1) != MCP342X_OK) {
        return MCP342X_ERR_CONFIG;
    }

    return MCP342X_OK;
}

mcp342x_err_t mcp342x_read_channel(mcp342x_dev_t* dev,
                                   mcp342x_result_t* results) {
    uint8_t reading[4] = {0};
    if (mcp342x_write_config(dev) != MCP342X_OK) {
        return MCP342X_ERR_CONFIG;
    }

    float time = 0.0;
    while (1) {
        if (dev->timeout_ms != 0.0 && time > dev->timeout_ms) {
            return MCP342X_ERR_TIMEOUT;
        }

        if (dev->read(dev->addr, reading, 4) != MCP342X_OK) {
            return MCP342X_ERR;
        }

        if (dev->resolution == MCP342X_RESOLUTION_18_BIT) {
            if ((reading[3] >> 7) == 0) {
                break;
            }
            break;
        } else {
            if ((reading[2] >> 7) == 0) {
                break;
            }
        }

        dev->delay(10);
        time += 10.0;
    }

    if (mcp342x_convert_raw(dev->resolution, reading, &results->raw) !=
        MCP342X_OK) {
        return MCP342X_ERR_CONVERT;
    }

    results->voltage = mcp342x_convert_raw_to_voltage(dev->resolution,
                                                      dev->gain, results->raw);
    if (dev->scale_factor != 0.0) {
        results->voltage *= dev->scale_factor;
    }

    return MCP342X_OK;
}
