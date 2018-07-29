// Bezier.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//点结构体
struct point2 {
	double x;
	double y;
	point2() :x(0), y(0) {}
	point2(double _x, double _y) :x(_x), y(_y) {}
};

//控制点个数
const int ctrlSize = 3;
// Bezier控制点
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

//组合数计算
double C(int n, int m) {
	//输入合法性判断
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

//根据t计算插值点
//需要对B进行优化，因为对于每个t，有一部分都是公共的，可以进行预计算
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
	//初始化GLFW
	glfwInit();
	//将主版本号与次版本号设置为3 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//核心模式(可编程管线)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 使用GLFW打开窗口并设置上下文
	GLFWwindow* window = glfwCreateWindow(800, 600, "Bezier", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 初始化 GLEW
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


	//中间插值m个点,后期通过GUI对m值进行指定
	int m = 59;
	//为Bezier曲线申请数组
	float* Bezier = new float[m * 3];
	//间隔为1/m+1
	double space = 1.0 / (double)(m + 1);

	//对B的固有部分进行计算
	point2* common = new point2[ctrlSize];
	for (int i = 0; i < ctrlSize; i++) {
		double c = C(ctrlSize-1, i);
		common[i].x = c * vertices[i * 3 + 0];
		common[i].y = c * vertices[i * 3 + 1];
	}

	//根据t依次计算中间的m个点
	for (int i = 1; i <= m; i++) {
		double t = i*space;
		point2 point = B(t, ctrlSize-1, common);
		std::cout << point.x << " " << point.y << std::endl;
		Bezier[(i - 1) * 3 + 0] = point.x;
		Bezier[(i - 1) * 3 + 1] = point.y;
		Bezier[(i - 1) * 3 + 2] = 0;
	}

	// 创建顶点数组
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 创建顶点缓冲对象
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//缓冲数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//解析数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// 创建顶点数组
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 创建顶点缓冲对象
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//缓冲数据
	glBufferData(GL_ARRAY_BUFFER, m * 3 * sizeof(float), Bezier, GL_STATIC_DRAW);
	//解析数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//创建顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//检查编译状态
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" 
			      << infoLog << std::endl;
	}

	//创建片元着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//检查编译状态
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
			<< infoLog << std::endl;
	}

	//创建着色器程序
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//检查连接状态
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
			<< infoLog << std::endl;
	}

	bool show_demo_window = false;

	//渲染周期
	do {
		glfwPollEvents();

		//开始新一帧的渲染
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

		//使用相应的VAO绘制控制点
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


	// 清除
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	//释放数组
	delete[] Bezier;
	delete[] common;
    return 0;
}

