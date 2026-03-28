#ifndef MANCURYA_SHADER_HPP
#define MANCURYA_SHADER_HPP

#include <Mancurya/GLBridge.hpp>
#include <string>
#include <fstream>
#include <sstream>

namespace Mancurya {

class Shader {
public:
    GLuint program;
    Shader() : program(0) {}
    
    void load(const char* vPath, const char* fPath) {
        if (!GLBridge::glCreateShader) return;
        std::string vSourceStr = readFile(vPath);
        std::string fSourceStr = readFile(fPath);
        const char* vSrc = vSourceStr.c_str();
        const char* fSrc = fSourceStr.c_str();

        GLuint vs = GLBridge::glCreateShader(GL_VERTEX_SHADER);
        GLBridge::glShaderSource(vs, 1, &vSrc, NULL);
        GLBridge::glCompileShader(vs);

        GLuint fs = GLBridge::glCreateShader(GL_FRAGMENT_SHADER);
        GLBridge::glShaderSource(fs, 1, &fSrc, NULL);
        GLBridge::glCompileShader(fs);

        program = GLBridge::glCreateProgram();
        GLBridge::glAttachShader(program, vs);
        GLBridge::glAttachShader(program, fs);
        GLBridge::glLinkProgram(program);
    }

    void use() { if (program && GLBridge::glUseProgram) GLBridge::glUseProgram(program); }
    static void stop() { if (GLBridge::glUseProgram) GLBridge::glUseProgram(0); }
    
    void setUniform1f(const char* name, float val) {
        GLint loc = GLBridge::glGetUniformLocation(program, name);
        if (loc != -1) GLBridge::glUniform1f(loc, val);
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
