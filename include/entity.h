#ifndef ENTITY_H_
#define ENTITY_H_

#include "common.h"

struct entity {
    struct point p;
    char        *disp_ch;
    bool         solid;
    bool         visible;
};

#endif // ENTITY_H_
