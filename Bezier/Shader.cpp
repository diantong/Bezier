#include "stdafx.h"
#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessCtrlPath, const char* tessEvalPath) {
	// 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::string tessCtrlCode;
	std::string tessEvalCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream tcShaderFile;
	std::ifstream teShaderFile;
	// ��֤ifstream��������׳��쳣��
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		std::stringstream vShaderStream, fShaderStream, tcShaderStream, teShaderStream;
		// ���ļ�
		vShaderFile.open(vertexPath);
		vShaderStream << vShaderFile.rdbuf();
		vShaderFile.close();
		vertexCode = vShaderStream.str();

		fShaderFile.open(fragmentPath);
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();

		if (tessCtrlPath != NULL) {
			tcShaderFile.open(tessCtrlPath);
			tcShaderStream << tcShaderFile.rdbuf();
			tcShaderFile.close();
			tessCtrlCode = tcShaderStream.str();
		}

		if (tessEvalPath != NULL) {
			teShaderFile.open(tessEvalPath);
			teShaderStream << teShaderFile.rdbuf();
			teShaderFile.close();
			tessEvalCode = teShaderStream.str();
		}
		
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();


	//����������ɫ��
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//������״̬
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
			<< infoLog << std::endl;
	}

	//����ƬԪ��ɫ��
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//������״̬
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
			<< infoLog << std::endl;
	}

	//����ϸ�ֿ�����ɫ��
	unsigned int tessCtrlShader;
	if (tessCtrlPath != NULL) {
		const char* tessCtrlShaderSource = tessCtrlCode.c_str();
		tessCtrlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tessCtrlShader, 1, &tessCtrlShaderSource, NULL);
		glCompileShader(tessCtrlShader);
		//������״̬
		glGetShaderiv(tessCtrlShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tessCtrlShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TESSCTRL::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}
	}

	//����ϸ�ּ�����ɫ��
	unsigned int tessEvalShader;
	if (tessEvalPath != NULL) {
		const char* tessEvalShaderSource = tessEvalCode.c_str();
		tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tessEvalShader, 1, &tessEvalShaderSource, NULL);
		glCompileShader(tessEvalShader);
		//������״̬
		glGetShaderiv(tessEvalShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tessEvalShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TESSEval::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}
	}

	//������ɫ������
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if(tessCtrlPath != NULL)
		glAttachShader(ID, tessCtrlShader);
	if (tessEvalPath != NULL)
		glAttachShader(ID, tessEvalShader);
	glLinkProgram(ID);
	//�������״̬
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
			<< infoLog << std::endl;
	}

	//ɾ����ɫ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (tessCtrlPath != NULL)
		glDeleteShader(tessCtrlShader);
	if (tessEvalPath != NULL)
		glDeleteShader(tessEvalShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}