#pragma once

#include <string>
#include "GL/glew.h"
#include "SDL/SDL.h"
#include "Math.hpp"

// this class loads in shaders and lets OpenGL know about them
// step 1: load and compile the vertex shader
// step 2: load and compile the fragment shader
// step 3: link the two shaders together into a "shader program"
class Shader {
public:
	Shader();
	~Shader();

	// load the vertex/fragment shaders with the given names
	bool Load(const std::string& vertName, const std::string& fragName);
	// delete the shader program, vertex, and fragment shaders
	void Unload();
	// set this as the active shader program
	void SetActive();
	// set a matrix uniform
	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	// set a vector3 uniform
	void SetVectorUniform(const char* name, const Vector3& vector);
	// set a float uniform
	void SetFloatUniform(const char* name, float value);

private:
	// tries to compile the specified shader
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);
	// tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// tests whether vertex/fragment programs link
	bool IsValidProgram();

private:
	// store the shader object IDs
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};