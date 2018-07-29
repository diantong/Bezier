// Bezier.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//��ṹ��
struct point2 {
	double x;
	double y;
	point2() :x(0), y(0) {}
	point2(double _x, double _y) :x(_x), y(_y) {}
};

//���Ƶ����
const int ctrlSize = 3;
// Bezier���Ƶ�
float vertices[ctrlSize * 3] = {-1.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f,
                                 1.0f, 0.0f, 0.0f};

const char* vertexShaderSource = "#version 330 core\n"
                           "layout(location = 0) in vec3 bPos;\n"
                           "void main() {\n"
	                           "gl_Position = vec4(bPos.x, bPos.y, bPos.z, 1.0);\n"
                           "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main() {\n"
                                       "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "};\0";
const char* glsl_version = "#version 130";

//���������
double C(int n, int m) {
	//����Ϸ����ж�
	if (m > n)
		return 0;
	
	//C(n, m) = C(n, n-m)
	if (m < n / 2.0)
		m = n - m;

	double first = 0;
	for (int i = m + 1; i <= n; i++)
		first += log((double)i);
	double second = 0;
	int upper = n - m;
	for (int i = 2; i <= upper; i++)
		second += log((double)i);

	return exp(first - second);
}

//����t�����ֵ��
//��Ҫ��B�����Ż�����Ϊ����ÿ��t����һ���ֶ��ǹ����ģ����Խ���Ԥ����
point2 B(double t, int n, point2* common) {
	double x = 0, y = 0;
	for (int i = 0; i <= n; i++) {
		double c = pow(1 - t, n - i) * pow(t, i);
		x += c * common[i].x;
		y += c * common[i].y;
	}
	return point2(x, y);
}

int main()
{
	//��ʼ��GLFW
	glfwInit();
	//�����汾����ΰ汾������Ϊ3 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//����ģʽ(�ɱ�̹���)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ʹ��GLFW�򿪴��ڲ�����������
	GLFWwindow* window = glfwCreateWindow(800, 600, "Bezier", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// ��ʼ�� GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();


	//�м��ֵm����,����ͨ��GUI��mֵ����ָ��
	int m = 59;
	//ΪBezier������������
	float* Bezier = new float[m * 3];
	//���Ϊ1/m+1
	double space = 1.0 / (double)(m + 1);

	//��B�Ĺ��в��ֽ��м���
	point2* common = new point2[ctrlSize];
	for (int i = 0; i < ctrlSize; i++) {
		double c = C(ctrlSize-1, i);
		common[i].x = c * vertices[i * 3 + 0];
		common[i].y = c * vertices[i * 3 + 1];
	}

	//����t���μ����м��m����
	for (int i = 1; i <= m; i++) {
		double t = i*space;
		point2 point = B(t, ctrlSize-1, common);
		std::cout << point.x << " " << point.y << std::endl;
		Bezier[(i - 1) * 3 + 0] = point.x;
		Bezier[(i - 1) * 3 + 1] = point.y;
		Bezier[(i - 1) * 3 + 2] = 0;
	}

	// ������������
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// �������㻺�����
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//��������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// ������������
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// �������㻺�����
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//��������
	glBufferData(GL_ARRAY_BUFFER, m * 3 * sizeof(float), Bezier, GL_STATIC_DRAW);
	//��������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

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

	//������ɫ������
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//ɾ����ɫ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//�������״̬
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
			<< infoLog << std::endl;
	}

	bool show_demo_window = false;

	//��Ⱦ����
	do {
		glfwPollEvents();

		//��ʼ��һ֡����Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Add Primitives")) {
				if (ImGui::MenuItem("Rectangle")) {}
				if (ImGui::MenuItem("Triangle")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		ImGui::Render();

		//ʹ����Ӧ��VAO���ƿ��Ƶ�
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 3);

		glBindVertexArray(vao);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, m);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	// ���
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	//�ͷ�����
	delete[] Bezier;
	delete[] common;
    return 0;
}

