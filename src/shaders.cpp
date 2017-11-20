
#include <functional>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "util.h"
#include "shaders.h"

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::vector;


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




vector<GLint> ShaderProgram::ResourceQuery(GLenum Interface, GLuint ResourceIndex, GLenum Query, GLsizei ItemCount)
{
    vector<GLint> Result(ItemCount);
    glGetProgramResourceiv(ProgramId, Interface, ResourceIndex, 1, &Query, ItemCount, NULL, Result.data());
    return Result;
}




vector<GLint> ShaderProgram::ResourceQuery(GLenum Interface, GLuint ResourceIndex, vector<GLenum> Query)
{
    vector<GLint> Result(Query.size());
    glGetProgramResourceiv(ProgramId, Interface, ResourceIndex, Query.size(), (const GLenum*) Query.data(), Query.size(), NULL, Result.data());
    return Result;
}




GLint ShaderProgram::ResourceQuery(GLenum Interface, GLuint ResourceIndex, const GLenum Query)
{
    GLint Value;
    glGetProgramResourceiv(ProgramId, Interface, ResourceIndex, 1, &Query, 1, NULL, &Value);
    return Value;
}




GLint ShaderProgram::InterfaceProperty(GLenum Interface, GLenum Property)
{
    GLint Value;
    glGetProgramInterfaceiv(ProgramId, Interface, Property, &Value);
    return Value;
}




string ShaderProgram::ResourceName(GLenum Interface, GLuint ResourceIndex)
{
    GLint NameLength = ResourceQuery(Interface, ResourceIndex, GL_NAME_LENGTH);
    string Name = string(NameLength, 0);
    glGetProgramResourceName(ProgramId, Interface, ResourceIndex, NameLength, NULL, (char*) Name.data());
    Name.resize(Name.find_first_of('\0'));
    return Name;
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
    GLint BlockCount = InterfaceProperty(GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES);
    if (BlockCount)
    {
    	for (int BlockIndex = 0; BlockIndex < BlockCount; BlockIndex++)
    	{
	    GLint ActiveUniforms = ResourceQuery(GL_UNIFORM_BLOCK, BlockIndex, GL_NUM_ACTIVE_VARIABLES);
	    
    	    if (ActiveUniforms)
    	    {
		UniformBlock NewBlock;
		NewBlock.Name = ResourceName(GL_UNIFORM_BLOCK, BlockIndex);
		NewBlock.ProgramId = ProgramId;
		NewBlock.BlockIndex = BlockIndex;
		NewBlock.BufferSize = ResourceQuery(GL_UNIFORM_BLOCK, BlockIndex, GL_BUFFER_DATA_SIZE);
		
    		vector<GLint> UniformIndices = ResourceQuery(GL_UNIFORM_BLOCK, BlockIndex, GL_ACTIVE_VARIABLES, ActiveUniforms);
    		for (int IndexIndex = 0; IndexIndex < ActiveUniforms; IndexIndex++)
    		{
    		    GLint UniformIndex = UniformIndices[IndexIndex];
    		    vector<GLint> UniformInfo = ResourceQuery(
			GL_UNIFORM, UniformIndex,
			{GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE});

		    UniformEntry NewEntry;
		    NewEntry.Name = ResourceName(GL_UNIFORM, UniformIndex);
		    NewEntry.Type = (GLenum) UniformInfo[0];
		    NewEntry.ArraySize = UniformInfo[1];
		    NewEntry.BlockOffset = UniformInfo[2];
		    NewEntry.BlockArrayStride = UniformInfo[3];
		    NewEntry.BlockMatrixStride = UniformInfo[3];
		    NewBlock.Uniforms.push_back(NewEntry);
    		}

		UniformBlocks.push_back(NewBlock);
    	    }
    	}
    }
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




PYTHON_API(BuildShader)
{
    if (nargs == 2)
    {
	const string VertexSource = string((const char*)PyUnicode_DATA(args[0]));
	const string FragmentSource = string((const char*)PyUnicode_DATA(args[1]));
	return PyLong_FromLong(BuildShaderProgram(VertexSource, FragmentSource)->ProgramId);
    }
    RaiseError("Invalid number of arguments.");
    Py_RETURN_NONE;
}




PYTHON_API(ActivateShader)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    glUseProgram(ShaderProgramId);
    Py_RETURN_NONE;
}




PYTHON_API(GetShaderAttrs)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    shared_ptr<ShaderProgram> Shader = GetShaderProgram(ShaderProgramId);
    
    if (Shader)
    {
	int AttrCount = Shader->Attributes.size();
	PyObject* Dict = PyDict_New();
	for (int a=0; a<AttrCount; a++)
	{
	    string Name = Shader->Attributes[a].Name;
	    PyDict_SetItemString(Dict, Name.data(), PyLong_FromLong(a));
	}
	return Dict;
    }

    Py_RETURN_NONE;
}




PYTHON_API(GetShaderUniformBlocks)
{
    GLuint ShaderProgramId = PyLong_AsLong(args[0]);
    shared_ptr<ShaderProgram> Shader = GetShaderProgram(ShaderProgramId);
    
    if (Shader)
    {
	int BlockCount = Shader->UniformBlocks.size();
	PyObject* Dict = PyDict_New();
	for (int b=0; b<BlockCount; b++)
	{
	    string BlockName = Shader->UniformBlocks[b].Name;
	    long BlockAddress = (long)&(Shader->UniformBlocks[b]);
	    PyDict_SetItemString(Dict, BlockName.data(), PyLong_FromLong(BlockAddress));
	}
	return Dict;
    }

    Py_RETURN_NONE;
}
