

#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <Python.h>
#include <iostream>
#include <vector>
#include <string>
#include "shaders.h"
#include "buffers.h"
#include "pdqdrawable.h"


using std::string;
using std::shared_ptr;




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
	PyObject* Tuple = PyTuple_New(AttrCount);
	for (int a=0; a<AttrCount; a++)
	{
	    string Name = Shader->Attributes[a].Name;
	    PyTuple_SET_ITEM(Tuple, a, PyUnicode_FromString(Name.data()));
	}
	return Tuple;
    }

    Py_RETURN_NONE;
}




static PyObject* WrapCreateBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    return PyLong_FromLong(CreateBuffer(GL_ARRAY_BUFFER));
}




static PyObject* WrapDeleteBuffer(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 1)
    {
        GLuint BufferId = PyLong_AsLong(args[0]);
	DeleteBuffer(BufferId);
    }
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
    }
    Py_RETURN_NONE;
}




static PyObject* WrapNaiveDraw(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 3)
    {
        GLuint BufferId = PyLong_AsLong(args[0]);
	GLuint Offset = PyLong_AsLong(args[1]);
	GLuint Range = PyLong_AsLong(args[2]);
	NaiveDraw(BufferId, Offset, Range);
    }
    Py_RETURN_NONE;
}




static PyMethodDef ThroughputMethods[] = {
    {"setup", (PyCFunction)SetupContext, METH_FASTCALL, NULL},
    {"teardown", (PyCFunction)TeardownContext, METH_FASTCALL, NULL},
    {"swap_buffers", (PyCFunction)SwapBuffers, METH_FASTCALL, NULL},

    {"build_shader", (PyCFunction)BuildShader, METH_FASTCALL, NULL},
    {"activate_shader", (PyCFunction)ActivateShader, METH_FASTCALL, NULL},
    {"shader_attrs", (PyCFunction)GetShaderAttrs, METH_FASTCALL, NULL},

    {"create_buffer", (PyCFunction)WrapCreateBuffer, METH_FASTCALL, NULL},
    {"delete_buffer", (PyCFunction)WrapDeleteBuffer, METH_FASTCALL, NULL},
    {"fill_buffer", (PyCFunction)WrapFillBuffer, METH_FASTCALL, NULL},

    {"naive_draw", (PyCFunction)WrapNaiveDraw, METH_FASTCALL, NULL},
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
