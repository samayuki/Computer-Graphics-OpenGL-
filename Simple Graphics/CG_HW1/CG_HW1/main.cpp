#include "imgui.h"
#include "glad.h"
#include <GL/glfw3.h>
#include <iostream>
#include "imgui_impl_glfw_gl3.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
//void processInput(GLFWwindow *window);
void change_color();

int window_width = 800;
int window_height = 500;
//着色器
const char *shader_src = "#version 330 core\n"
						 "layout(location = 0) in vec3 aPos;\n"
						 "layout(location = 1) in vec3 aColor;\n"
						 "out vec3 point_color;\n"
						 "void main() {\n"
							"gl_Position = vec4(aPos, 1.0);\n"
							"point_color = aColor;}";
const char *frag_src = "#version 330 core\n"
					   "out vec4 FragColor;\n"
					   "in vec3 point_color;\n"
					   "void main() {\n"
					   "FragColor = vec4(point_color, 1.0);}";
//4个顶点
float vertices[] = {
	-0.4, -0.4, 0.0, 1.0, 0.0, 0.0,
	0.4, -0.4, 0.0, 0.0, 0.0, 1.0,
	0.0,  0.3, 0.0, 0.0, 1.0, 0.0,
	-0.2, -0.7, 0.0, 0.0, 1.0, 0.7
};
unsigned int indices[] = {
	0, 1, 2,		//第一个三角形
	1, 2, 3			//第二个三角形
};
unsigned int indice[] = {
	0, 1, 2,		//第一个三角形
};
unsigned int* now_indice = indice;
float top_point_color[] = { 1.0, 0.0, 0.0 };
float left_point_color[] = { 0.0, 0.0, 1.0 };
float right_point_color[] = { 0.0, 1.0, 0.0 };
float down_point_color[] = { 0.4, 0.7, 0.3, 0.0 };
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
	//glfwSwapInterval(1);
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
	
	//创建顶点着色器
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &shader_src, NULL);
	glCompileShader(vertex_shader);
	//创建片段着色器
	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_src, NULL);
	glCompileShader(fragment_shader);
	//创建着色器程序
	unsigned int shader_program;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	//删除着色器对象
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	//生成一个VAO对象
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//生成一个VBO对象
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//将缓冲绑定
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//将用户定义的数据复制到当前绑定缓冲
	//生成一个EBO对象
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
																				
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//解绑
	//顶点颜色
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	//解绑
	glBindVertexArray(0);
	//创建选择颜色的菜单
	bool show_menu = true;
	bool color_picker = true;
	//图形选择
	//0表示单个三角形，1表示两个三角形，2表示线段，3表示点
	static int which_shape = 0;

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}*/
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//激活程序
		glUseProgram(shader_program);
		//将顶点颜色改为GUI中的颜色
		change_color();
		//将改变的vertices重新赋值到缓冲中
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//将用户定义的数据复制到当前绑定缓冲
		if (which_shape == 1) {	//2个三角形
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indice), indice, GL_STATIC_DRAW);
		}
		//绘制三角形
		glBindVertexArray(VAO);
		glPointSize(5);
		glLineWidth(5);
		if (which_shape == 2) {
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		}
		else if (which_shape == 3) {
			glDrawElements(GL_POINTS, 3, GL_UNSIGNED_INT, 0);
		}
		else if (which_shape == 0){
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glfwPollEvents();

		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Menu", &show_menu);
		//改变图形
		if (ImGui::CollapsingHeader("Graph Shape")) {
			ImGui::RadioButton("Triangle", &which_shape, 0);
			ImGui::RadioButton("Two Triangles", &which_shape, 1);
			ImGui::RadioButton("Line", &which_shape, 2);
			ImGui::RadioButton("Point", &which_shape, 3);
		}
		//改变颜色
		if (ImGui::CollapsingHeader("Color Picker")) {
			ImGui::ColorEdit3("top point", (float*)&top_point_color);
			ImGui::ColorEdit3("left point", (float*)&left_point_color);
			ImGui::ColorEdit3("right point", (float*)&right_point_color);
			if (which_shape == 1) {
				ImGui::ColorEdit3("down point", (float*)&down_point_color);
			}
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
void change_color() {
	vertices[3] = left_point_color[0]; vertices[4] = left_point_color[1]; vertices[5] = left_point_color[2];
	vertices[9] = right_point_color[0]; vertices[10] = right_point_color[1]; vertices[11] = right_point_color[2];
	vertices[15] = top_point_color[0]; vertices[16] = top_point_color[1]; vertices[17] = top_point_color[2];
	vertices[21] = down_point_color[0]; vertices[22] = down_point_color[1]; vertices[23] = down_point_color[2];
}