
#include "object_handle.h"
#include <iostream>
#include "util.h"
#include "buffers.h"
#include "shaders.h"


using std::make_shared;



void FillBuffer(GLenum BufferType, GLint BufferId, GLsizeiptr Size, const GLvoid *Data, GLenum UsageHint)
{
    glBindBuffer(BufferType, BufferId);
    // CheckforGlError();
    glBufferData(BufferType, Size, Data, UsageHint);
    // CheckforGlError();
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
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer++;
	    ArgOffset++;
	}
	else if (Uniform->Type == GL_FLOAT_VEC2)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer += 2;
	    ArgOffset += 2;
	}
	else if (Uniform->Type == GL_FLOAT_VEC3)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer[2] = (float)PyFloat_AsDouble(args[ArgOffset+2]);
	    Writer += 4; // intentional
	    ArgOffset += 3;
	}
	else if (Uniform->Type == GL_FLOAT_VEC4)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer[2] = (float)PyFloat_AsDouble(args[ArgOffset+2]);
	    Writer[3] = (float)PyFloat_AsDouble(args[ArgOffset+3]);
	    Writer += 4;
	    ArgOffset += 4;
	}
	else if (Uniform->Type == GL_FLOAT_MAT2)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer[2] = 0.0;
	    Writer[3] = 0.0;
	    Writer[4] = (float)PyFloat_AsDouble(args[ArgOffset+2]);
	    Writer[5] = (float)PyFloat_AsDouble(args[ArgOffset+3]);
	    Writer[6] = 0.0;
	    Writer[7] = 0.0;
	    Writer += 8;
	    ArgOffset += 4;
	}
	else if (Uniform->Type == GL_FLOAT_MAT3)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer[2] = (float)PyFloat_AsDouble(args[ArgOffset+2]);
	    Writer[3] = 0.0;
	    Writer[4] = (float)PyFloat_AsDouble(args[ArgOffset+3]);
	    Writer[5] = (float)PyFloat_AsDouble(args[ArgOffset+4]);
	    Writer[6] = (float)PyFloat_AsDouble(args[ArgOffset+5]);
	    Writer[7] = 0.0;
	    Writer[8] = (float)PyFloat_AsDouble(args[ArgOffset+6]);
	    Writer[9] = (float)PyFloat_AsDouble(args[ArgOffset+7]);
	    Writer[10] = (float)PyFloat_AsDouble(args[ArgOffset+8]);
	    Writer[11] = 0.0;
	    Writer += 12;
	    ArgOffset += 6;
	}
	else if (Uniform->Type == GL_FLOAT_MAT4)
	{
	    Writer[0] = (float)PyFloat_AsDouble(args[ArgOffset]);
	    Writer[1] = (float)PyFloat_AsDouble(args[ArgOffset+1]);
	    Writer[2] = (float)PyFloat_AsDouble(args[ArgOffset+2]);
	    Writer[3] = (float)PyFloat_AsDouble(args[ArgOffset+3]);

	    Writer[4] = (float)PyFloat_AsDouble(args[ArgOffset+4]);
	    Writer[5] = (float)PyFloat_AsDouble(args[ArgOffset+5]);
	    Writer[6] = (float)PyFloat_AsDouble(args[ArgOffset+6]);
	    Writer[7] = (float)PyFloat_AsDouble(args[ArgOffset+7]);

	    Writer[8] = (float)PyFloat_AsDouble(args[ArgOffset+8]);
	    Writer[9] = (float)PyFloat_AsDouble(args[ArgOffset+9]);
	    Writer[10] = (float)PyFloat_AsDouble(args[ArgOffset+10]);
	    Writer[11] = (float)PyFloat_AsDouble(args[ArgOffset+11]);

	    Writer[12] = (float)PyFloat_AsDouble(args[ArgOffset+12]);
	    Writer[13] = (float)PyFloat_AsDouble(args[ArgOffset+13]);
	    Writer[14] = (float)PyFloat_AsDouble(args[ArgOffset+14]);
	    Writer[15] = (float)PyFloat_AsDouble(args[ArgOffset+15]);

	    Writer += 12;
	    ArgOffset += 16;
	}
	else
	{
	    std::cout << "Non-float uniforms not supported yet.\n";
	}
    }
    
    FillBuffer(GL_UNIFORM_BUFFER, Buffer->BufferId, BufferSize, Blob, GL_DYNAMIC_DRAW);
    free(Blob);
    CheckforGlError();
    Py_RETURN_NONE;
}
