

#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <Python.h>


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
    }
    Py_RETURN_NONE;
}


static PyObject* BuildShader(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 2)
    {
        const char* VertexSource = (const char*)PyUnicode_DATA(args[0]);
        const char* FragmentSource = (const char*)PyUnicode_DATA(args[1]);
	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexSource, NULL);
	glCompileShader(VertexShaderId);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentSource, NULL);
	glCompileShader(FragmentShaderId);
	GLuint ShaderProgramId = glCreateProgram();
	glAttachShader(ShaderProgramId, VertexShaderId);
	glAttachShader(ShaderProgramId, FragmentShaderId);
	glLinkProgram(ShaderProgramId);
	return PyLong_FromLong(ShaderProgramId);
    }

    Py_RETURN_NONE;
}


static PyMethodDef ThroughputMethods[] = {
    {"setup", (PyCFunction)SetupContext, METH_FASTCALL, NULL},
    {"teardown", (PyCFunction)TeardownContext, METH_FASTCALL, NULL},
    {"swap_buffers", (PyCFunction)SwapBuffers, METH_FASTCALL, NULL},
    {"build_shader", (PyCFunction)BuildShader, METH_FASTCALL, NULL},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef ModuleDef = {
    PyModuleDef_HEAD_INIT,
    "glfw_test",
    "",
    -1,
    ThroughputMethods
};


PyMODINIT_FUNC PyInit_glfw_test(void)
{
    PyObject *Module = PyModule_Create(&ModuleDef);
    return Module;
}
