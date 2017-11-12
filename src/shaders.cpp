
#include <functional>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "util.h"
#include "shaders.h"

using std::shared_ptr;
using std::make_shared;
using std::string;


std::unordered_map<GLuint, shared_ptr<ShaderProgram>> ShaderPrograms;




std::hash<std::string> StringHasher;
size_t ShaderStageHash(const string ShaderSource, const GLenum ShaderType)
{
    string TypeString;
    if (ShaderType == GL_VERTEX_SHADER)
    {
	TypeString = "VS";
    }
    else if (ShaderType == GL_FRAGMENT_SHADER)
    {
	TypeString = "FS";
    }
    return StringHasher(TypeString + ShaderSource);
}



//
// "CheckStatus" is an effort to DRY up the status and log fetching
// code for shader compilation.
//
// The methods "CheckShaderStatus", and "CheckProgramStatus" are
// provided as aliases for the two intended configurations.
//
template<void (*GetObjectIv)(GLuint, GLenum, GLint*),
	 void (*GetObjectInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*)>
string CheckStatus(GLuint ObjectId, GLenum StatusEnum)
{
    GLint StatusValue;
    GetObjectIv(ObjectId, StatusEnum, &StatusValue);
    if (!StatusValue)
    {
        GLint LogLength;
	GetObjectIv(ObjectId, GL_INFO_LOG_LENGTH, &LogLength);
        if (LogLength)
        {
            string ErrorLog(LogLength, 0);
	    GetObjectInfoLog(ObjectId, LogLength, NULL, (char*) ErrorLog.data());
	    return ErrorLog;
        }
	else
	{
	    return string("An unknown error occured.");
	}
    }
    return string();
}
// Aliases
string (&CheckShaderStatus)(GLuint, GLenum) = CheckStatus<glGetShaderiv, glGetShaderInfoLog>;
string (&CheckProgramStatus)(GLuint, GLenum) = CheckStatus<glGetProgramiv, glGetProgramInfoLog>;




ShaderStage::ShaderStage(const string ShaderSource, const GLenum _ShaderType)
{
    bIsValid = false;
    ShaderType = _ShaderType;
    ShaderHash = ShaderStageHash(ShaderSource, ShaderType);
    ShaderId = glCreateShader(ShaderType);
    if (ShaderId)
    {
	const GLchar* SourcePtr = ShaderSource.data();
	glShaderSource(ShaderId, 1, &SourcePtr, NULL);
	glCompileShader(ShaderId);
	ErrorString = CheckShaderStatus(ShaderId, GL_COMPILE_STATUS);
	bIsValid = ErrorString.empty();
    }
    else
    {
	CheckforGlError();
	ErrorString = string("glCreateShader returned 0.");
    }

    std::cout << "Created shader stage " << ShaderId << "\n";
}




ShaderStage::~ShaderStage()
{
    if (ShaderId)
    {
	glDeleteShader(ShaderId);
    }

    std::cout << "Deleted shader stage " << ShaderId << "\n";
}




ShaderProgram::ShaderProgram(const string VertexSource, const string FragmentSource)
{
    bIsValid = false;
    ProgramId = 0;
    VertexShader = make_shared<ShaderStage>(VertexSource, GL_VERTEX_SHADER);
    FragmentShader = make_shared<ShaderStage>(FragmentSource, GL_FRAGMENT_SHADER);

    if (VertexShader->bIsValid && FragmentShader->bIsValid)
    {
	ProgramId = glCreateProgram();
	if (ProgramId)
	{
	    glAttachShader(ProgramId, VertexShader->ShaderId);
	    glAttachShader(ProgramId, FragmentShader->ShaderId);

	    glLinkProgram(ProgramId);
	    ErrorString = CheckProgramStatus(ProgramId, GL_LINK_STATUS);
	    
	    if (ErrorString.empty())
	    {
		glValidateProgram(ProgramId);
		ErrorString = CheckProgramStatus(ProgramId, GL_VALIDATE_STATUS);
		
		bIsValid = ErrorString.empty();
	    }
	}
	else
	{
	    ErrorString = string("glCreateProgram returned 0.");
	}
    }
    else
    {
	std::ostringstream ErrorStream;
	ErrorStream << "Shader program failed to compile:\n";
	if (!VertexShader->bIsValid)
	{
	    ErrorStream << "VERTEX SHADER ERROR:\n"
			<< VertexShader->ErrorString << "\n";
	}
	if (!FragmentShader->bIsValid)
	{
	    ErrorStream << "FRAGMENT SHADER ERROR:\n"
			<< FragmentShader->ErrorString << "\n";
	}
	ErrorString = ErrorStream.str();
    }

    std::cout << "Created shader program " << ProgramId << "\n";


    if (bIsValid)
    {
	GatherAttributes();
	GatherUniforms();
    }
}




