#include <Mancurya/GLBridge.hpp>

namespace Mancurya {
namespace GLBridge {
    PFNGLCREATESHADERPROC glCreateShader = nullptr;
    PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
    PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
    PFNGLATTACHSHADERPROC glAttachShader = nullptr;
    PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
    PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
    PFNGLUNIFORM1FPROC glUniform1f = nullptr;

    void init() {
        glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddressARB((const GLubyte*)"glCreateShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddressARB((const GLubyte*)"glShaderSource");
        glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddressARB((const GLubyte*)"glCompileShader");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glCreateProgram");
        glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddressARB((const GLubyte*)"glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glLinkProgram");
        glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glUseProgram");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddressARB((const GLubyte*)"glGetUniformLocation");
        glUniform1f = (PFNGLUNIFORM1FPROC)glXGetProcAddressARB((const GLubyte*)"glUniform1f");
    }
}
}
