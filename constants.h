#ifndef SURF_CHECKER_CONSTANTS_H
#define SURF_CHECKER_CONSTANTS_H

#define NB_REQUESTS 5
#define QUERY_TIMEOUT 5000
#define SWELL_NB 6

#define LED_PIN_CLOCK 12
#define LED_PIN_SIDES 11
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_DIRECTIONS 24
#define NUM_LEDS_CLOCK (2 * NUM_DIRECTIONS)
#define NUM_LEDS_PANELS 11
#define NUM_PANELS 4
#define NUM_LEDS_SIDES (NUM_PANELS * NUM_LEDS_PANELS)
#define LOW_TIDE 0
#define HIGH_TIDE (NUM_LEDS_PANELS - 1)
#define NUM_PANELS 4
#define BRIGHTNESS 48
#define FRAMES_PER_SECOND 100

#define DEBUG_TIMEOUT 10000

#define HOURS_TO_SECONDS (60 * 60)
#define MINUTES_TO_SECONDS (60)
#define TIDE_HOURS (6)
#define TIDE_MINUTES (12)
#define TIDE_SECONDS (30)
#define MAX_TIDES_NB 4

enum HttpDataType
{
    TIME = 0,
    WAVE = 1,
    WIND = 2,
    TIDES = 3,
    WEATHER = 4,
};
typedef enum HttpDataType HttpDataType;

#endif