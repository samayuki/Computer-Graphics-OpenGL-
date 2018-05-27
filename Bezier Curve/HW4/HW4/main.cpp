/*
#include "glad.h"
#include <GL/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <ctime>
#include "shader_m.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
void mouseClick(GLFWwindow* window, int button, int action, int mods);
void mouseMove(GLFWwindow* window, double xpos, double ypos);
void Bezier_Curve(unsigned int & pointVAO);

//������ṹ
struct Point{
	GLfloat x, y;
};
//�洢�����������
Point points[4];
//4����-> 3*4=12
GLfloat pointsArr[12] = {0};
unsigned int indices[] = {
	0, 1, 2, 3
};
float vertices[] = {
-0.5, -0.5, 0.0,
0.5, -0.5, 0.0,
0.5,  0.5, 0.0,
-0.5, 0.5, 0.0
};

//
//�ѻ���ĸ���
int pointNum = 0;
//����λ��
Point mouseCoor;
//���ڴ�С
int window_width = 800;
int window_height = 800;


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
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Bezier Curve", NULL, NULL);	//���ڿ��ߣ�����
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
	glViewport(0, 0, window_width, window_height);
	//��Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
	//�ص�����
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//���ع��
	//glfwSetCursorPosCallback(window, mouseMove);				//����ƶ����
	//glfwSetMouseButtonCallback(window, mouseClick);				//��������
	//��ɫ��
	Shader helpLineShader("shader/helpLine.vs","shader/helpLine.fs");
	Shader BezierShader("shader/bezierCurve.vs", "shader/bezierCurve.fs");
	//VAO, VBO, EBO
	unsigned int pointVAO, pointVBO, pointEBO;
	glGenVertexArrays(1, &pointVAO);
	glGenBuffers(1, &pointVBO);
	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);	//�������
	glGenBuffers(1, &pointEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointEBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	//����λ��
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	bool show_menu = true;
	int a = 1;
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		BezierShader.use();
		glm::vec3 curveColor(0.5, 0.4, 0.7);
		BezierShader.setVec3("curveColor", curveColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindVertexArray(pointVAO);
		glPointSize(5);
		glLineWidth(5);
		//������
		glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, 0);
		//��������������
		//glm::vec3 curveColor(0.5, 0.4, 0.7);
		if (pointNum == 4) {
			BezierShader.use();
			BezierShader.setVec3("curveColor", curveColor);
			Bezier_Curve(pointVAO);
		}
		
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);	//�����ӽ�
		//GUI����
		ImGui_ImplGlfwGL3_NewFrame();
		//ImGui::Begin("Menu", &show_menu);
		//ImGui::End();

		//��Ⱦ
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &pointVAO);
	glDeleteVertexArrays(1, &pointVBO);
	glDeleteVertexArrays(1, &pointEBO);
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

/*
 * �����&�ƶ��¼�����
 * �����������+�ɿ���-> ���㣨���ֻ�ܻ�4���㣩
 * �����&�ƶ�������+�ƶ�+�ɿ� -> �ƶ����λ�ã�
 */
/*
 //������»���
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (pointNum < 4) {
			points[pointNum].x = mouseCoor.x / (window_width * 0.5) - 1.0;
			points[pointNum].y = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
			cout << points[pointNum].x << "  " << points[pointNum].y << "\n";
			pointsArr[pointNum * 3] = points[pointNum].x;
			pointsArr[pointNum * 3 + 1] = points[pointNum].y;
			pointsArr[pointNum * 3 + 2] = 0.0;
			pointNum++;
		}
	}
}
//����ƶ����
void mouseMove(GLFWwindow* window, double xpos, double ypos) {
	mouseCoor.x = xpos;
	mouseCoor.y = ypos;
}

/*
* ������4������������ױ���������
* ����points����4��Point�ṹ��
* �����ڵ��˳�����£�
* [0]����ʼ�㣻[1]��[2]�ǿ��Ƶ㣻[3]���յ�
*/
/*
void Bezier_Curve(unsigned int & pointVAO) {
	GLfloat t = 0.0;
	GLfloat bezier[3003] = { 0 };
	GLfloat tempX = 0.0, tempY = 0.0;
	int k = 0;
	for (t = 0.0; t <= 1.0; t += 0.001) {
		tempX = points[0].x * pow(1.0 - t, 3) + 3 * points[1].x * t * pow(1.0 - t, 2) + 3 * points[2].x * pow(t, 2) * (1.0 - t) + points[3].x * pow(t, 3);
		tempY = points[0].y * pow(1.0 - t, 3) + 3 * points[1].y * t * pow(1.0 - t, 2) + 3 * points[2].y * pow(t, 2) * (1.0 - t) + points[3].y * pow(t, 3);
		bezier[k] = tempX;
		bezier[k + 1] = tempY;
		bezier[k + 2] = 0.0;
		k += 3;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezier), bezier, GL_STATIC_DRAW);
	glBindVertexArray(pointVAO);
	glDrawArrays(GL_POINTS, 0, 1001);
}
*/

