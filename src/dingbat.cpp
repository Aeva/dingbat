

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include <Python.h>
#include <iostream>
#include <vector>
#include <string>
#include "shaders.h"
#include "buffers.h"
#include "painter.h"
#include "util.h"


using std::string;
using std::shared_ptr;
using std::vector;




void WindowCloseCallback(GLFWwindow* window)
{
    PyErr_SetInterrupt();   
}




bool bSetupCompleted = false;
GLFWwindow* window;
static PyObject* SetupContext(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (!bSetupCompleted)
    {
        if (!glfwInit())
	{
	    Py_RETURN_NONE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
	    Py_RETURN_NONE;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowCloseCallback(window, WindowCloseCallback);
        bSetupCompleted = true;
    }
    Py_RETURN_NONE;
}




static PyObject* TeardownContext(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (bSetupCompleted)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        bSetupCompleted = false;
    }
    Py_RETURN_NONE;
}




static PyObject* SwapBuffers(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (bSetupCompleted)
    {
        glfwSwapBuffers(window);
	glfwPollEvents();
    }
    Py_RETURN_NONE;
}




static PyObject* BuildShader(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 2)
    {
	const string VertexSource = string((const char*)PyUnicode_DATA(args[0]));
	const string FragmentSource = string((const char*)PyUnicode_DATA(args[1]));
	return PyLong_FromLong(BuildShaderProgram(VertexSource, FragmentSource)->ProgramId);
    }
    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




static PyObject* ActivateShader(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    glUseProgram(ShaderProgramId);
    Py_RETURN_NONE;
}




static PyObject* GetShaderAttrs(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    shared_ptr<ShaderProgram> Shader = GetShaderProgram(ShaderProgramId);
    
    if (Shader)
    {
	int AttrCount = Shader->Attributes.size();
	PyObject* Dict = PyDict_New();
	for (int a=0; a<AttrCount; a++)
	{
	    string Name = Shader->Attributes[a].Name;
	    PyDict_SetItemString(Dict, Name.data(), PyLong_FromLong(a));
	}
	return Dict;
    }

    Py_RETURN_NONE;
}




static PyObject* GetShaderUniformBlocks(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    shared_ptr<ShaderProgram> Shader = GetShaderProgram(ShaderProgramId);
    
    if (Shader)
    {
	int BlockCount = Shader->UniformBlocks.size();
	PyObject* Dict = PyDict_New();
	for (int b=0; b<BlockCount; b++)
	{
	    string BlockName = Shader->UniformBlocks[b].Name;
	    // int UniCount = Shader->UniformBlocks[b].Uniforms.size();
	    // PyObject* Tuple = PyTuple_New(UniCount);
	    // for (int u=0; u<UniCount; u++)
	    // {
	    // 	string UniName = Shader->UniformBlocks[b].Uniforms[u].Name;
	    // 	PyTuple_SET_ITEM(Tuple, u, PyUnicode_FromString(UniName.data()));
	    // }
	    // PyDict_SetItemString(Dict, BlockName.data(), Tuple);

	    // return address to uniform block
	    long BlockAddress = (long)&(Shader->UniformBlocks[b]);
	    PyDict_SetItemString(Dict, BlockName.data(), PyLong_FromLong(BlockAddress));
	}
	return Dict;
    }

    Py_RETURN_NONE;
}




static PyObject* WrapCreateBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    return PyLong_FromLong(CreateBuffer(GL_ARRAY_BUFFER));
    Py_RETURN_NONE;
}




static PyObject* WrapDeleteBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 1)
    {
        GLuint BufferId = PyLong_AsLong(args[0]);
	DeleteBuffer(BufferId);
    }
    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




static PyObject* WrapFillBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 2)
    {
        GLuint BufferId = PyLong_AsLong(args[0]);

	Py_buffer DataView;
	int BufferState = PyObject_GetBuffer(args[1], &DataView, PyBUF_CONTIG_RO);
	if (BufferState > -1)
	{
	    // item count == DataView.len / DataView.itemsize
	    FillBuffer(BufferId, DataView.len, DataView.buf, GL_STATIC_DRAW);
	    PyBuffer_Release(&DataView);
	}
	Py_RETURN_NONE;
    }
    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




static PyObject* WrapFillUniformBlock(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint BufferId = PyLong_AsLong(args[0]);
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
    
    FillBuffer(BufferId, BufferSize, Blob, GL_DYNAMIC_DRAW);
    free(Blob);
    CheckforGlError();
    Py_RETURN_NONE;
}




static PyObject* WrapBindAttributeBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 3)
    {
	GLuint BufferId = PyLong_AsLong(args[0]);
	GLuint AttrIndex = PyLong_AsLong(args[1]);
	GLint VectorSize = PyLong_AsLong(args[2]);
	GLenum Type = GL_FLOAT;
	GLboolean Normalized = false;
	GLsizei Stride = 0;
	int Handle = BindAttributeBuffer(BufferId, AttrIndex, VectorSize, Type, Normalized, Stride);
	return PyLong_FromLong(Handle);
    }

    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




static PyObject* WrapBindUniformBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint BufferId = PyLong_AsLong(args[0]);
    UniformBlock* BlockPtr = (UniformBlock*) PyLong_AsLong(args[1]);
    int Handle = BindUniformBuffer(BufferId, BlockPtr->ProgramId, BlockPtr->BlockIndex);
    return PyLong_FromLong(Handle);
}




static PyObject* WrapBindDrawArrays(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
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




static PyObject* WrapBatchDraw(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
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




static PyMethodDef ThroughputMethods[] = {
    {"setup", (PyCFunction)SetupContext, METH_FASTCALL, NULL},
    {"teardown", (PyCFunction)TeardownContext, METH_FASTCALL, NULL},
    {"swap_buffers", (PyCFunction)SwapBuffers, METH_FASTCALL, NULL},

    {"build_shader", (PyCFunction)BuildShader, METH_FASTCALL, NULL},
    {"activate_shader", (PyCFunction)ActivateShader, METH_FASTCALL, NULL},
    {"shader_attrs", (PyCFunction)GetShaderAttrs, METH_FASTCALL, NULL},
    {"shader_uniform_blocks", (PyCFunction)GetShaderUniformBlocks, METH_FASTCALL, NULL},

    {"create_buffer", (PyCFunction)WrapCreateBuffer, METH_FASTCALL, NULL},
    {"delete_buffer", (PyCFunction)WrapDeleteBuffer, METH_FASTCALL, NULL},
    {"fill_buffer", (PyCFunction)WrapFillBuffer, METH_FASTCALL, NULL},
    {"fill_uniform_block", (PyCFunction)WrapFillUniformBlock, METH_FASTCALL, NULL},

    {"bind_attr_buffer", (PyCFunction)WrapBindAttributeBuffer, METH_FASTCALL, NULL},
    {"bind_uniform_buffer", (PyCFunction)WrapBindUniformBuffer, METH_FASTCALL, NULL},
    {"bind_draw_arrays", (PyCFunction)WrapBindDrawArrays, METH_FASTCALL, NULL},
    {"batch_draw", (PyCFunction)WrapBatchDraw, METH_FASTCALL, NULL},
    {NULL, NULL, 0, NULL}
};




static struct PyModuleDef ModuleDef = {
    PyModuleDef_HEAD_INIT,
    "dingbat",
    "",
    -1,
    ThroughputMethods
};




PyMODINIT_FUNC PyInit_dingbat(void)
{
    PyObject *Module = PyModule_Create(&ModuleDef);
    return Module;
}
