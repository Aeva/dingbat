
#include "gl_api.h"
#include "context.h"
#include <Python.h>
#include <iostream>


bool bSetupCompleted = false;
GLFWwindow* Window;




void WindowCloseCallback(GLFWwindow* Window)
{
    PyErr_SetInterrupt();   
}




void DebugCallback(GLenum Source, 
		   GLenum Type, 
		   GLuint Id, 
		   GLenum Severity, 
		   GLsizei MessageLength, 
		   const GLchar *ErrorMessage, 
		   const void *UserParam)
{
    std::cout << ErrorMessage << "\n";
    if (Severity == GL_DEBUG_SEVERITY_HIGH)
    {
	PyErr_SetString(PyExc_RuntimeError, ErrorMessage);
    }
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
#if DEBUG_BUILD
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif	
	Window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!Window)
	{
	    Py_RETURN_NONE;
	}
	glfwMakeContextCurrent(Window);
	glfwSetWindowCloseCallback(Window, WindowCloseCallback);

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
	
#if DEBUG_BUILD
	GLint ContextFlags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &ContextFlags);
	if (ContextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
	    glEnable(GL_DEBUG_OUTPUT);
	    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
	    glDebugMessageCallback(&DebugCallback, nullptr);
	    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	}
	else
	{
	    std::cout << "Debug context not available!\n";
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
        glfwDestroyWindow(Window);
        glfwTerminate();
        bSetupCompleted = false;
    }
    Py_RETURN_NONE;
}




PYTHON_API(SwapBuffers)
{
    glfwSwapBuffers(Window);
    glfwPollEvents();
    Py_RETURN_NONE;
}




PYTHON_API(Clear)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Py_RETURN_NONE;
}
