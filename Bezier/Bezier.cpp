// Bezier.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h"
#include "Shader.h"
#include "Curve.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum Window {
	bezier_curve, bezier_surface, none
};

//�ӿڴ�С
int WIDTH = 800;
int HEIGHT = 600;

const char* glsl_version = "#version 130";

//�ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()  {

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
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//ע��ص�����
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
	
	//ʹ��imgui�����ͼ��������¼����м����󣬽�Curveת��Ϊ�ֲ�����������Ҫȫ��
	//VAO���󴴽�ǰ��Ҫ�ȴ�GLEW��ʼ����ϣ�֮ǰcurve��ȫ�ֱ���������ڹ��캯���ж�VAO��VBO���г�ʼ������ɷ��ʳ�ͻ��
	//����curve�Ǿֲ�������������GLEW��ʼ����Ϻ������������ԭGLinit����ֱ���ڹ��캯���н���
	Curve curve(100, 99);

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

	//��ɫ������
	Shader shader("Bezier.vs", "Bezier.fs");

	//չʾ����
	Window show = none;

	//��Ⱦ����
	do {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//��ʼ��һ֡����Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Bezier")) {
				if (ImGui::MenuItem("Curve")) { show = bezier_curve; }
				if (ImGui::MenuItem("Surface")) { show = bezier_surface; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		//��Ⱦ����
		switch (show) {
			//Bezier����
			case bezier_curve: {
				//����������¼�,��Ҫ�����Hovering��click�¼��������ͻ
				if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringAnyWindow() == false) {
					double x, y;
					x = ImGui::GetMousePos().x;
					y = ImGui::GetMousePos().y;
					//��x/yת����NDC
					x = -1 + 2 * (x / WIDTH);
					y = 1 - 2 * (y / HEIGHT);
					//��ӿ��Ƶ�
					bool success = curve.push(x, y);
				}
				//����Ҽ�����¼�
				if (ImGui::IsMouseClicked(1)) {
					//ɾ�����Ƶ�
					bool success = curve.pop();
				}
				//���O������¼�
				if (ImGui::IsKeyPressed(GLFW_KEY_O))
					curve.AniSwitch();

				//������߸���
				curve.Update();
				//ʹ����Ӧ��VAO���л���
				shader.use();
				//����Bezier����
				curve.DrawBezier();
				//���ƿ��Ƶ�
				curve.DrawCtrlPoints();
				//�������ɶ���
				curve.DrawDynamic();
				break;
			}
			//Bezier����
			case bezier_surface:
				break;
			case none:
				break;
			default:
				break;
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//���Ƹ������
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	
	// ���
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
    return 0;
}

//���ڴ�С�ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
