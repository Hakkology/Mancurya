#ifndef MANCURYA_FLEETMANAGER_HPP
#define MANCURYA_FLEETMANAGER_HPP

#include <Mancurya/Types.hpp>
#include <Mancurya/Shader.hpp>
#include <vector>

namespace Mancurya {

class FleetManager {
public:
    std::vector<Ship> mancurian, french;
    std::vector<Projectile> mancurianBombs, frenchBombs;
    std::vector<Explosion> explosions;
    Shader projectileShader;
    
    void init();
    void update(float storyTimer);
    void fire();
    void draw(float storyTimer);

private:
    void drawShip(float x, float y, float w, float h, float r, float g, float b, bool isFrench);
    void drawHorse(float x, float y);
    void drawCroissant(float x, float y);
    void drawBaguetteDepot(float x, float y, bool isBurning);
};

}

#endif
