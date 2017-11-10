
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include "pdqdrawable.h"
#include "util.h"

void NaiveDraw(GLuint BufferId, GLuint Offset, GLuint Range)
{
    glBindBuffer(GL_ARRAY_BUFFER, BufferId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //CheckforGlError();

    glDrawArrays(GL_TRIANGLES, Offset, Range);
    //CheckforGlError();
}


