#ifndef MANCURYA_FLEETMANAGER_HPP
#define MANCURYA_FLEETMANAGER_HPP

#include <Mancurya/Fleet.hpp>
#include <Mancurya/Shader.hpp>
#include <vector>

namespace Mancurya {

class FleetManager {
public:
    Fleet mancurianFleet;
    Fleet frenchFleet;
    std::vector<Explosion> explosions;
    Shader projectileShader;
    
    void init();
    void update(float storyTimer);
    void fire();
    void draw(float storyTimer);
};

}

#endif
