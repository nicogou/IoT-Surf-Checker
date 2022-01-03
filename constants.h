#ifndef SURF_CHECKER_CONSTANTS_H
#define SURF_CHECKER_CONSTANTS_H

#define NB_REQUESTS 3
#define QUERY_TIMEOUT 2000

enum HttpDataType
{
    TIME = 0,
    WAVE = 1,
    WIND = 2,
};
typedef enum HttpDataType HttpDataType;

#endif