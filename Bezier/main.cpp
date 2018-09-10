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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
	
	Curve curve(100, 99);

	//����
	Surface surface(4, 4, 4, 4);
	//��������м���
	surface.ComputeBezier();
	surface.ctrlMesh();
	surface.BezierMesh();

	//�任����
	glm::mat4 model(1.0f);

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.5f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//�����������
	glm::vec3 Front = glm::normalize(cameraTarget - cameraPos);
	glm::vec3 Right = glm::normalize(glm::cross(Front, worldUp));
	glm::vec3 Up = glm::normalize(glm::cross(Right, Front));

	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	
	//��ɫ������
	Shader curve_shader("Curve.vs", "Curve.fs");
	curve_shader.use();
	curve_shader.setVec3("color", glm::vec3(0.6f, 0.6f, 0.6f));

	//������ɫ������
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

	//չʾ����
	Window show = none;
	
	//�����
	bool firstMouseDown = true;
	double downX;
	double downY;
	double offsetX = 0;
	double offsetY = 0;
	const float SENSITIVITY = 0.01f;

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
				}
				if (ImGui::MenuItem("Surface")) { 
					show = bezier_surface; 
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
					bool success = curve.push(x, y);
				}
				//����Ҽ�����¼�
				if (ImGui::IsMouseClicked(1)) {
					bool success = curve.pop();
				}
				//���O������¼�
				if (ImGui::IsKeyPressed(GLFW_KEY_O))
					curve.AniSwitch();

				//���ߡ����Ƶ㡢����
				curve.Update();
				curve_shader.use();
				curve.DrawBezier();
				curve.DrawCtrlPoints();
				curve.DrawDynamic();
				break;
			}
			//Bezier����
			case bezier_surface: {
				//�������������¼����м���
				if (ImGui::IsMouseDown(0)) {
					//�հ���ʱ��¼��ǰ����λ��
					if (firstMouseDown) {
						downX = ImGui::GetMousePos().x;
						cameraPos = glm::rotate(glm::mat4(1.0f), (float)offsetX, cameraTarget + Up) * 
							        glm::vec4(cameraPos, 1.0f);
						firstMouseDown = false;
					} else{
						//����ƫ����
						offsetX = downX - ImGui::GetMousePos().x;
						offsetX *= SENSITIVITY;
						
						//��ת�����
						glm::vec3 newPos = glm::rotate(glm::mat4(1.0f), (float)offsetX, cameraTarget + Up) *
							               glm::vec4(cameraPos, 1.0f);
						view = glm::lookAt(newPos, cameraTarget, worldUp);
				
						//������ɫ��
						surface_shader.use();
						surface_shader.setMat4("mvp", projection * view * model);
						ctrl_shader.use();
						ctrl_shader.setMat4("mvp", projection * view * model);
					}
				}
				else
					firstMouseDown = true;
	
				//���Ƶ�
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