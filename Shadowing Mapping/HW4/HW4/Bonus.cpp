//#define GLEW_STATIC
//#include <GL/glew.h>
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
#include "stb_image.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
void processInput(GLFWwindow *window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void RenderScene(Shader &shader, unsigned int & planeVAO, unsigned int & cubeVAO);
GLuint create_texture(GLchar* path);

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
Camera camera(glm::vec3(0.0f, 2.5f, 4.0f));
//�������8������
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
};
//�ذ�ƽ��
float planeVertices[]{
	 25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f,  0.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,  0.0f, 25.0f,
	-25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,

	 25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f,  0.0f,
	 25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,  0.0f, 25.0f
};

//��Դλ��
glm::vec3 lightPos(-2.0f, 3.0f, -1.0f);

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
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Shadowing Mapping", NULL, NULL);	//���ڿ��ߣ�����
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
	//��ʼ��GLEW
	//glewExperimental = GL_TRUE;
	//glewInit();
	glViewport(0, 0, window_width, window_height);
	//��Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//����shader_m�ഴ����ɫ��
	Shader lightShader("shader/light.vs", "shader/light.fs");
	Shader cubeShader("shader/cube_shader.vs", "shader/cube_shader.fs");
	Shader depthShader("shader/depth_shader.vs", "shader/depth_shader.fs");
	cubeShader.use();
	cubeShader.setInt("diffuseTexture", 0);
	cubeShader.setInt("shadowMap", 1);

	//���ɵذ�ƽ��VAO��VBO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);	//�������
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//����λ��
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	//������
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	//����
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//����������VAO,VBO����
	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//�������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//��vertices��ֵ��������
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//����λ��
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	//������
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	//����
	glEnableVertexAttribArray(2);

	//���ɶ����ԴVAO,VBO����
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//����λ��
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//����������
	GLuint cubeTexture = create_texture("./img/wall.jpg");

	//�����ͼ��֡����
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	//����һ��2D����
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//���Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);	//��������ɫ���ݵ���Ⱦ,����ֻ��Ҫ�����Ϣ
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	bool show_menu = true;	//�����˵�
	float fov = 45.0, near_plat1 = 0.1, far_plat1 = 100.0;	//ͶӰ��ϵ��
	float ambientStrength = 0.15;	//������ǿ��
	int Shininess = 64;				//�����
	float specularStrength = 1.0;	//���淴���ǿ��
	float diffStrength = 1.0;		//������ǿ��
	static int choice = 0;			//ѡ���Դ��ͶӰģʽ
	static int lightPosChange = 0;	//��Դ����or��ͣ��
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

		//ѡ���Ƿ�ı��Դλ��
		if (lightPosChange == 1) {
			lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
			lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f + 1.2f;
			lightPos.z = sin(glfwGetTime()) * 2.0f - 1.0f;
		}

		//1. �Թ�Դ�ӽ���Ⱦ�������õ��������ͼ������ͶӰor͸��ͶӰ��
		glm::mat4 lightProjection;
		GLfloat near_plane = 1.0f, far_plane = 25.0f;
		if (choice == 0) {	//����ͶӰ
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		}
		else {				//͸��ͶӰ
			lightProjection = glm::perspective(glm::radians(45.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		}
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));	//�ӹ�Դ���򳡾�����
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;	//�任����T
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);	//��Ⱦ�����ͼ
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(depthShader, planeVAO, cubeVAO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//2. ������Ⱦ��͸��ͶӰ��
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);	//�����ӽ�
		glm::mat4 model, view, projection;
		//��תһ���Ա�۲�
		//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		view = camera.getViewMatrix();	//��ʱ��������۲����,����Camera��
		projection = glm::perspective(glm::radians(fov), (float)display_w / (float)display_h, near_plat1, far_plat1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubeShader.use();
		cubeShader.setFloat("ambientStrength", ambientStrength);
		cubeShader.setFloat("specularStrength", specularStrength);
		cubeShader.setFloat("diffStrength", diffStrength);
		cubeShader.setInt("Shininess", Shininess);
		cubeShader.setVec3("lightPos", lightPos);
		cubeShader.setVec3("viewPos", camera.cameraPos);
		//cubeShader.setMat4("model", model);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		//ortho or perspective
		cubeShader.setInt("ortho_or_pespective", choice);
		cubeShader.setFloat("near_plane", near_plane);
		cubeShader.setFloat("far_plane", far_plane);
		//������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderScene(cubeShader, planeVAO, cubeVAO);

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
		if (ImGui::CollapsingHeader("Choose projection of the light")) {
			ImGui::RadioButton("ortho", &choice, 0);
			ImGui::RadioButton("perspective", &choice, 1);
		}
		if (ImGui::CollapsingHeader("Phong Shading")) {
			ImGui::SliderFloat("ambient strenght", &ambientStrength, 0.0, 1.0, "%.2f");		//������ǿ��
			ImGui::SliderFloat("diff strenght", &diffStrength, 0.0, 1.0, "%.2f");		//������ǿ��
			ImGui::SliderFloat("specular strength", &specularStrength, 0.0, 1.0, "%.2f");	//���淴��ǿ��
			ImGui::SliderInt("Shininess", &Shininess, 0, 500);			//�����
		}
		ImGui::End();
		//��Ⱦ
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//��ֹ
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &planeVBO);
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
//��Ⱦ(�ذ�+������)
void RenderScene(Shader &shader, unsigned int & planeVAO, unsigned int & cubeVAO) {
	//�ذ�ƽ�棨������ʾ��Ӱ��
	glm::mat4 model;
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//�����壨2����
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0));
	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 0.5f, -0.5));
	model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.5));
	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// ����&��������
GLuint create_texture(GLchar* path) {
	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//���Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	//���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);	//���
	stbi_image_free(data);
	return Texture;
}
