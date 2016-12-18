#pragma once

#include <math/Vector.h>

namespace pg {

// Axis-aligned box
struct AABox {
    Vec3f min;
    Vec3f max;

    Vec3f center() const {
        return 0.5f*(max + min);
    }
};

}