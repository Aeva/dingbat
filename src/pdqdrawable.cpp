
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include "pdqdrawable.h"
#include "util.h"

void NaiveDraw(GLuint *BufferIds, GLuint Offset, GLuint Range)
{
    for (GLuint Attr = 0; BufferIds[Attr] != 0; Attr++) 
    {
	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[Attr]);
	glEnableVertexAttribArray(Attr);
	glVertexAttribPointer(Attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
	CheckforGlError();
    }
    glDrawArrays(GL_TRIANGLES, Offset, Range);
    CheckforGlError();
}


