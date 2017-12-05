
#include "gl_api.h"
#include "object_handle.h"
#include "painter.h"
#include "shaders.h"
#include "buffers.h"
#include "util.h"

using std::make_shared;
using BindingInterface = std::function<void()>;




PYTHON_API(BindAttributeBuffer)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);
    GLuint AttrIndex = PyLong_AsLong(args[1]);
    GLint VectorSize = PyLong_AsLong(args[2]);
    GLenum Type = GL_FLOAT;
    GLboolean Normalized = false;
    GLsizei Stride = 0;

    return WrapLambda<BindingInterface>([=]()
    {
	glBindBuffer(GL_ARRAY_BUFFER, Buffer->BufferId);
	glEnableVertexAttribArray(AttrIndex);
	glVertexAttribPointer(AttrIndex, VectorSize, Type, Normalized, Stride, 0);
	CheckforGlError();
    });
}




PYTHON_API(BindUniformBuffer)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);
    UniformBlock* BlockPtr = (UniformBlock*) PyLong_AsLong(args[1]);
    // TODO : BlockPtr should probably be replaced with some kind of
    // user handle object.  Weak ref to program on the block object,
    // but the binding could hold a shared pointer ok.
    GLuint ProgramId = BlockPtr->ProgramId;
    GLuint ProgramBlockIndex = BlockPtr->BlockIndex;
    GLintptr Offset = 0;
    GLsizeiptr Size = BlockPtr->BufferSize;
    return WrapLambda<BindingInterface>([=]()
    {
	glUniformBlockBinding(ProgramId, ProgramBlockIndex, ProgramBlockIndex);
	glBindBufferRange(GL_UNIFORM_BUFFER, ProgramBlockIndex, Buffer->BufferId, Offset, Size);
	CheckforGlError();
    });
}




PYTHON_API(BindDrawArrays)
{
    GLenum PrimitiveType = GL_TRIANGLES;
    GLuint Offset = PyLong_AsLong(args[0]);
    GLuint Range = PyLong_AsLong(args[1]);
    return WrapLambda<BindingInterface>([=]()
    {
	glDrawArrays(PrimitiveType, Offset, Range);
	CheckforGlError();
    });
}




PYTHON_API(BatchDraw)
{
    for (int i=0; i<nargs; i++)
    {
	(*AccessObject<BindingInterface>(args[i]))();
    }
    Py_RETURN_NONE;
}
