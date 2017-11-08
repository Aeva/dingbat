
#pragma once


GLuint CreateBuffer(GLenum BufferType);
void DeleteBuffer(GLuint BufferId);
void FillBuffer(GLint BufferId, GLsizeiptr Size, const GLvoid *Data, GLenum UsageHint);
