
#define GLFW_INCLUDE_ES31
#include <iostream>
#include "painter.h"
#include "util.h"


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
