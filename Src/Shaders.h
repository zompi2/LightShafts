/**
* Shaders helper library.
*
* Simple static library for easy loading and compiling shaders.
*
* (c) 2014 Damian Nowakowski
*/


#pragma once

#include <GL/glew.h>

class Shaders
{
public:
	/**
	* Attach the shader file to the program
	* @param program	- Handler of the program 
	*					 (if not initialized this function will create program under this handler)
	* @param typ		- Type of shader, can be: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or GL_GEOMETRY_SHADER
	* @param path		- Path to the shader file
	*/											  
	static void AttachShader(GLuint &program, GLenum type, const char *path);

	/**
	* Link the attached shaders into the program
	* @param program	- Handler to the existing program
	*/
	static void LinkProgram(GLuint program);

	/**
	* Delete shaders from program
	* @param program	- Handler to the existing program
	*/
	static void DeleteShaders(GLuint program);

private:
	/**
	* Load shader from the file
	* @param type - Type of shader, can be: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or GL_GEOMETRY_SHADER
	* @param path - Path to the shader file
	* @returns the id of the created shader
	*/
	static GLuint LoadShader(GLenum type, const char *path);

	/**
	* Validate the shader. Use right after the glCompileShader. 
	* Function reads the last shader compilation error. When there was an error
	* it prints it and gracefully exit the application.
	* @param shader - id of the shader to validate
	* @param path	- optional path for printig which file is not valid
	*/
	static void ValidateShader(GLuint shader, const char* path = "");
	
	/**
	* Validate the program. Use right after the glLinkProgram and glValidateProgram.
	* Function reads the last shader linking error. When there was an error
	* it prints it and gracefully exit the application.
	* @param program - handler of the program to validate
	*/
	static void ValidateProgram(GLuint program);
};
