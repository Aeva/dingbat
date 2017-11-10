
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <Python.h>


void RaiseError(const char* ErrorMessage)
{
    PyErr_SetString(PyExc_RuntimeError, ErrorMessage);
}


void RaiseError(const char* HintString, const char* ErrorMessage)
{
    PyObject* ErrorString = PyUnicode_FromFormat("%s Error: %s", HintString, ErrorMessage ? ErrorMessage : "(no log data)");
    PyErr_SetObject(PyExc_RuntimeError, ErrorString);
}


std::unordered_map<GLenum, const char*> ErrorStrings = {
    {GL_INVALID_ENUM,
     "INVALID_ENUM : An unacceptable value is specified for an enumerated\n"
     "argument. The offending command is ignored and has no other side\n"
     "effect than to set the error flag.\n"},
    {GL_INVALID_VALUE,
     "INVALID_VALUE : A numeric argument is out of range. The offending\n"
     "command is ignored and has no other side effect than to set the error\n"
     "flag.\n"},
    {GL_INVALID_OPERATION,
     "INVALID_OPERATION : The specified operation is not allowed in the\n"
     "current state. The offending command is ignored and has no other side\n"
     "effect than to set the error flag.\n"},
    {GL_INVALID_FRAMEBUFFER_OPERATION,
     "INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not\n"
     "complete. The offending command is ignored and has no other side\n"
     "effect than to set the error flag.\n"},
    {GL_OUT_OF_MEMORY,
     "OUT_OF_MEMORY : There is not enough memory left to execute the\n"
     "command. The state of the GL is undefined, except for the state of the\n"
     "error flags, after this error is recorded.\n"}
};


void CheckforGlError()
{
    glFlush();
    GLenum ErrorCode = glGetError();
    if (ErrorCode > 0)
    {
        RaiseError(ErrorStrings[ErrorCode]);
    }
}
