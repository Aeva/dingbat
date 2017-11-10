
#pragma once
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
};



GLuint BuildShaderProgram(const string VertexSource, const string FragmentSource);
