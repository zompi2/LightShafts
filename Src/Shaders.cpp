/**
* Shaders helper library.
*
* Simple static library for easy loading and compiling shaders.
*
* (c) 2014 Damian Nowakowski
*/


#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "shaders.h"

// Use this macro when something go bad. The program will pause
// so the user can read the shader compilation error and then exit.
#define FAIL_GRACEFULLY		system("pause"); \
							exit(EXIT_FAILURE);


/**
* Attach the shader file to the program
* @param program	- Handler of the program
*					 (if not initialized this function will create program under this handler)
* @param typ		- Type of shader, can be: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or GL_GEOMETRY_SHADER
* @param path		- Path to the shader file
*/
void Shaders::AttachShader(GLuint &program, GLenum type, const char *path)
{
	GLuint shader = LoadShader(type, path);

	// If the program doesn't exist create a new one
	if (glIsProgram(program) == false)
	{
		program = glCreateProgram();
	}

	glAttachShader(program, shader);
}

/**
* Link the attached shaders into the program
* @param program	- Handler to the existing program
*/
void Shaders::LinkProgram(GLuint program)
{
	// Link shaders into program
	glLinkProgram(program);

	// Check if linking went good and display an error log when not
	ValidateProgram(program);

	// walidacja shader�w
	glValidateProgram(program);

	ValidateProgram(program);
}

/**
* Delete shaders from program
* @param program	- Handler to the existing program
*/
void Shaders::DeleteShaders(GLuint program)
{
	// Check how many shaders are in this program
	GLint maxCount = 0;
	glGetProgramiv(program, GL_ATTACHED_SHADERS, &maxCount);
	
	/// When there are any shaders in this program delete them
	if (maxCount != 0)
	{
		GLuint *shaders = new GLuint[maxCount];
		glGetAttachedShaders(program, maxCount, NULL, shaders);
		for (int i = 0; i < maxCount; i++)
		{
			glDeleteShader(shaders[i]);
		}
		delete[] shaders;
	}

	// Clear the opengl error buffer
	glGetError();
}

/**
* Load shader from the file
* @param type - Type of shader, can be: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or GL_GEOMETRY_SHADER
* @param path - Path to the shader file
* @returns the id of the created shader
*/
GLuint Shaders::LoadShader(GLenum type, const char *path)
{
	/// Read the shader from file to buffer
	std::ifstream file;
	file.open(path, std::ios::binary);
	if (file.is_open() == false || file.bad() == true)
	{
		printf("Can't open the shader file: %s\n", path);
		FAIL_GRACEFULLY
	}
	file.seekg(0, std::ios::end);
	int fileLength = (int)(file.tellg());
	file.seekg(0, std::ios::beg);
	GLchar* sourceBuffer = new GLchar[(fileLength + 1) * sizeof(GLchar)];
	file.read(sourceBuffer, fileLength);
	sourceBuffer[fileLength] = '\0';
	file.close();

	// Create shader of asked type.
	GLuint shader = glCreateShader(type);

	// Load the source to the shader
	glShaderSource(shader, 1, const_cast<const GLchar**>(&sourceBuffer), NULL);

	// Clean up the buffer
	delete[] sourceBuffer;

	// Now compile the shader
	glCompileShader(shader);

	// Check if shader compilation went good and display an error log when not
	ValidateShader(shader, path);

	// If everything went good return the complete shader
	return shader;
}

/**
* Validate the shader. Use right after the glCompileShader.
* Function reads the last shader compilation error. When there was an error
* it prints it and gracefully exit the application.
* @param shader - id of the shader to validate
* @param path	- optional path for printig which file is not valid
*/
void Shaders::ValidateShader(GLuint shader, const char* path)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		GLint errorLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLogLength);
		char *log = new char[errorLogLength];
		glGetShaderInfoLog(shader, errorLogLength, NULL, log);

		printf("Error when compiling the shader: %s\n", path);
		printf("%s\n", log);

		delete[] log;

		FAIL_GRACEFULLY
	}
}

/**
* Validate the program. Use right after the glLinkProgram and glValidateProgram.
* Function reads the last shader linking error. When there was an error
* it prints it and gracefully exit the application.
* @param program - handler of the program to validate
*/
void Shaders::ValidateProgram(GLuint program)
{
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint errorLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLogLength);
		char *log = new char[errorLogLength];
		glGetProgramInfoLog(program, errorLogLength, NULL, log);

		printf("Error when linking the shader\n");
		printf("%s\n", log);

		delete[] log;

		FAIL_GRACEFULLY
	}
}
