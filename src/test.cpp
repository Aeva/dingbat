

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


void ShaderError(GLenum ShaderType, const char* ErrorMessage)
{
  char* TypeString = (char*)"Unknown";
    if (ShaderType == GL_VERTEX_SHADER)
    {
	TypeString = (char*)"Vertex";
    }
    if (ShaderType == GL_FRAGMENT_SHADER)
    {
	TypeString = (char*)"Fragment";
    }

    PyObject* ErrorString = PyUnicode_FromFormat("%s shader failed to compile: %s",
						 TypeString, ErrorMessage);
    PyErr_SetObject(PyExc_RuntimeError, ErrorString);
}


GLuint CompileShader(const char* ShaderSource, GLenum ShaderType)
{
    GLuint ShaderId = glCreateShader(ShaderType);
    if (!ShaderId)
    {
        ShaderError(ShaderType, "glCreateShader returned 0.");
	return 0;
    }
    
    glShaderSource(ShaderId, 1, &ShaderSource, NULL);
    glCompileShader(ShaderId);

    GLint CompiledOk;
    glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &CompiledOk);
    if (!CompiledOk)
    {

        GLint ShaderLogLength = 0;
        glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &ShaderLogLength);
	if (ShaderLogLength)
	{
            char* ShaderLog = (char*)malloc(sizeof(char) * ShaderLogLength);
	    glGetShaderInfoLog(ShaderId, ShaderLogLength, NULL, ShaderLog);
	    ShaderError(ShaderType, ShaderLog);
	    free(ShaderLog);
	}
	else
	{
            ShaderError(ShaderType, "unkown error.");
	}

	glDeleteShader(ShaderId);
	return 0;
    }
    return ShaderId;
}


static PyObject* BuildShader(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    if (nargs == 2)
    {
        const char* VertexSource = (const char*)PyUnicode_DATA(args[0]);
        const char* FragmentSource = (const char*)PyUnicode_DATA(args[1]);
	
	GLuint VertexShaderId = CompileShader(VertexSource, GL_VERTEX_SHADER);
	if (!VertexShaderId)
	{
            Py_RETURN_NONE;
	}
	GLuint FragmentShaderId = CompileShader(FragmentSource, GL_FRAGMENT_SHADER);
	if (!FragmentShaderId)
	{
            Py_RETURN_NONE;
	}

	GLuint ShaderProgramId = glCreateProgram();
	glAttachShader(ShaderProgramId, VertexShaderId);
	glAttachShader(ShaderProgramId, FragmentShaderId);
	glLinkProgram(ShaderProgramId);

	// TODO: validate program, raise if there is an error.
	
	return PyLong_FromLong(ShaderProgramId);
    }

    Py_RETURN_NONE;
}


static PyObject* ActivateShader(PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    glUseProgram(ShaderProgramId);
    Py_RETURN_NONE;
}


static PyMethodDef ThroughputMethods[] = {
    {"setup", (PyCFunction)SetupContext, METH_FASTCALL, NULL},
    {"teardown", (PyCFunction)TeardownContext, METH_FASTCALL, NULL},
    {"swap_buffers", (PyCFunction)SwapBuffers, METH_FASTCALL, NULL},
    {"build_shader", (PyCFunction)BuildShader, METH_FASTCALL, NULL},
    {"activate_shader", (PyCFunction)ActivateShader, METH_FASTCALL, NULL},
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
