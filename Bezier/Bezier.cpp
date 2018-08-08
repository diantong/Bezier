// Bezier.cpp : �������̨Ӧ�ó������ڵ㡣

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	
	//ʹ��imgui�����ͼ��������¼����м����󣬽�Curveת��Ϊ�ֲ�����������Ҫȫ��
	//VAO���󴴽�ǰ��Ҫ�ȴ�GLEW��ʼ����ϣ�֮ǰcurve��ȫ�ֱ���������ڹ��캯���ж�VAO��VBO���г�ʼ������ɷ��ʳ�ͻ��
	//����curve�Ǿֲ�������������GLEW��ʼ����Ϻ������������ԭGLinit����ֱ���ڹ��캯���н���
	Curve curve(100, 99);

	//����
	Surface surface(4, 4, 20, 20);
	//��������м���
	surface.ComputeBezier();
	//������������м���
	surface.ctrlMesh();
	//������������м���
	surface.BezierMesh();

	//��ɫ������
	Shader curve_shader("Curve.vs", "Curve.fs");
	Shader surface_shader("Surface.vs", "Surface.fs");

	//�任����
	glm::mat4 model(1.0f);
	glm::vec3 cameraPos = glm::vec3(0.5f, 0.5f, 2.5f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	surface_shader.use();
	surface_shader.setMat4("mvp", projection * view * model);
	curve_shader.use();
	curve_shader.setMat4("mvp", projection * view * model);

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

	//չʾ����
	Window show = none;

	//��Ⱦ����
	do {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//��ʼ��һ֡����Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//UI
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Bezier")) {
				if (ImGui::MenuItem("Curve")) { 
					show = bezier_curve; 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				if (ImGui::MenuItem("Surface")) { 
					show = bezier_surface; 
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
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
				curve_shader.use();
				//����Bezier����
				curve.DrawBezier();
				//���ƿ��Ƶ�
				curve.DrawCtrlPoints();
				//�������ɶ���
				curve.DrawDynamic();
				break;
			}
			//Bezier����
			case bezier_surface: {
				curve_shader.use();
				surface.DrawCtrlPoints();
				surface_shader.use();
				surface.DrawCtrlMesh();
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