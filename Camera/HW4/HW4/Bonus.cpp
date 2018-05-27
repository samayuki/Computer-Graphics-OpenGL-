
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
//��ɫ��
const char *shader_src = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 point_color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"point_color = aColor;\n"
"}";
const char *frag_src = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 point_color;\n"
"void main() {\n"
"FragColor = vec4(point_color, 1.0);}";

//�������8������
float vertices[] = {
	-0.2, -0.2,  0.2, 1.0, 0.0, 0.0,
	0.2, -0.2,  0.2, 0.0, 1.0, 0.0,
	-0.2,  0.2,  0.2, 0.4, 0.2, 0.2,
	0.2,  0.2,  0.2, 0.0, 0.3, 0.6,
	-0.2, -0.2, -0.2, 0.0, 0.0, 1.0,
	0.2, -0.2, -0.2, 0.2, 0.5, 1.0,
	-0.2,  0.2, -0.2, 0.4, 0.3, 0.3,
	0.2,  0.2, -0.2, 0.5, 0.7, 0.6
};

//����2��������->2 * 6 = 12
unsigned int indices[] = {
	0, 1, 2,
	1, 2, 3,
	4, 5, 6,
	5, 6, 7,
	0, 1, 4,
	1, 4, 5,
	1, 3, 5,
	3, 5, 7,
	2, 3, 6,
	3, 6, 7,
	0, 2, 4,
	2, 4, 6
};
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
	//GLAD����
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}
	//��Ȳ���
	glEnable(GL_DEPTH_TEST);
	//����Ȳ���
	//glDisable(GL_DEPTH_TEST);

	//��imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//����������ɫ��
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &shader_src, NULL);
	glCompileShader(vertex_shader);
	//����Ƭ����ɫ��
	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_src, NULL);
	glCompileShader(fragment_shader);
	//������ɫ������
	unsigned int shader_program;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	//ɾ����ɫ������
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//����һ��VAO����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//����һ��VBO����
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//�������
										//����һ��EBO����
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//���
									//������ɫ
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	//���
	glBindVertexArray(0);
	//�����˵�
	bool show_menu = true;
	//���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//����ƶ����
	glfwSetCursorPosCallback(window, mouse_callback);
	//ͶӰ��ϵ��
	float fov = 45.0, near_plat1 = 0.1, far_plat1 = 100.0;
	
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//������һ֡����һ֡��ʱ���
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//��������
		processInput(window, deltaTime);
		
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//�������
		glUseProgram(shader_program);
		glm::mat4 model, view, projection;
		//��תһ���Ա�۲�
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//��ʱ��������۲����,����Camera��
		view = camera.getViewMatrix();
		//͸��ͶӰ
		projection = glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, near_plat1, far_plat1);
		//
		unsigned int modelLoc = glGetUniformLocation(shader_program, "model");
		unsigned int viewLoc = glGetUniformLocation(shader_program, "view");
		unsigned int projLoc = glGetUniformLocation(shader_program, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

		//���ı��vertices���¸�ֵ��������
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//���û���������ݸ��Ƶ���ǰ�󶨻���
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		//����������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glfwPollEvents();
		//GUI����
		ImGui_ImplGlfwGL3_NewFrame();
		

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
