// Bezier.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//�ӿڴ�С
int WIDTH = 800;
int HEIGHT = 600;

//��ṹ��
struct point2 {
	double x;
	double y;
	point2() :x(0), y(0) {}
	point2(double _x, double _y) :x(_x), y(_y) {}
};

//���������Ƶ�
const int maxSize = 100;
//��ǰ������Ƶ����
int currentSize = 0;
//��ֵ����
int m = 99;
//���Ϊ1/m+1
double space = 1.0 / (double)(m + 1);

//��̬������Ƶ�
float* ctrlPoint = NULL;
//ΪBezier������������
float* Bezier = NULL;
//���в���
point2* common = NULL;
//ΪBezierd�Ķ�̬���ɵ���������
float* Anibezier = NULL;

//��ɫ��
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
double C(int n, int m);
//����t�����ֵ��
point2 B(double t, int n, point2* common);
//��������
void ComputeBezier();

//�ص�����
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//���Ƶ��Ƿ񱻸���
bool update = false;

//�����Ƿ���
bool isAnimation = false;
//����ʱ��
float aniTime = 0;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//ע��ص�����
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//GLFW Options
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//��ʼ�� GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	
	/*
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
	*/

	//��������������
	ctrlPoint = new float[maxSize * 3];
	//ΪBezier������������
	Bezier = new float[m * 3];
	//Ϊ��̬���ɵ���������
	Anibezier = new float[maxSize * 3];
	//��B�Ĺ��в��ֽ��м���
	common = new point2[maxSize];

	//������������
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//�������㻺�����
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//��������
	glBufferData(GL_ARRAY_BUFFER, m * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	//��������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//������������
	unsigned int cvao;
	glGenVertexArrays(1, &cvao);
	glBindVertexArray(cvao);
	//�������㻺�����
	unsigned int cvbo;
	glGenBuffers(1, &cvbo);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	//��������
	glBufferData(GL_ARRAY_BUFFER, maxSize * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	//��������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//������������
	unsigned int avao;
	glGenVertexArrays(1, &avao);
	glBindVertexArray(avao);
	unsigned int avbo;
	glGenBuffers(1, &avbo);
	glBindBuffer(GL_ARRAY_BUFFER, avbo);
	//��������
	glBufferData(GL_ARRAY_BUFFER, maxSize * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
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

	//��Ⱦ����
	do {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		/*
		//��ʼ��һ֡����Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Bezier")) {
				if (ImGui::MenuItem("cure")) {}
				if (ImGui::MenuItem("plane")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		ImGui::Render();
		*/
		
		//ʹ����Ӧ��VAO���л���
		glUseProgram(shaderProgram);
		
		//����Bezier����
		if (currentSize > 1) {
			// ���Ƶ����2�Բ�ֵ����л���
			//���Ʋ�ֵ��
			//VAO���Դ洢��������Щ���ԣ���ν�����Щ�����Լ����ĸ�VBO��ȡ��Щ����
			//�������Ҫ��VBO�����ݽ����޸ģ�������Ҫ�Ȱ���Ӧ�Ļ����ٽ�����Ӧ�ĸ��²���
			glBindVertexArray(vao);
			if (update == true) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m * 3 * sizeof(float), Bezier);
			}
			//glPointSize(5.0f);
			//glDrawArrays(GL_POINTS, 0, m);
			glDrawArrays(GL_LINE_STRIP, 0, m);
			glBindVertexArray(0);
		}
		
		//���ƿ��Ƶ�
		glBindVertexArray(cvao);
		if (update == true) {
			glBindBuffer(GL_ARRAY_BUFFER, cvbo);
			//�Կռ����Ԥ���䣬�󲻶ϸ������
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize * 3 * sizeof(float), ctrlPoint);
		}
		glPointSize(15.0f);
		glDrawArrays(GL_POINTS, 0, currentSize);
		glDrawArrays(GL_LINE_STRIP, 0, currentSize);
		glBindVertexArray(0);
	
		//Bezier�������ɶ���
		if (currentSize > 1 && isAnimation) {
			glBindVertexArray(avao);
			glBindBuffer(GL_ARRAY_BUFFER, avbo);
			int round = currentSize - 1;
			//��ʼ��
			//�ɶ�̬�������ڵ���������ԣ����Է����ڸ�����Ļ�����������һ�ֶ�̬��
			for (int i = 0; i < currentSize * 3; i++)
				Anibezier[i] = ctrlPoint[i];
			while (round > 0) {
				for(int i = 0; i < round; i++) {
					Anibezier[i * 3 + 0] = Anibezier[i * 3 + 0] + (Anibezier[(i + 1) * 3 + 0] - Anibezier[i * 3 + 0])*aniTime;
					Anibezier[i * 3 + 1] = Anibezier[i * 3 + 1] + (Anibezier[(i + 1) * 3 + 1] - Anibezier[i * 3 + 1])*aniTime;
				}
				//���������뻺��
				glBufferSubData(GL_ARRAY_BUFFER, 0, round * 3 * sizeof(float), Anibezier);
				//���Ƶ�
				glPointSize(5.0f);
				glDrawArrays(GL_POINTS, 0, round);
				//��������
				glDrawArrays(GL_LINE_STRIP, 0, round);
				//��һ�ֶ�̬���ɵ�Ļ���
				round--;
			}
			//�������
			glBindVertexArray(0);
			aniTime += 0.0001;
			if (aniTime > 1)
				aniTime = 0;
		}

		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//���Ƹ������
		update = false;
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	/*
	// ���
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	*/

	glfwTerminate();

	//�ͷ�
	delete[] Bezier;
	delete[] common;
	delete[] ctrlPoint;
	delete[] Anibezier;
	//ɾ��
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &cvao);
	glDeleteBuffers(1, &cvbo);
	glDeleteVertexArrays(1, &avao);
	glDeleteBuffers(1, &avbo);
    return 0;
}

