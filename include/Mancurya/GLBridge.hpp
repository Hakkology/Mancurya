#ifndef MANCURYA_GLBRIDGE_HPP
#define MANCURYA_GLBRIDGE_HPP

#include <GL/gl.h>
#include <GL/glx.h>

namespace Mancurya {
namespace GLBridge {
    typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
    typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
    typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
    typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
    typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
    typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
    typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
    typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
    typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);

    extern PFNGLCREATESHADERPROC glCreateShader;
    extern PFNGLSHADERSOURCEPROC glShaderSource;
    extern PFNGLCOMPILESHADERPROC glCompileShader;
    extern PFNGLCREATEPROGRAMPROC glCreateProgram;
    extern PFNGLATTACHSHADERPROC glAttachShader;
    extern PFNGLLINKPROGRAMPROC glLinkProgram;
    extern PFNGLUSEPROGRAMPROC glUseProgram;
    extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    extern PFNGLUNIFORM1FPROC glUniform1f;

    void init();
}
}

#endif