void ShaderProgram::GatherAttributes()
{
    GLint AttributeCount;
    glGetProgramiv(ProgramId, GL_ACTIVE_ATTRIBUTES, &AttributeCount);
    if (AttributeCount)
    {
	GLint AttributeMaxLength;
	glGetProgramiv(ProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &AttributeMaxLength);

	for (int AttrIndex=0; AttrIndex<AttributeCount; AttrIndex+=1)
	{
	    Attribute Attr;
	    Attr.Slot = AttrIndex;
	    Attr.Name = string(AttributeMaxLength, 0);
	    glGetActiveAttrib(ProgramId, AttrIndex, AttributeMaxLength, NULL, &Attr.Size, &Attr.Type, (char*) Attr.Name.data());
	    Attr.Name.resize(Attr.Name.find_first_of('\0')); // trim extra null charcaters
	    Attributes.push_back(Attr);
	}
    }
}




void ShaderProgram::GatherUniforms()
{
    GLint UnifromCount;
    glGetProgramInterfaceiv(ProgramId, GL_UNIFORM, GL_ACTIVE_RESOURCES, &UnifromCount);
    if (UnifromCount)
    {
	const GLenum Query[4] = {GL_BLOCK_INDEX, GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};

	for (int u = 0; u < UnifromCount; u++)
	{
	    GLint UniformInfo[4];
	    glGetProgramResourceiv(ProgramId, GL_UNIFORM, u, 4, Query, 4, NULL, UniformInfo);
	    if (UniformInfo[0] == -1)
	    {
		// Uniform is in the default block.
		Uniform Entry = {
		    string(UniformInfo[1], 0),
		    (GLenum) UniformInfo[2],
		    UniformInfo[3]
		};

		glGetProgramResourceName(ProgramId, GL_UNIFORM, u, UniformInfo[1], NULL, (char*)Entry.Name.data());
		Entry.Name.resize(Entry.Name.find_first_of('\0')); // trim extra null charcaters

		Uniforms.push_back(Entry);
	    }
	}
    }
}




ShaderProgram::~ShaderProgram()
{
    VertexShader.reset();
    FragmentShader.reset();
    if (ProgramId)
    {
	glDeleteProgram(ProgramId);
    }
    std::cout << "Deleted shader program " << ProgramId << "\n";
}




//
// Shader program compilation interface.  Returns the program handle
// if successful, otherwise returns zero and raises an error.
//
shared_ptr<ShaderProgram> BuildShaderProgram(const string VertexSource, const string FragmentSource)
{
    auto Program = make_shared<ShaderProgram>(VertexSource, FragmentSource);
    if (Program->bIsValid)
    {
	ShaderPrograms[Program->ProgramId] = Program;
	std::cout << "Returning ProgramId: " << Program->ProgramId << "\n";
    }
    else
    {
	// raise an error
	RaiseError(Program->ErrorString.data());
    }
    return Program;
}




//
shared_ptr<ShaderProgram> GetShaderProgram(GLuint ProgramId)
{
    return ShaderPrograms[ProgramId];
}
