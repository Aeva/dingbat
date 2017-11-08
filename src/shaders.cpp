
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include "util.h"
#include "shaders.h"




//
// "CheckStatus" is an effort to DRY up the status and log fetching
// code for shader compilation.
//
// The methods "CheckShaderStatus", and "CheckProgramStatus" are
// provided as aliases for the two intended configurations.
//
typedef std::function<void(char*)> ErrorHandler;
template<void (*GetObjectIv)(GLuint, GLenum, GLint*),
	 void (*GetObjectInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*)>
bool CheckStatus(GLuint ObjectId, GLenum StatusEnum, ErrorHandler ErrorCallback)
{
    GLint StatusValue;
    GetObjectIv(ObjectId, StatusEnum, &StatusValue);
    if (!StatusValue)
    {
        GLint LogLength;
	GetObjectIv(ObjectId, GL_INFO_LOG_LENGTH, &LogLength);
        if (LogLength)
        {
            std::string ErrorLog(LogLength, 0);
	    GetObjectInfoLog(ObjectId, LogLength, NULL, (char*) ErrorLog.data());
	    ErrorCallback((char*) ErrorLog.data());
        }
	else
	{
            ErrorCallback(nullptr);
	}
        return false;
    }
    else
    {
        return true;
    }
}
inline bool CheckShaderStatus(GLuint ObjectId, GLenum StatusEnum, ErrorHandler ErrorCallback)
{
    return CheckStatus<glGetShaderiv, glGetShaderInfoLog>(ObjectId, StatusEnum, ErrorCallback);
}
inline bool CheckProgramStatus(GLuint ObjectId, GLenum StatusEnum, ErrorHandler ErrorCallback)
{
    return CheckStatus<glGetProgramiv, glGetProgramInfoLog>(ObjectId, StatusEnum, ErrorCallback);
}




const char* ShaderTypeHint(GLenum ShaderType)
{
    if (ShaderType == GL_VERTEX_SHADER)
    {
        return "Vertex Shader";
    }
    if (ShaderType == GL_FRAGMENT_SHADER)
    {
        return "Fragment Shader";
    }
    return "Unknown Shader Type";
}




//
// Compiles a shader stage, and returns the handle if successful or
// zero if compilation failed.  If failed, this will also raise a
// python error message.
//
GLuint CompileShader(const char* ShaderSource, GLenum ShaderType)
{
    GLuint ShaderId = glCreateShader(ShaderType);
    if (!ShaderId)
    {
        RaiseError(ShaderTypeHint(ShaderType), "glCreateShader returned 0.");
	return 0;
    }
    glShaderSource(ShaderId, 1, &ShaderSource, NULL);
    glCompileShader(ShaderId);

    CheckShaderStatus(ShaderId, GL_COMPILE_STATUS, [&ShaderId, &ShaderType](const char* ErrorLog) {
	glDeleteShader(ShaderId);
        ShaderId = 0;
	RaiseError(ShaderTypeHint(ShaderType), ErrorLog);
    });

    return ShaderId;
}




//
// Links a shader program, and returns the handle if successful or
// zero if link or validation failed.  If failed, this will also raise
// a python error message.
//
GLuint LinkShaderProgram(GLuint VertexShaderId, GLuint FragmentShaderId)
{
    GLuint ProgramId = glCreateProgram();
    if (!ProgramId)
    {
        RaiseError("glCreateProgram returned 0.");
	return 0;
    }
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

    CheckProgramStatus(ProgramId, GL_LINK_STATUS, [&ProgramId](const char* ErrorLog) {
	glDeleteProgram(ProgramId);
        ProgramId = 0;
	RaiseError("Shader Program Linker", ErrorLog);
    });

    if (!ProgramId)
    {
        return 0;
    }

    glValidateProgram(ProgramId);
    CheckProgramStatus(ProgramId, GL_VALIDATE_STATUS, [&ProgramId](const char* ErrorLog) {
	glDeleteProgram(ProgramId);
        ProgramId = 0;
	RaiseError("Shader Program Validation", ErrorLog);
    });

    return ProgramId;
}




//
// Shader program compilation interface.  Returns the program handle
// if successful, otherwise returns zero and raises an error.
//
GLuint BuildShaderProgram(const char* VertexSource, const char* FragmentSource)
{
    GLuint VertexShaderId = CompileShader(VertexSource, GL_VERTEX_SHADER);
    if (!VertexShaderId)
    {
        return 0;
    }
    GLuint FragmentShaderId = CompileShader(FragmentSource, GL_FRAGMENT_SHADER);
    if (!FragmentShaderId)
    {
        return 0;
    }
    GLuint ShaderProgramId = LinkShaderProgram(VertexShaderId, FragmentShaderId);
    if (!ShaderProgramId)
    {
        return 0;
    }
    return ShaderProgramId;
}
