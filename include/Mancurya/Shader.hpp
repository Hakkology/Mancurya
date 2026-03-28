#ifndef MANCURYA_SHADER_HPP
#define MANCURYA_SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

namespace Mancurya {

class Shader {
public:
    GLuint program;
    Shader() : program(0) {}
    
    void load(const char* vPath, const char* fPath) {
        std::string vSourceStr = readFile(vPath);
        std::string fSourceStr = readFile(fPath);
        const char* vSrc = vSourceStr.c_str();
        const char* fSrc = fSourceStr.c_str();

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vSrc, NULL);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, NULL);
        glCompileShader(fs);

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
    }

    void use() { if (program) glUseProgram(program); }
    static void stop() { glUseProgram(0); }
    
    void setUniform1f(const char* name, float val) {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1) glUniform1f(loc, val);
    }

private:
    std::string readFile(const char* path) {
        std::ifstream f(path);
        if (!f.is_open()) return "";
        std::stringstream b;
        b << f.rdbuf();
        return b.str();
    }
};

}

#endif
