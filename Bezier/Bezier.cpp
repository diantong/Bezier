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

//动态输入控制点
float* ctrlPoint = NULL;
//最多输入控制点
const int maxSize = 1000;
//当前输入控制点个数
int currentSize = 0;
//共有部分
point2* common = NULL;
//插值点数
int m = 99;
//为Bezier曲线申请数组
float* Bezier = NULL;
//间隔为1/m+1
double space = 1.0 / (double)(m + 1);

const int WIDTH = 800;
const int HEIGHT = 600;

//着色器
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
double C(int n, int m);

//根据t计算插值点
point2 B(double t, int n, point2* common);

//计算曲线
void ComputeBezier();

bool update = false;
//鼠标点击回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Set required callback function
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//GLFW Options
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//初始化 GLEW
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

	//利用数组来管理
	ctrlPoint = new float[maxSize * 3];
	//为Bezier曲线申请数组
	Bezier = new float[m * 3];
	//对B的固有部分进行计算
	common = new point2[maxSize];

	// 创建顶点数组
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// 创建顶点缓冲对象
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//缓冲数据
	glBufferData(GL_ARRAY_BUFFER, m * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	//解析数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// 创建顶点数组
	unsigned int cvao;
	glGenVertexArrays(1, &cvao);
	glBindVertexArray(cvao);
	// 创建顶点缓冲对象
	unsigned int cvbo;
	glGenBuffers(1, &cvbo);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	//缓冲数据,数据会频繁的改变
	glBufferData(GL_ARRAY_BUFFER, maxSize * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
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

	//渲染周期
	do {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		/*
		//开始新一帧的渲染
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
		
		//使用相应的VAO绘制控制点
		glUseProgram(shaderProgram);
		
		if (currentSize > 1) {
			// 控制点大于2对插值点进行绘制
			//绘制插值点
			glBindVertexArray(vao);
			if (update == true) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m * 3 * sizeof(float), Bezier);
			}
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, m);
			glBindVertexArray(0);
		}
		
		//绘制控制点
		glBindVertexArray(cvao);
		if (update == true) {
			glBindBuffer(GL_ARRAY_BUFFER, cvbo);
			//对空间进行预分配，后不断更新填充
			glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize * 3 * sizeof(float), ctrlPoint);
		}
		glPointSize(15.0f);
		glDrawArrays(GL_POINTS, 0, currentSize);
		glBindVertexArray(0);

		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//绘制更新完毕
		update = false;
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	/*
	// 清除
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	*/

	glfwTerminate();

	//释放
	delete[] Bezier;
	delete[] common;
	delete[] ctrlPoint;
	//删除
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &cvao);
	glDeleteBuffers(1, &cvbo);
    return 0;
}

//鼠标点击回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//发生鼠标点击事件
	if (action == GLFW_PRESS)
		switch (button) {
			//鼠标左键
			case GLFW_MOUSE_BUTTON_LEFT: {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				//将x/y转换到NDC
				x = -1 + 2 * (x / WIDTH);
				y = 1 - 2 * (y / HEIGHT);
				//对鼠标输入的控制点进行存储
				if (currentSize < maxSize) {
					ctrlPoint[currentSize * 3 + 0] = x;
					ctrlPoint[currentSize * 3 + 1] = y;
					ctrlPoint[currentSize * 3 + 2] = 0;
					currentSize++;
					update = true;
				}
				break;
			}
			//鼠标右键
			case GLFW_MOUSE_BUTTON_RIGHT: {
				//数组管理的好处即是只需要对个数进行操作
				if (currentSize > 0) {
					currentSize--;
					update = true;
				}
				break;
			}
		}

	//控制点发生变化，重新计算插值点
	if (update == true && currentSize > 1)
		ComputeBezier();

	return;
}

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
	//对公共部分进行计算
	for (int i = 0; i < currentSize; i++) {
		double c = C(currentSize - 1, i);
		common[i].x = c * ctrlPoint[i * 3 + 0];
		common[i].y = c * ctrlPoint[i * 3 + 1];
	}

	//根据t依次计算中间的m个点
	for (int i = 1; i <= m; i++) {
		double t = i*space;
		point2 point = B(t, currentSize - 1, common);
		Bezier[(i - 1) * 3 + 0] = point.x;
		Bezier[(i - 1) * 3 + 1] = point.y;
		Bezier[(i - 1) * 3 + 2] = 0;
	}
}