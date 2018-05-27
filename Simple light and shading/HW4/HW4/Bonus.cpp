
#include "glad.h"
#include <GL/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <ctime>
#include "Camera.h"
#include "shader_m.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
void processInput(GLFWwindow *window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//���ڴ�С
int window_width = 800;
int window_height = 800;
//֡��֡ʱ���
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//��ʼ���λ��
float lastx = window_width / 2;
float lasty = window_height / 2;
bool firstMouse = true;
//������࣬��ʼ���������
Camera camera;
//�������8������
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

//��Դλ��
glm::vec3 lightPos(0.7f, 1.0f, 0.3f);

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	//��ʼ��GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//����GLFW,�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//������Ⱦģʽ
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//��������
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "my_triangle", NULL, NULL);	//���ڿ��ߣ�����
	if (window == NULL) {
		cout << "Fail to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//���˴��ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//���ع��
	//glfwSetCursorPosCallback(window, mouse_callback);				//����ƶ����
	//GLAD����
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}
	//��Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//����shader_m�ഴ����ɫ��
	Shader lightShader("shader/light.vs", "shader/light.fs");
	Shader PhongShader("shader/Phong_cube.vs", "shader/Phong_cube.fs");
	Shader GouraudShader("shader/Gouraud_cube.vs", "shader/Gouraud_cube.fs");
	//����������VAO����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//����һ��VBO����
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//�������
	//��vertices��ֵ��������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//���
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//���ɶ����Դ����
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//�������
	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//���
	glBindVertexArray(0);
	//�����˵�
	bool show_menu = true;
	
	//ͶӰ��ϵ��
	float fov = 45.0, near_plat1 = 0.1, far_plat1 = 100.0;
	//������ɫ&��Դ��ɫ
	float objectColor[] = { 1.0f, 0.5f, 0.31f };
	float lightColor[] = { 1.0f, 1.0f, 1.0f };
	//������ǿ��
	float ambientStrength = 0.1;
	//�����
	int Shininess = 32;
	//���淴���ǿ��
	float specularStrength = 1.0;
	//������ǿ��
	float diffStrength = 1.0;
	//ѡ���ĸ�����ģ��
	static int choice = 0;
	//��Դ����or��ͣ��
	static int lightPosChange = 0;
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//������һ֡����һ֡��ʱ���
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//��������
		processInput(window, deltaTime);
		
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//�ı��Դλ��
		if (lightPosChange == 1) {
			lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
			lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
		}
		glm::mat4 model, view, projection;
		//��תһ���Ա�۲�
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		//��ʱ��������۲����,����Camera��
		view = camera.getViewMatrix();
		//͸��ͶӰ
		projection = glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, near_plat1, far_plat1);
		//�������
		Shader* temp = NULL;
		if (choice == 0) {		//Phong Shading
			temp = &PhongShader;
		}
		else if (choice == 1) {		//Gouraud Shading
			temp = &GouraudShader;
		}
		temp->use();
		temp->setFloat("ambientStrength", ambientStrength);
		temp->setFloat("specularStrength", specularStrength);
		temp->setFloat("diffStrength", diffStrength);
		temp->setInt("Shininess", Shininess);
		temp->setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
		temp->setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
		temp->setVec3("lightPos", lightPos);
		temp->setVec3("viewPos", camera.cameraPos);
		temp->setMat4("model", model);
		temp->setMat4("view", view);
		temp->setMat4("projection", projection);
		
		//����������
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//���Ƶ�Դ
		lightShader.use();
		//����Դλ��һ�²���С
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwPollEvents();
		//GUI����
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Menu", &show_menu);
		if (ImGui::CollapsingHeader("Choose the light position fixed or changing")) {
			ImGui::RadioButton("fixed", &lightPosChange, 0);
			ImGui::RadioButton("changing", &lightPosChange, 1);
		}
		if (ImGui::CollapsingHeader("Choose Shading Model")) {
			ImGui::RadioButton("Phong Shading", &choice, 0);
			ImGui::RadioButton("Gouraud Shading", &choice, 1);
			ImGui::ColorEdit3("object color", (float*)&objectColor);	//������ɫ
			ImGui::ColorEdit3("light color", (float*)&lightColor);		//��Դ��ɫ
			ImGui::SliderFloat("ambient strenght", &ambientStrength, 0.0, 1.0, "%.2f");		//������ǿ��
			ImGui::SliderFloat("diff strenght", &diffStrength, 0.0, 1.0, "%.2f");		//������ǿ��
			ImGui::SliderFloat("specular strength", &specularStrength, 0.0, 1.0, "%.2f");	//���淴��ǿ��
			ImGui::SliderInt("Shininess", &Shininess, 0, 500);			//�����
			
		}
		ImGui::End();
		//��Ⱦ
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		//�ӿ�λ��&��С���ӿ�<=����
		glViewport(0, 0, display_w, display_h);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//��ֹ
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}


//��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Error %d: %s\n", error, description);
}
//��������
void processInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {		//W��
		camera.moveForward(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {		//S��
		camera.moveBack(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {		//A��
		camera.moveLeft(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {		//D��
		camera.moveRight(deltaTime);
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//��һ������ƶ�
	if (firstMouse) {
		lastx = xpos;
		lasty = ypos;
		firstMouse = false;
	}
	//���λ��
	float xoffset = -(xpos - lastx);
	float yoffset = -(lasty - ypos);
	lastx = xpos;
	lasty = ypos;
	camera.rotate(xoffset, yoffset);
}
