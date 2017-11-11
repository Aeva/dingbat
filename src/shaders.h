
#pragma once
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <memory>


using std::string;
using std::shared_ptr;




class ShaderStage
{
public:
    ShaderStage(const string ShaderSource, const GLenum ShaderType);
    ~ShaderStage();
    GLuint ShaderId;
    GLenum ShaderType;
    size_t ShaderHash;
    string ErrorString;
    bool bIsValid;
};




struct Attribute
{
    string Name;
    GLenum Type;
    GLint Slot;
    GLint Size;
};




class ShaderProgram
{
public:
    ShaderProgram(const string VertexSource, const string FragmentSource);
    ~ShaderProgram();
    GLuint ProgramId;
    string ErrorString;
    bool bIsValid;

    shared_ptr<ShaderStage> VertexShader;
    shared_ptr<ShaderStage> FragmentShader;

    std::vector<Attribute> Attributes;
};



shared_ptr<ShaderProgram> BuildShaderProgram(const string VertexSource, const string FragmentSource);
shared_ptr<ShaderProgram> GetShaderProgram(GLuint ProgramId);
