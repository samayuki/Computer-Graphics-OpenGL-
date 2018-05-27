
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
//窗口大小
int window_width = 800;
int window_height = 800;
//帧与帧时间差
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//初始鼠标位置
float lastx = window_width / 2;
float lasty = window_height / 2;
bool firstMouse = true;
//摄像机类，初始摄像机设置
Camera camera;
//立方体的8个顶点
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

//灯源位置
glm::vec3 lightPos(0.7f, 1.0f, 0.3f);

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	//初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//配置GLFW,版本号3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//核心渲染模式
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//创建窗口
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "my_triangle", NULL, NULL);	//窗口宽、高，命名
	if (window == NULL) {
		cout << "Fail to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//将此窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//隐藏光标
	//glfwSetCursorPosCallback(window, mouse_callback);				//鼠标移动监控
	//GLAD加载
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}
	//深度测试
	glEnable(GL_DEPTH_TEST);

	//绑定imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//利用shader_m类创建着色器
	Shader lightShader("shader/light.vs", "shader/light.fs");
	Shader PhongShader("shader/Phong_cube.vs", "shader/Phong_cube.fs");
	Shader GouraudShader("shader/Gouraud_cube.vs", "shader/Gouraud_cube.fs");
	//生成立方体VAO对象
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//生成一个VBO对象
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//将缓冲绑定
	//将vertices赋值到缓冲中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//解绑
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//生成顶点光源对象
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//将缓冲绑定
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//解绑
	glBindVertexArray(0);
	//创建菜单
	bool show_menu = true;
	
	//投影的系数
	float fov = 45.0, near_plat1 = 0.1, far_plat1 = 100.0;
	//物体颜色&光源颜色
	float objectColor[] = { 1.0f, 0.5f, 0.31f };
	float lightColor[] = { 1.0f, 1.0f, 1.0f };
	//环境光强度
	float ambientStrength = 0.1;
	//反光度
	int Shininess = 32;
	//镜面反射的强度
	float specularStrength = 1.0;
	//漫反射强度
	float diffStrength = 1.0;
	//选择哪个光照模型
	static int choice = 0;
	//光源不动or不停动
	static int lightPosChange = 0;
	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//计算上一帧与这一帧的时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//键盘输入
		processInput(window, deltaTime);
		
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//改变光源位置
		if (lightPosChange == 1) {
			lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
			lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
		}
		glm::mat4 model, view, projection;
		//旋转一下以便观察
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		//此时的摄像机观察矩阵,运用Camera类
		view = camera.getViewMatrix();
		//透视投影
		projection = glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, near_plat1, far_plat1);
		//激活程序
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
		
		//绘制三角形
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//绘制灯源
		lightShader.use();
		//将灯源位移一下并缩小
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwPollEvents();
		//GUI界面
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Menu", &show_menu);
		if (ImGui::CollapsingHeader("Choose the light position fixed or changing")) {
			ImGui::RadioButton("fixed", &lightPosChange, 0);
			ImGui::RadioButton("changing", &lightPosChange, 1);
		}
		if (ImGui::CollapsingHeader("Choose Shading Model")) {
			ImGui::RadioButton("Phong Shading", &choice, 0);
			ImGui::RadioButton("Gouraud Shading", &choice, 1);
			ImGui::ColorEdit3("object color", (float*)&objectColor);	//物体颜色
			ImGui::ColorEdit3("light color", (float*)&lightColor);		//光源颜色
			ImGui::SliderFloat("ambient strenght", &ambientStrength, 0.0, 1.0, "%.2f");		//环境光强度
			ImGui::SliderFloat("diff strenght", &diffStrength, 0.0, 1.0, "%.2f");		//漫反射强度
			ImGui::SliderFloat("specular strength", &specularStrength, 0.0, 1.0, "%.2f");	//镜面反射强度
			ImGui::SliderInt("Shininess", &Shininess, 0, 500);			//反光度
			
		}
		ImGui::End();
		//渲染
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		//视口位置&大小，视口<=窗口
		glViewport(0, 0, display_w, display_h);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//终止
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}


//辅助函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Error %d: %s\n", error, description);
}
//键盘输入
void processInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {		//W键
		camera.moveForward(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {		//S键
		camera.moveBack(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {		//A键
		camera.moveLeft(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {		//D键
		camera.moveRight(deltaTime);
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//第一次鼠标移动
	if (firstMouse) {
		lastx = xpos;
		lasty = ypos;
		firstMouse = false;
	}
	//鼠标位移
	float xoffset = -(xpos - lastx);
	float yoffset = -(lasty - ypos);
	lastx = xpos;
	lasty = ypos;
	camera.rotate(xoffset, yoffset);
}
