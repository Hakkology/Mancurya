#ifndef MANCURYA_ENVIRONMENT_HPP
#define MANCURYA_ENVIRONMENT_HPP

#include <Mancurya/Types.hpp>
#include <vector>

namespace Mancurya {

class Environment {
public:
    std::vector<Star> stars;
    
    void init();
    void draw(float storyTimer);

private:
    void drawCircle(float cx, float cy, float r, int segs, float red, float grn, float blu);
};

}

#endif
