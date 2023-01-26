#include "mcp342x.h"

static double mcp342x_convert_range(mcp342x_resolution_t res,
                                    mcp342x_gain_t gain,
                                    double raw) {
    return raw / (2048 * (1 << ((int)res * 2))) / (1 << (int)gain);
}

static int32_t mcp342x_convert_raw(mcp342x_resolution_t res, uint8_t* vals) {
    switch (res) {
        case MCP342X_RESOLUTION_12_BIT:
            return ((vals[0] & 1 << 3) != 0)
                       ? ~(((vals[0] & 0x07) << 8) | vals[1])
                       : ((vals[0] & 0x07) << 8) | vals[1];
        case MCP342X_RESOLUTION_14_BIT:
            return ((vals[0] & 1 << 5) != 0)
                       ? ~(((vals[0] & 0x1f) << 8) | vals[1])
                       : ((vals[0] & 0x1f) << 8) | vals[1];
        case MCP342X_RESOLUTION_16_BIT:
            return ((vals[0] & 1 << 7) != 0)
                       ? ~(((vals[0] & 0x7f) << 8) | vals[1])
                       : ((vals[0] & 0x7f) << 8) | vals[1];
        case MCP342X_RESOLUTION_18_BIT:
            return ((vals[0] & 1 << 1) != 0)
                       ? ~(((vals[0] & 0x01) << 16) | (vals[1] << 8) | vals[2])
                       : ((vals[0] & 0x01) << 16) | (vals[1] << 8) | vals[2];
        default:
            return 0;
    }
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

mcp342x_err_t mcp342x_read_channel(mcp342x_dev_t* dev, mcp342x_result_t* res) {
    uint8_t reading[4] = {0};
    uint32_t time      = 0;

    while (1) {  // Retries when reading invalid value
        if (mcp342x_write_config(dev) != MCP342X_OK) {
            return MCP342X_ERR_CONFIG;
        }

        dev->read(dev->addr, reading, 4);
        while (1) {  // Waits for conversion to finish
            if (dev->timeout_ms != 0.0 && time > dev->timeout_ms) {
                return MCP342X_ERR_TIMEOUT;
            }

            if (dev->read(dev->addr, reading, 4) != MCP342X_OK) {
                return MCP342X_ERR_READ;
            }

            res->config = dev->resolution == MCP342X_RESOLUTION_18_BIT
                              ? reading[3]
                              : reading[2];

            if (res->config >> 7 == 0) {
                break;
            }

            dev->delay(10);
            time += 10;
        }

        res->raw = mcp342x_convert_raw(dev->resolution, reading);
        res->value =
            mcp342x_convert_range(dev->resolution, dev->gain, res->raw);
        res->value *= dev->scale_factor != 0 ? dev->scale_factor : 1;

        if (res->raw != MCP342X_INVALID_VALUE) {
            break;
        }

        dev->delay(50);
        time += 50;
    }

    return MCP342X_OK;
}
