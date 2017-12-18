
#include "object_handle.h"
#include <iostream>
#include "util.h"
#include "buffers.h"
#include "shaders.h"
#include "matrix_math.h"
#include <glm.hpp>


using std::make_shared;



void FillBuffer(GLenum BufferType, GLint BufferId, GLsizeiptr Size, const GLvoid *Data, GLenum UsageHint)
{
    glBindBuffer(BufferType, BufferId);
    glBufferData(BufferType, Size, Data, UsageHint);
}




PYTHON_API(WrapCreateBuffer)
{
    return WrapObject<BufferObject>(make_shared<BufferObject>(GL_ARRAY_BUFFER));
}




PYTHON_API(WrapFillBuffer)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);

    GLuint BufferId = Buffer->BufferId;
    GLenum BufferType = Buffer->BufferType;

    Py_buffer DataView;
    int BufferState = PyObject_GetBuffer(args[1], &DataView, PyBUF_CONTIG_RO);
    if (BufferState > -1)
    {
	// item count == DataView.len / DataView.itemsize
	FillBuffer(BufferType, BufferId, DataView.len, DataView.buf, GL_STATIC_DRAW);
	PyBuffer_Release(&DataView);
    }
    Py_RETURN_NONE;
}




PYTHON_API(WrapFillUniformBlock)
{
    auto Buffer = AccessObject<BufferObject>(args[0]);
    UniformBlock* BlockPtr = (UniformBlock*) PyLong_AsLong(args[1]);

    vector<UniformEntry>* Uniforms = &(BlockPtr->Uniforms);
    GLuint BufferSize = BlockPtr->BufferSize;

    void* Blob = malloc(BufferSize);
    float* Writer = (float*) Blob;
    Py_ssize_t ArgOffset = 2;

    for (size_t i=0; i<Uniforms->size(); i++)
    {
	// TODO : guard this in a not terrible way
	if (ArgOffset >= nargs)
	{
	    break;
	}
	
	UniformEntry* Uniform = &((*Uniforms)[i]);
	if (Uniform->Type == GL_FLOAT)
	{
	    Writer[0] = PyFloat_AsDouble(args[ArgOffset]);
	    Writer++;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_VEC2)
	{
	    glm::vec2 &Vector = ((MathHandle<glm::vec2>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Vector[0];
	    Writer[1] = Vector[1];
	    Writer += 2;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_VEC3)
	{
	    glm::vec3 &Vector = ((MathHandle<glm::vec3>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Vector[0];
	    Writer[1] = Vector[1];
	    Writer[2] = Vector[2];
	    Writer += 4; // intentional
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_VEC4)
	{
	    glm::vec4 &Vector = ((MathHandle<glm::vec4>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Vector[0];
	    Writer[1] = Vector[1];
	    Writer[2] = Vector[2];
	    Writer[3] = Vector[3];
	    Writer += 4;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_MAT2)
	{
	    glm::mat2 &Matrix = ((MathHandle<glm::mat2>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Matrix[0][0];
	    Writer[1] = Matrix[0][1];
	    Writer[2] = 0.0;
	    Writer[3] = 0.0;
	    Writer[4] = Matrix[1][0];
	    Writer[5] = Matrix[1][1];
	    Writer[6] = 0.0;
	    Writer[7] = 0.0;
	    Writer += 8;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_MAT3)
	{
	    glm::mat3 &Matrix = ((MathHandle<glm::mat3>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Matrix[0][0];
	    Writer[1] = Matrix[0][1];
	    Writer[2] = Matrix[0][2];
	    Writer[3] = 0.0;
	    Writer[4] = Matrix[1][0];
	    Writer[5] = Matrix[1][1];
	    Writer[6] = Matrix[1][2];
	    Writer[7] = 0.0;
	    Writer[8] = Matrix[2][0];
	    Writer[9] = Matrix[2][1];
	    Writer[10] = Matrix[2][2];
	    Writer[11] = 0.0;
	    Writer += 12;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_MAT4)
	{
	    glm::mat4 &Matrix = ((MathHandle<glm::mat4>*)args[ArgOffset])->Wrapped;
	    Writer[0] = Matrix[0][0];
	    Writer[1] = Matrix[0][1];
	    Writer[2] = Matrix[0][2];
	    Writer[3] = Matrix[0][3];

	    Writer[4] = Matrix[1][0];
	    Writer[5] = Matrix[1][1];
	    Writer[6] = Matrix[1][2];
	    Writer[7] = Matrix[1][3];

	    Writer[8] = Matrix[2][0];
	    Writer[9] = Matrix[2][1];
	    Writer[10] = Matrix[2][2];
	    Writer[11] = Matrix[2][3];

	    Writer[12] = Matrix[3][0];
	    Writer[13] = Matrix[3][1];
	    Writer[14] = Matrix[3][2];
	    Writer[15] = Matrix[3][3];

	    Writer += 16;
	    ArgOffset++;
	}
	else
	{
	    std::cout << "Non-float uniforms not supported yet.\n";
	}
    }
    
    FillBuffer(GL_UNIFORM_BUFFER, Buffer->BufferId, BufferSize, Blob, GL_DYNAMIC_DRAW);
    free(Blob);
    Py_RETURN_NONE;
}