//������ص�����
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//����������¼�
	if (action == GLFW_PRESS)
		switch (button) {
			//������
			case GLFW_MOUSE_BUTTON_LEFT: {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				//��x/yת����NDC
				x = -1 + 2 * (x / WIDTH);
				y = 1 - 2 * (y / HEIGHT);
				//���������Ŀ��Ƶ���д洢
				if (currentSize < maxSize) {
					ctrlPoint[currentSize * 3 + 0] = x;
					ctrlPoint[currentSize * 3 + 1] = y;
					ctrlPoint[currentSize * 3 + 2] = 0;
					currentSize++;
					update = true;
					aniTime = 0;
				}
				break;
			}
			//����Ҽ�
			case GLFW_MOUSE_BUTTON_RIGHT: {
				//�������ĺô�����ֻ��Ҫ�Ը������в���
				if (currentSize > 0) {
					currentSize--;
					update = true;
					aniTime = 0;
				}
				break;
			}
		}

	//���Ƶ㷢���仯�����¼����ֵ��
	if (update == true && currentSize > 1)
		ComputeBezier();

	return;
}

//��������ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;
	switch (key) {
		case GLFW_KEY_O: {
			isAnimation = !isAnimation;
			if (isAnimation)
				std::cout << "Bezier animation open!" << std::endl;
			else {
				std::cout << "Bezier animation close!" << std::endl;
				aniTime = 0;
			}
			break;
		}
		default:
			break;
	}
}

//���ڴ�С�ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}

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

point2 B(double t, int n, point2* common) {
	double x = 0, y = 0;
	for (int i = 0; i <= n; i++) {
		double c = pow(1 - t, n - i) * pow(t, i);
		x += c * common[i].x;
		y += c * common[i].y;
	}
	return point2(x, y);
}

void ComputeBezier() {
	//�Թ������ֽ��м���
	for (int i = 0; i < currentSize; i++) {
		double c = C(currentSize - 1, i);
		common[i].x = c * ctrlPoint[i * 3 + 0];
		common[i].y = c * ctrlPoint[i * 3 + 1];
	}

	//����t���μ����м��m����
	for (int i = 1; i <= m; i++) {
		double t = i*space;
		point2 point = B(t, currentSize - 1, common);
		Bezier[(i - 1) * 3 + 0] = point.x;
		Bezier[(i - 1) * 3 + 1] = point.y;
		Bezier[(i - 1) * 3 + 2] = 0;
	}
}