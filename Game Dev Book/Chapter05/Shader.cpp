#include "Shader.hpp"
#include <fstream>
#include <sstream>

Shader::Shader() {
	mShaderProgram = 0;
	mVertexShader = 0;
	mFragShader = 0;
}

Shader::~Shader() {

}

void Shader::Unload() {
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

// sets a shader program as the active one, which OpenGL will use when drawing triangles
void Shader::SetActive() {
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix) {
	// "name" is the uniform variable's name in the shader file
	// "matrix" is the custom matrix we assign to the uniform variable

	// find the uniform by this name
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// send the matrix data to the uniform
	glUniformMatrix4fv(
		loc,  // uniform ID
		1,  // number of matrices (only 1 in this case)
		GL_TRUE,  // set to TRUE if using row vectors
		matrix.GetAsFloatPtr()  // pointer to matrix data
	);
}

// validate whether a shader program has linked its vertex and fragment shaders successfully
bool Shader::IsValidProgram() {
	GLint status;

	// query the link status
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		// encountered error, retrieve a human-readable error message
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("Shader Program Link Failed:\n%s", buffer);
		return false;
	}
	return true;
}

// compile and link a vertex shader and fragment shader together
bool Shader::Load(const std::string& vertName, const std::string& fragName) {
	// compile vertex and fragment shaders
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
		!CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader)) {
		return false;
	}

	// now create a shader program that links together the vertex/frag shaders
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);  // add vertex shader to the shader program
	glAttachShader(mShaderProgram, mFragShader);  // likewise, add fragment shader
	glLinkProgram(mShaderProgram);  // link together all attached shaders

	// verify that the program linked successfully
	if (!IsValidProgram()) {
		return false;
	}

	return true;
}

// validate whether a shader object compiled successfully
bool Shader::IsCompiled(GLuint shader) {
	GLint status;
	
	// query the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		// encountered error, retrieve a human-readable error message
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSL Compile Failed:\n%s", buffer);
		return false;
	}
	return true;
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader) {
	// create an ifstream to load in the shader file
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open()) {
		// use a string stream to load contents of the file into a string
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();  // get the C-style string pointer

		// create an OpenGL shader object corresponding to the shader, of the specified type, and save this ID in outShader
		outShader = glCreateShader(shaderType);
		// set the source characters and try to compile
		glShaderSource(outShader, 1, &(contentsChar), nullptr);  // specifies the string containing the shader source code
		glCompileShader(outShader);

		// validate that the shader compiles
		if (!IsCompiled(outShader)) {
			SDL_Log("Failed to compile shader %s", fileName.c_str());
			return false;
		}
	}
	else {
		SDL_Log("Shader file not found: %s", fileName.c_str());
		return false;
	}

	return true;
}