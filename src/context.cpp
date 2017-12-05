
#include "gl_api.h"
#include "context.h"
#include <Python.h>


bool bSetupCompleted = false;
GLFWwindow* window;




void WindowCloseCallback(GLFWwindow* window)
{
    PyErr_SetInterrupt();   
}




PYTHON_API(SetupContext)
{
    if (!bSetupCompleted)
    {
        if (!glfwInit())
	{
	    Py_RETURN_NONE;
	}

#if USING_GL_ES_3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif USING_GL_4_2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
	    Py_RETURN_NONE;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowCloseCallback(window, WindowCloseCallback);

#if USING_GL_4_2
	GLenum GlewError = glewInit();
	if (GlewError != GLEW_OK)
	{
	    PyErr_SetString(PyExc_RuntimeError, "Glew failed to initialize.\n");
	    glfwTerminate();
	    Py_RETURN_NONE;
	}
	if (!GLEW_ARB_ES3_compatibility)
	{
	    PyErr_SetString(PyExc_RuntimeError, "ES3 compatibility extension not available.\n");
	    glfwTerminate();
	    Py_RETURN_NONE;
	}
#endif
	
        bSetupCompleted = true;
    }
    Py_RETURN_NONE;
}




PYTHON_API(TeardownContext)
{
    if (bSetupCompleted)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        bSetupCompleted = false;
    }
    Py_RETURN_NONE;
}




PYTHON_API(SwapBuffers)
{
    glfwSwapBuffers(window);
    glfwPollEvents();
    Py_RETURN_NONE;
}




PYTHON_API(Clear)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Py_RETURN_NONE;
}
