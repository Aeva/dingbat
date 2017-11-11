
#pragma once
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>
#include <unordered_map>
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
    GLint Slot;
    GLint Size;
    GLenum Type;
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

    std::unordered_map<string, Attribute> Attributes;
};



GLuint BuildShaderProgram(const string VertexSource, const string FragmentSource);
