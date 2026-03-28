#ifndef MANCURYA_SEA_HPP
#define MANCURYA_SEA_HPP

#include <Mancurya/Shader.hpp>
#include <Mancurya/Types.hpp>
#include <vector>

namespace Mancurya {

class Sea {
public:
    Shader shader;
    float waveOffset = 0.0f;
    
    void init();
    void update();
    void draw(const std::vector<Explosion>& explosions);
};

}

#endif
