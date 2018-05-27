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
//��ɫ��
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
//4������
float vertices[] = {
	-0.4, -0.4, 0.0, 1.0, 0.0, 0.0,
	0.4, -0.4, 0.0, 0.0, 0.0, 1.0,
	0.0,  0.3, 0.0, 0.0, 1.0, 0.0,
	-0.2, -0.7, 0.0, 0.0, 1.0, 0.7
};
unsigned int indices[] = {
	0, 1, 2,		//��һ��������
	1, 2, 3			//�ڶ���������
};
unsigned int indice[] = {
	0, 1, 2,		//��һ��������
};
unsigned int* now_indice = indice;
float top_point_color[] = { 1.0, 0.0, 0.0 };
float left_point_color[] = { 0.0, 0.0, 1.0 };
float right_point_color[] = { 0.0, 1.0, 0.0 };
float down_point_color[] = { 0.4, 0.7, 0.3, 0.0 };
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
	//glfwSwapInterval(1);
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
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//���û���������ݸ��Ƶ���ǰ�󶨻���
	//����һ��EBO����
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
																				
	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//���
	//������ɫ
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	//���
	glBindVertexArray(0);
	//����ѡ����ɫ�Ĳ˵�
	bool show_menu = true;
	bool color_picker = true;
	//ͼ��ѡ��
	//0��ʾ���������Σ�1��ʾ���������Σ�2��ʾ�߶Σ�3��ʾ��
	static int which_shape = 0;

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}*/
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//�������
		glUseProgram(shader_program);
		//��������ɫ��ΪGUI�е���ɫ
		change_color();
		//���ı��vertices���¸�ֵ��������
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//���û���������ݸ��Ƶ���ǰ�󶨻���
		if (which_shape == 1) {	//2��������
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indice), indice, GL_STATIC_DRAW);
		}
		//����������
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
		//�ı�ͼ��
		if (ImGui::CollapsingHeader("Graph Shape")) {
			ImGui::RadioButton("Triangle", &which_shape, 0);
			ImGui::RadioButton("Two Triangles", &which_shape, 1);
			ImGui::RadioButton("Line", &which_shape, 2);
			ImGui::RadioButton("Point", &which_shape, 3);
		}
		//�ı���ɫ
		if (ImGui::CollapsingHeader("Color Picker")) {
			ImGui::ColorEdit3("top point", (float*)&top_point_color);
			ImGui::ColorEdit3("left point", (float*)&left_point_color);
			ImGui::ColorEdit3("right point", (float*)&right_point_color);
			if (which_shape == 1) {
				ImGui::ColorEdit3("down point", (float*)&down_point_color);
			}
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
void change_color() {
	vertices[3] = left_point_color[0]; vertices[4] = left_point_color[1]; vertices[5] = left_point_color[2];
	vertices[9] = right_point_color[0]; vertices[10] = right_point_color[1]; vertices[11] = right_point_color[2];
	vertices[15] = top_point_color[0]; vertices[16] = top_point_color[1]; vertices[17] = top_point_color[2];
	vertices[21] = down_point_color[0]; vertices[22] = down_point_color[1]; vertices[23] = down_point_color[2];
}