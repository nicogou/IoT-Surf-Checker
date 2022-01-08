#ifndef SURF_CHECKER_CONSTANTS_H
#define SURF_CHECKER_CONSTANTS_H

#define NB_REQUESTS 3
#define QUERY_TIMEOUT 5000
#define SWELL_NB 6

#define LED_PIN 12
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_DIRECTIONS 4 // 24
#define NUM_LEDS (2 * NUM_DIRECTIONS)
#define BRIGHTNESS 48

enum HttpDataType
{
    TIME = 0,
    WAVE = 1,
    WIND = 2,
};
typedef enum HttpDataType HttpDataType;

#endif