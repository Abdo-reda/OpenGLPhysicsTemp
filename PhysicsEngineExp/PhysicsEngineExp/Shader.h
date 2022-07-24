#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class Shader {


public:
	unsigned int ID; //Shader program ID

	//Constructor
	Shader(const char* vertexPath, const char* fragmentPath);

	//use/activate shader program
	void use();

	void remove();

	//utility functions for uniforms
	void setBool(const string& name, bool value) const;
	void setInt(const string& name, int value) const;
	void setFloat(const string& name, float value) const;
	void setVec2(const string& name, const vec2& value) const;
	void setVec2(const string& name, float x, float y) const;
	void setVec3(const string& name, const vec3& value) const;
	void setVec3(const string& name, float x, float y, float z) const;
	void setVec4(const string& name, const vec4& value) const;
	void setVec4(const string& name, float x, float y, float z, float w) const;
	void setMat2(const string& name, const mat2& mat) const;
	void setMat3(const string& name, const mat3& mat) const;
	void setMat4(const string& name, const mat4& mat) const;

};



Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	//read from a file.
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	//?! ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		//read file buffer into string streams
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		//close files
		vShaderFile.close();
		fShaderFile.close();

		//convert streams to strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	} catch (ifstream::failure e) {
		cout << "ERROR! CANT READ SHADER FILES!" << endl;
	}

	//need to convert string objects to char *
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//debuing
	int success;
	char infoLog[512];



	//create and compile vertex and fragment shaders
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //get compile status
	if (!success) { //check status and print info log if error
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR! CANT COMPILE VERTEX SHADER\n" << infoLog << endl;
	};


	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); //get compile status
	if (!success) { //check status and print info log if error
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR! CANT COMPILE FRAGMENT SHADER\n" << infoLog << endl;
	};

	//create the shader program
	ID = glCreateProgram();
	//attach and link them to the program.
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success); //get status
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR! CANT LINK SHADER PROGRAM\n" << infoLog << endl;
	}

	//linked so no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const string& name, bool value) const {
	//looks for uniform with name and set its value
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);

	/* which is basically .....
	const char* temp = name.c_str();
	int tempLocation = glGetUniformLocation(ID, temp);
	glUniform1i(tempLocation, value);
	*/
}

void Shader::setInt(const string& name, int value) const {
	//looks for uniform with name and set its value
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const string& name, float value) const {
	//looks for uniform with name and set its value
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const string& name, const vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const string& name, const vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const string& name, const vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const string& name, const mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const string& name, const mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const string& name, const mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::remove() {
	glDeleteProgram(ID);
}




#endif
