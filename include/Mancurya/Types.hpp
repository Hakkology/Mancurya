#ifndef MANCURYA_TYPES_HPP
#define MANCURYA_TYPES_HPP

#include <vector>

namespace Mancurya {
    const float PI = 3.1415926535f;
    const float GRAVITY = -0.003f;

    struct Ship {
        float x, y, z, baseX, scale = 1.0f;
        bool isBurning = false, isShooting = true;
    };

    struct Projectile {
        float x, y, z, vx, vy, vz;
        bool isExploded = false;
    };

    struct Explosion { float x, y, z; int timer; };
    struct Star { float x, y, size; };
    struct Particle { float x, y, vx, vy, life; };
}

#endif
