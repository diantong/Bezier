#ifndef SHADER_H
#define SHADER_H

#include<GLEW\glew.h>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

class Shader {
public:
	unsigned int ID;

	//构造函数
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessCtrlPath = NULL, const char* tessEvalPath = NULL);

	//激活着色器程序
	void use();
	//uniform
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void set1DFloat(const std::string &name, int size, float* value) const;
	void setVec3(const std::string &name, glm::vec3 value) const;
	void setMat4(const std::string &name, glm::mat4 value) const;
};


#endif // ! SHADER_H

