#pragma once

#include "gl_api.h"
#include "python_api.h"


#define BUFFER_BINDINGS \
    {"create_buffer", (PyCFunction)WrapCreateBuffer, METH_FASTCALL, NULL}, \
    {"fill_buffer", (PyCFunction)WrapFillBuffer, METH_FASTCALL, NULL}, \
    {"fill_uniform_block", (PyCFunction)WrapFillUniformBlock, METH_FASTCALL, NULL},


class BufferObject
{
public:
    BufferObject(GLenum _BufferType)
    {
	glGenBuffers(1, &BufferId);
	BufferType = _BufferType;
    }
    ~BufferObject()
    {
	glDeleteBuffers(1, &BufferId);
    }
    GLuint BufferId;
    GLenum BufferType;
};


PYTHON_API(WrapCreateBuffer);
PYTHON_API(WrapFillBuffer);
PYTHON_API(WrapFillUniformBlock);
