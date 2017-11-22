
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>

#include "object_handle.h"

#include <iostream>
#include <functional>
#include <vector>

#include "painter.h"
#include "shaders.h"
#include "buffers.h"
#include "util.h"

using BindingInterface = std::function<void()>;
using BindingList = std::vector<BindingInterface>;

BindingList SavedBindings;




int RegisterBinding(BindingInterface Binding)
{
    SavedBindings.push_back(Binding);
    return SavedBindings.size() -1;
}




int BindAttributeBuffer(
    GLuint BufferId,
    GLuint AttrIndex,
    GLint VectorSize,
    GLenum Type,
    GLboolean Normalized,
    GLsizei Stride)
{
    return RegisterBinding([=]()
    {
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glEnableVertexAttribArray(AttrIndex);
	glVertexAttribPointer(AttrIndex, VectorSize, Type, Normalized, Stride, 0);
	CheckforGlError();
    });
}




int BindUniformBuffer(
    GLuint BufferId,
    GLuint ProgramId,
    GLuint ProgramBlockIndex,
    GLintptr Offset,
    GLsizeiptr Size)
{
    return RegisterBinding([=]()
    {
	glUniformBlockBinding(ProgramId, ProgramBlockIndex, ProgramBlockIndex);
	glBindBufferRange(GL_UNIFORM_BUFFER, ProgramBlockIndex, BufferId, Offset, Size);
	CheckforGlError();
    });
}




int BindDrawArrays(
    GLenum PrimitiveType,
    GLuint Offset,
    GLuint Range)
{
    return RegisterBinding([=]()
    {
	glDrawArrays(PrimitiveType, Offset, Range);
	CheckforGlError();
    });
}




// TODO: replace the indices and SavedBindings with pointers and some allocator
void BatchDraw(int* Batch, int Count)
{
    for (int i=0; i<Count; i++)
    {
	SavedBindings[Batch[i]]();
    }
}




PYTHON_API(WrapBindAttributeBuffer)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);
    GLuint AttrIndex = PyLong_AsLong(args[1]);
    GLint VectorSize = PyLong_AsLong(args[2]);
    GLenum Type = GL_FLOAT;
    GLboolean Normalized = false;
    GLsizei Stride = 0;
    int Handle = BindAttributeBuffer(Buffer->BufferId, AttrIndex, VectorSize, Type, Normalized, Stride);
    return PyLong_FromLong(Handle);
}




PYTHON_API(WrapBindUniformBuffer)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);
    UniformBlock* BlockPtr = (UniformBlock*) PyLong_AsLong(args[1]);
    int Handle = BindUniformBuffer(Buffer->BufferId, BlockPtr->ProgramId, BlockPtr->BlockIndex, 0, BlockPtr->BufferSize);
    return PyLong_FromLong(Handle);
}




PYTHON_API(WrapBindDrawArrays)
{
    if (nargs == 2)
    {
	GLenum PrimitiveType = GL_TRIANGLES;
	GLuint Offset = PyLong_AsLong(args[0]);
	GLuint Range = PyLong_AsLong(args[1]);
	int Handle = BindDrawArrays(PrimitiveType, Offset, Range);
	return PyLong_FromLong(Handle);
    }

    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




PYTHON_API(WrapBatchDraw)
{
    int* Batch = (int*)malloc(sizeof(int) * nargs);
    for (int i=0; i<nargs; i++)
    {
	Batch[i] = PyLong_AsLong(args[i]);
    }
    BatchDraw(Batch, nargs);
    free(Batch);
    Py_RETURN_NONE;
}
