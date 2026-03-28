#ifndef MANCURYA_FLEET_HPP
#define MANCURYA_FLEET_HPP

#include <Mancurya/Types.hpp>
#include <Mancurya/Shader.hpp>
#include <vector>

namespace Mancurya {

class Fleet {
public:
    std::vector<Ship> ships;
    std::vector<Projectile> projectiles;

    void init(bool isFrenchSide, float baseX);
    void update(float storyTimer, std::vector<Explosion>& explosions, std::vector<Ship>& targetFleet, bool isMancurian);
    void fire(std::vector<Ship>& targetFleet, bool isMancurian);
    void draw(bool isFrench, float storyTimer);

private:
    void drawShip(float x, float y, float w, float h, float r, float g, float b, bool isFrench);
    void drawHorse(float x, float y);
    void drawCroissant(float x, float y);
    void drawBaguetteDepot(float x, float y, bool isBurning);
};

}

#endif
