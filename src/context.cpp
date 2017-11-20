
#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#include "context.h"


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
