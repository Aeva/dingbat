
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "util.h"
#include "buffers.h"


std::unordered_map<GLuint, GLenum> BufferTypeMap;


GLuint CreateBuffer(GLenum BufferType)
{
    GLuint BufferId;
    glGenBuffers(1, &BufferId);
    // CheckforGlError();
    BufferTypeMap[BufferId] = BufferType;
    return BufferId;
}


void DeleteBuffer(GLuint BufferId)
{
    BufferTypeMap.erase(BufferId);
    glDeleteBuffers(1, &BufferId);
    // CheckforGlError();
    return;
}


void FillBuffer(GLint BufferId, GLsizeiptr Size, const GLvoid *Data, GLenum UsageHint)
{
    GLenum BufferType = BufferTypeMap[BufferId];
    glBindBuffer(BufferType, BufferId);
    // CheckforGlError();
    glBufferData(BufferType, Size, Data, UsageHint);
    // CheckforGlError();
}
