#ifndef MANCURYA_SEA_HPP
#define MANCURYA_SEA_HPP

#include <Mancurya/Shader.hpp>

namespace Mancurya {

class Sea {
public:
    Shader shader;
    float waveOffset = 0.0f;
    
    void init();
    void update();
    void draw();
};

}

#endif
