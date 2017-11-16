
#pragma once

#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>


using BindingInterface = std::function<void()>;
using BindingList = std::vector<BindingInterface>;




int BindAttributeBuffer(
    GLuint BufferId,
    GLuint AttrIndex,
    GLint VectorSize,
    GLenum Type,
    GLboolean Normalized,
    GLsizei Stride);




int BindUniformBuffer(
    GLuint BufferId,
    GLuint ProgramId,
    GLuint ProgramBlockIndex,
    GLintptr Offset,
    GLsizeiptr Size);




int BindDrawArrays(
    GLenum PrimitiveType,
    GLuint Offset,
    GLuint Range);




void BatchDraw(int* Batch, int Count);
