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
int matchPoint(GLfloat & tempX, GLfloat & tempY);
void deletePoint(int index);
//���ڴ�С
int window_width = 800;
int window_height = 500;
//4����-> 3*4=12
GLfloat pointsArr[12] = { 0 };
//4������ƶ�
bool pointMove[4] = { false };
unsigned int indices[] = {
	0, 1, 2, 3
};

//�ѻ���ĸ���
int pointNum = 0;
//������ṹ
struct Point {
	GLfloat x, y;
};
//����λ��
Point mouseCoor;

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	//��ʼ��GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //����GLFW,�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //������Ⱦģʽ
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//��������
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "my_triangle", NULL, NULL);  //���ڿ��ߣ�����
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

	//��imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
	//�ص�����
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//���ع��
	glfwSetCursorPosCallback(window, mouseMove);				//����ƶ����
	glfwSetMouseButtonCallback(window, mouseClick);				//��������
	//��ɫ��
	Shader helpLineShader("shader/helpLine.vs", "shader/helpLine.fs");
	Shader BezierShader("shader/bezierCurve.vs", "shader/bezierCurve.fs");

	//����һ��VAO����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//����һ��VBO����
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //�������
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);   //���
	glBindVertexArray(0);
	bool show_menu = true;
	//������ɫ
	float curveColor[] = { 0.5f, 0.4f, 0.7f };
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//�������
		helpLineShader.use();
		glBufferData(GL_ARRAY_BUFFER, sizeof(pointsArr), pointsArr, GL_STATIC_DRAW);  //���û���������ݸ��Ƶ���ǰ�󶨻���
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		//���Ƹ�����
		glBindVertexArray(VAO);
		glPointSize(1.5);
		glLineWidth(1.5);
		glDrawElements(GL_LINE_STRIP, pointNum, GL_UNSIGNED_INT, 0);
		//����������
		if (pointNum == 4) {
			BezierShader.use();
			BezierShader.setVec3("curveColor", glm::vec3(curveColor[0], curveColor[1], curveColor[2]));
			Bezier_Curve(VAO);
		}
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Bezier Curve", &show_menu);
		//���
		if (ImGui::Button("Clear")) {
			pointNum = 0;
		}
		//�ı�������ɫ
		if (ImGui::CollapsingHeader("Change Curve color")) {
			ImGui::ColorEdit3("Curve color", (float*)&curveColor);
		}
		ImGui::End();
		//��Ⱦ
		glfwGetFramebufferSize(window, &window_width, &window_height);
		//�ӿ�λ��&��С���ӿ�<=����
		glViewport(0, 0, window_width, window_height);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//system("pause");
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

/*
* �����&�ƶ��¼�����
* ����������£�-> ���㣨���ֻ�ܻ�4���㣩
* �����&�ƶ�������+�ƶ�+�ɿ� -> �ƶ����λ�ã���4���㶼������������²����ƶ���
*/
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
	//�������
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (pointNum < 4) {	//����
			pointsArr[pointNum * 3] = mouseCoor.x / (window_width * 0.5) - 1.0;
			pointsArr[pointNum * 3 + 1] = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
			pointsArr[pointNum * 3 + 2] = 0.0;
			pointNum++;
		}
		else {	//�ƶ���
			GLfloat tempX = mouseCoor.x / (window_width * 0.5) - 1.0;
			GLfloat tempY = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
			//�жϰ��µ�λ���Ƿ��Ӧĳ����(�ݲ��ڰ뾶0.025��)
			int index = matchPoint(tempX, tempY);
			if (index >= 0) {	//��ƥ��ģ��ƶ������
				pointMove[index] = true;
			}
		}
	}
	//�ɿ�
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		for (int i = 0; i < 4; i++) {
			pointMove[i] = false;
		}
	}
	//�Ҽ�����ĳ����
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		GLfloat tempX = mouseCoor.x / (window_width * 0.5) - 1.0;
		GLfloat tempY = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
		//�жϰ��µ�λ���Ƿ��Ӧĳ����(�ݲ��ڰ뾶0.025��)
		int index = matchPoint(tempX, tempY);
		if (index >= 0) {	//��ƥ��ģ�ɾ�������
			deletePoint(index);
		}
	}
}
//�жϰ��µ�λ���Ƿ��Ӧĳ����(�ݲ��ڰ뾶0.025��)
int matchPoint(GLfloat & tempX, GLfloat & tempY) {
	int i;
	double dis_radiu;
	for (i = 0; i < 4; i++) {
		dis_radiu = sqrt(pow(pointsArr[i*3] - tempX, 2) + pow(pointsArr[i*3+1] - tempY, 2));
		if (dis_radiu <= 0.025) {
			return i;
		}
	}
	return -1;
}
//ɾ��һ����
void deletePoint(int index) {
	int i = 0;
	if (index < pointNum - 1) {	//�������һ����,����ĵ���ǰ��
		for (i = index; i < pointNum - 1; i++) {
			pointsArr[i * 3] = pointsArr[(i + 1) * 3];
			pointsArr[i * 3 + 1] = pointsArr[(i + 1) * 3 + 1];
			pointsArr[i * 3 + 2] = pointsArr[(i + 1) * 3 + 2];
		}
	}
	pointNum--;	//���һ����ֱ��pointNum--
}

//����ƶ����
void mouseMove(GLFWwindow* window, double xpos, double ypos) {
	mouseCoor.x = xpos;
	mouseCoor.y = ypos;
	int i;
	//�ж��Ƿ���ĳ�����ǿ��ƶ���(true)
	for (i = 0; i < 4; i++) {
		if (pointMove[i] == true) {
			break;
		}
	}
	if (i < 4) {	//�е����ƶ���,ʵʱ����
		pointsArr[i * 3] = mouseCoor.x / (window_width * 0.5) - 1.0;
		pointsArr[i * 3 + 1] = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
		pointsArr[i * 3 + 2] = 0.0;
	}
}

/*
* ������4������������ױ���������
* �����ڵ��˳�����£�
* [0]����ʼ�㣻[1]��[2]�ǿ��Ƶ㣻[3]���յ�
*/

void Bezier_Curve(unsigned int & pointVAO) {
	GLfloat t = 0.0;
	GLfloat bezier[3003] = { 0 };
	GLfloat tempX = 0.0, tempY = 0.0;
	int k = 0;
	for (t = 0.0; t <= 1.0; t += 0.001) {
		tempX = pointsArr[0] * pow(1.0 - t, 3) + 3 * pointsArr[3] * t * pow(1.0 - t, 2) + 3 * pointsArr[6] * pow(t, 2) * (1.0 - t) + pointsArr[9] * pow(t, 3);
		tempY = pointsArr[1] * pow(1.0 - t, 3) + 3 * pointsArr[4] * t * pow(1.0 - t, 2) + 3 * pointsArr[7] * pow(t, 2) * (1.0 - t) + pointsArr[10] * pow(t, 3);
		bezier[k] = tempX;
		bezier[k + 1] = tempY;
		bezier[k + 2] = 0.0;
		k += 3;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezier), bezier, GL_STATIC_DRAW);
	glBindVertexArray(pointVAO);
	glDrawArrays(GL_POINTS, 0, 1001);
}


