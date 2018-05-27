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
//窗口大小
int window_width = 800;
int window_height = 500;
//4个点-> 3*4=12
GLfloat pointsArr[12] = { 0 };
//4个点的移动
bool pointMove[4] = { false };
unsigned int indices[] = {
	0, 1, 2, 3
};

//已画点的个数
int pointNum = 0;
//点坐标结构
struct Point {
	GLfloat x, y;
};
//鼠标的位置
Point mouseCoor;

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	//初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //配置GLFW,版本号3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //核心渲染模式
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//创建窗口
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "my_triangle", NULL, NULL);  //窗口宽、高，命名
	if (window == NULL) {
		cout << "Fail to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//将此窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//GLAD加载
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}

	//绑定imGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
	//回调函数
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//隐藏光标
	glfwSetCursorPosCallback(window, mouseMove);				//鼠标移动监控
	glfwSetMouseButtonCallback(window, mouseClick);				//鼠标点击监控
	//着色器
	Shader helpLineShader("shader/helpLine.vs", "shader/helpLine.fs");
	Shader BezierShader("shader/bezierCurve.vs", "shader/bezierCurve.fs");

	//生成一个VAO对象
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//生成一个VBO对象
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //将缓冲绑定
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);   //解绑
	glBindVertexArray(0);
	bool show_menu = true;
	//曲线颜色
	float curveColor[] = { 0.5f, 0.4f, 0.7f };
	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//激活程序
		helpLineShader.use();
		glBufferData(GL_ARRAY_BUFFER, sizeof(pointsArr), pointsArr, GL_STATIC_DRAW);  //将用户定义的数据复制到当前绑定缓冲
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		//绘制辅助线
		glBindVertexArray(VAO);
		glPointSize(1.5);
		glLineWidth(1.5);
		glDrawElements(GL_LINE_STRIP, pointNum, GL_UNSIGNED_INT, 0);
		//贝塞尔曲线
		if (pointNum == 4) {
			BezierShader.use();
			BezierShader.setVec3("curveColor", glm::vec3(curveColor[0], curveColor[1], curveColor[2]));
			Bezier_Curve(VAO);
		}
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Bezier Curve", &show_menu);
		//清空
		if (ImGui::Button("Clear")) {
			pointNum = 0;
		}
		//改变曲线颜色
		if (ImGui::CollapsingHeader("Change Curve color")) {
			ImGui::ColorEdit3("Curve color", (float*)&curveColor);
		}
		ImGui::End();
		//渲染
		glfwGetFramebufferSize(window, &window_width, &window_height);
		//视口位置&大小，视口<=窗口
		glViewport(0, 0, window_width, window_height);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//system("pause");
	//终止
	glDeleteVertexArrays(1, &VAO);
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

/*
* 鼠标点击&移动事件规则
* 鼠标点击（按下）-> 画点（最多只能画4个点）
* 鼠标点击&移动（按下+移动+松开 -> 移动点的位置）在4个点都画出来的情况下才能移动点
*/
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
	//左键按下
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (pointNum < 4) {	//画点
			pointsArr[pointNum * 3] = mouseCoor.x / (window_width * 0.5) - 1.0;
			pointsArr[pointNum * 3 + 1] = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
			pointsArr[pointNum * 3 + 2] = 0.0;
			pointNum++;
		}
		else {	//移动点
			GLfloat tempX = mouseCoor.x / (window_width * 0.5) - 1.0;
			GLfloat tempY = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
			//判断按下的位置是否对应某个点(容差在半径0.025内)
			int index = matchPoint(tempX, tempY);
			if (index >= 0) {	//有匹配的，移动这个点
				pointMove[index] = true;
			}
		}
	}
	//松开
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		for (int i = 0; i < 4; i++) {
			pointMove[i] = false;
		}
	}
	//右键消除某个点
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		GLfloat tempX = mouseCoor.x / (window_width * 0.5) - 1.0;
		GLfloat tempY = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
		//判断按下的位置是否对应某个点(容差在半径0.025内)
		int index = matchPoint(tempX, tempY);
		if (index >= 0) {	//有匹配的，删除这个点
			deletePoint(index);
		}
	}
}
//判断按下的位置是否对应某个点(容差在半径0.025内)
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
//删除一个点
void deletePoint(int index) {
	int i = 0;
	if (index < pointNum - 1) {	//不是最后一个点,后面的点往前移
		for (i = index; i < pointNum - 1; i++) {
			pointsArr[i * 3] = pointsArr[(i + 1) * 3];
			pointsArr[i * 3 + 1] = pointsArr[(i + 1) * 3 + 1];
			pointsArr[i * 3 + 2] = pointsArr[(i + 1) * 3 + 2];
		}
	}
	pointNum--;	//最后一个点直接pointNum--
}

//鼠标移动监控
void mouseMove(GLFWwindow* window, double xpos, double ypos) {
	mouseCoor.x = xpos;
	mouseCoor.y = ypos;
	int i;
	//判断是否有某个点是可移动的(true)
	for (i = 0; i < 4; i++) {
		if (pointMove[i] == true) {
			break;
		}
	}
	if (i < 4) {	//有点在移动中,实时更新
		pointsArr[i * 3] = mouseCoor.x / (window_width * 0.5) - 1.0;
		pointsArr[i * 3 + 1] = (window_height - mouseCoor.y) / (window_height * 0.5) - 1.0;
		pointsArr[i * 3 + 2] = 0.0;
	}
}

/*
* 绘制由4个点决定的三阶贝塞尔曲线
* 数组内点的顺序如下：
* [0]是起始点；[1]、[2]是控制点；[3]是终点
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


