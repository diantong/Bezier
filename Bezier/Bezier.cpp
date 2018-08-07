// Bezier.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "Shader.h"
#include "Curve.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//视口大小
int WIDTH = 800;
int HEIGHT = 600;

const char* glsl_version = "#version 130";

//回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Curve curve(100, 99);

int main()  {

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
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//注册回调函数
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
	
	//VAO对象创建前需要等待GLEW初始化完毕，curve是全局变量，如果在构造函数中对VAO、VBO进行初始化将造成访问冲突。
	curve.GLInit();

	/*
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
	*/

	Shader shader("Bezier.vs", "Bezier.fs");

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
		*/

		//检查曲线更新
		curve.Update();
		//使用相应的VAO进行绘制
		shader.use();
		//绘制Bezier曲线
		curve.DrawBezier();	
		//绘制控制点
		curve.DrawCtrlPoints();
		//绘制生成动画
		curve.DrawDynamic();
	
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//绘制更新完毕
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	
	// 清除
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();

	glfwTerminate();
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
				//添加控制点
				bool success = curve.push(x, y);
				if (success)
					std::cout << "success" << std::endl;
				else
					std::cout << "fail" << std::endl;
				break;
			}
			//鼠标右键
			case GLFW_MOUSE_BUTTON_RIGHT: {
				//删除控制点
				bool success = curve.pop();
				if (success)
					std::cout << "success" << std::endl;
				else
					std::cout << "fail" << std::endl;
				break;
			}
		}
}

//键盘输入回调函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (key) {
			case GLFW_KEY_O: {
				curve.AniSwitch();
				break;
			}
		}
}

//窗口大小回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
