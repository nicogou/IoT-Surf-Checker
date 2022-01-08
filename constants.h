#ifndef SURF_CHECKER_CONSTANTS_H
#define SURF_CHECKER_CONSTANTS_H

#define NB_REQUESTS 3
#define QUERY_TIMEOUT 5000
#define SWELL_NB 6

enum HttpDataType
{
    TIME = 0,
    WAVE = 1,
    WIND = 2,
};
typedef enum HttpDataType HttpDataType;

#endif