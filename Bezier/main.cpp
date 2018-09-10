// Bezier.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "Shader.h"
#include "Curve.h"
#include "Surface.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum Window {
	bezier_curve, bezier_surface, none
};

//视口大小
int WIDTH = 800;
int HEIGHT = 600;

const char* glsl_version = "#version 130";

//回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()  {

	//初始化GLFW
	glfwInit();
	//将主版本号与次版本号设置为3 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	
	Curve curve(100, 99);

	//曲面
	Surface surface(4, 4, 4, 4);
	//对曲面进行计算
	surface.ComputeBezier();
	surface.ctrlMesh();
	surface.BezierMesh();

	//变换矩阵
	glm::mat4 model(1.0f);

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.5f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//摄像机坐标轴
	glm::vec3 Front = glm::normalize(cameraTarget - cameraPos);
	glm::vec3 Right = glm::normalize(glm::cross(Front, worldUp));
	glm::vec3 Up = glm::normalize(glm::cross(Right, Front));

	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	
	//着色器程序
	Shader curve_shader("Curve.vs", "Curve.fs");
	curve_shader.use();
	curve_shader.setVec3("color", glm::vec3(0.6f, 0.6f, 0.6f));

	//加入着色控制器
	Shader surface_shader("Surface.vs", "Surface.fs", "Surface.tc", "Surface.te");
	surface_shader.use();
	surface_shader.setMat4("mvp", projection * view * model);

	Shader ctrl_shader("Surface_ctrl.vs", "Curve.fs");
	ctrl_shader.use();
	ctrl_shader.setMat4("mvp", projection * view * model);
	ctrl_shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup style
	ImGui::StyleColorsDark();

	//展示窗口
	Window show = none;
	
	//鼠标点击
	bool firstMouseDown = true;
	double downX;
	double downY;
	double offsetX = 0;
	double offsetY = 0;
	const float SENSITIVITY = 0.01f;

	//渲染周期
	do {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//开始新一帧的渲染
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Bezier")) {
				if (ImGui::MenuItem("Curve")) { 
					show = bezier_curve; 
				}
				if (ImGui::MenuItem("Surface")) { 
					show = bezier_surface; 
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		//渲染窗口
		switch (show) {
			//Bezier曲线
			case bezier_curve: {
				//检测左键点击事件,需要处理好Hovering与click事件，避免冲突
				if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringAnyWindow() == false) {
					double x, y;
					x = ImGui::GetMousePos().x;
					y = ImGui::GetMousePos().y;
					//将x/y转换到NDC
					x = -1 + 2 * (x / WIDTH);
					y = 1 - 2 * (y / HEIGHT);
					bool success = curve.push(x, y);
				}
				//检测右键点击事件
				if (ImGui::IsMouseClicked(1)) {
					bool success = curve.pop();
				}
				//检测O键点击事件
				if (ImGui::IsKeyPressed(GLFW_KEY_O))
					curve.AniSwitch();

				//曲线、控制点、动画
				curve.Update();
				curve_shader.use();
				curve.DrawBezier();
				curve.DrawCtrlPoints();
				curve.DrawDynamic();
				break;
			}
			//Bezier曲面
			case bezier_surface: {
				//对鼠标左键按下事件进行监听
				if (ImGui::IsMouseDown(0)) {
					//刚按下时记录当前光标的位置
					if (firstMouseDown) {
						downX = ImGui::GetMousePos().x;
						cameraPos = glm::rotate(glm::mat4(1.0f), (float)offsetX, cameraTarget + Up) * 
							        glm::vec4(cameraPos, 1.0f);
						firstMouseDown = false;
					} else{
						//计算偏移量
						offsetX = downX - ImGui::GetMousePos().x;
						offsetX *= SENSITIVITY;
						
						//旋转摄像机
						glm::vec3 newPos = glm::rotate(glm::mat4(1.0f), (float)offsetX, cameraTarget + Up) *
							               glm::vec4(cameraPos, 1.0f);
						view = glm::lookAt(newPos, cameraTarget, worldUp);
				
						//更改着色器
						surface_shader.use();
						surface_shader.setMat4("mvp", projection * view * model);
						ctrl_shader.use();
						ctrl_shader.setMat4("mvp", projection * view * model);
					}
				}
				else
					firstMouseDown = true;
	
				//控制点
				ctrl_shader.use();
				surface.DrawCtrlPoints();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//surface.DrawCtrlMesh();

				surface_shader.use();
				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				surface.DrawBezierMesh();
				break;
			}
			case none:
				break;
			default:
				break;
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//绘制更新完毕
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	
	// 清除
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
    return 0;
}

//窗口大小回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}