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
        checkCompile(vs, "Vertex Shader");

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, NULL);
        glCompileShader(fs);
        checkCompile(fs, "Fragment Shader");

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        checkLink(program);
    }

    void use() { if (program) glUseProgram(program); }
    static void stop() { glUseProgram(0); }

    void setUniform1f(const char* name, float val) {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1) glUniform1f(loc, val);
    }

    void setUniform1i(const char* name, int val) {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1) glUniform1i(loc, val);
    }

    void setUniform3f(const char* name, float x, float y, float z) {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1) glUniform3f(loc, x, y, z);
    }

private:
    void checkCompile(GLuint shader, const char* name) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, NULL, log);
            fprintf(stderr, "ERROR::%s::COMPILATION_FAILED\n%s\n", name, log);
        }
    }

    void checkLink(GLuint prog) {
        GLint success;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(prog, 512, NULL, log);
            fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n%s\n", log);
        }
    }

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
