#include "imgui.h"
#include "glad.h"
#include <GL/glfw3.h>
#include <iostream>
#include "imgui_impl_glfw_gl3.h"
#include <algorithm>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
int drawLine(int x0, int y0, int x1, int y1, float w, float h, int k);
void draw_triangle(int display_w, int display_h, unsigned int & VAO);
void draw_circle(int display_w, int display_h, unsigned int & VAO);
int drawCircle(int centerX, int centerY, int radius, float w, float h, int k);

int scan_line(int x_left, int x_right, int y, float w, float h, int k);
int flat_topped_triangle(int x0, int y0, int x1, int y1, int x2, int y2, float w, float h, int k);
int flat_triangle(int x0, int y0, int x1, int y1, int x2, int y2, float w, float h, int k);
int triangle_scanner(float w, float h, int k);

int window_width = 800;
int window_height = 500;
//图形选择: 0表示单个三角形，1表示两个三角形，2表示线段，3表示点
static int which_shape = 0;
float* point_arr = new float[window_width * window_height * 3];
bool fill_triangle = false; //是否填充三角形
							//着色器
const char *shader_src = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"void main() {\n"
"gl_Position = vec4(aPos, 1.0);\n"
"}";
const char *frag_src = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"FragColor = vec4(1,0,0, 1.0);}";
//三角形顶点
int vertices[] = {
	-150, -100, 0,
	150, -150, 0,
	0,  100, 0
};
//圆的中心&半径
int circle_points[] = {
	0, 0, 100
};
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
										//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//解绑
									//顶点颜色
									//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
									//glEnableVertexAttribArray(1);	//解绑
	glBindVertexArray(0);
	//创建选择颜色的菜单
	bool show_menu = true;
	bool color_picker = true;
	bool show_demo_window = true;

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//激活程序
		glUseProgram(shader_program);
		//glPointSize(2);
		if (which_shape == 0) {
			//画三角形
			draw_triangle(window_width, window_height, VAO);
		}
		else {
			//画圆
			draw_circle(window_width, window_height, VAO);
		}
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Menu", &show_menu);
		//改变图形
		int max_radius = (int)(min(window_width, window_height) / 2);
		if (ImGui::CollapsingHeader("Graph Shape")) {
			ImGui::RadioButton("Triangle", &which_shape, 0);
			if (which_shape == 0) {	//三角形，可选择是否填充
				ImGui::Checkbox("Fill the Triangle", &fill_triangle);
			}
			ImGui::RadioButton("Circle", &which_shape, 1);
			if (which_shape == 1) {	//圆，可设置半径大小
				ImGui::SliderInt("Circle Radius", &circle_points[2], 0, max_radius, "%.0f");
			}
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
	//终止
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	delete[] point_arr;
	return 0;
}


//辅助函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	delete[] point_arr;
	point_arr = new float[width * height * 3];
}
static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Error %d: %s\n", error, description);
}

//通过3条线绘制三角形
void draw_triangle(int display_w, int display_h, unsigned int & VAO) {
	float w = display_w / 2.0;
	float h = display_h / 2.0;
	int k = 0;
	k = drawLine(vertices[0], vertices[1], vertices[3], vertices[4], w, h, k);
	k = drawLine(vertices[0], vertices[1], vertices[6], vertices[7], w, h, k);
	k = drawLine(vertices[3], vertices[4], vertices[6], vertices[7], w, h, k);
	if (fill_triangle) {
		k = triangle_scanner(w, h, k);
	}
	glBufferData(GL_ARRAY_BUFFER, k * sizeof(float), point_arr, GL_STATIC_DRAW);	//将用户定义的数据复制到当前绑定缓冲
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, k / 3);
}
//画线函数,返回需要画线的像素个数
int drawLine(int x0, int y0, int x1, int y1, float w, float h, int k) {
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	float m = 1.0 * (y1 - y0) / (x1 - x0);	//斜率
	float b = y1 - m * x1;
	int the_y = y1 - y0, the_x = x1 - x0;
	int p = 2 * the_y - the_x;
	int now_x = x0, now_y = y0;
	//根据斜率取不同的方法
	//垂直直线
	point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
	if (x0 == x1) {
		while (now_y < y1) {
			now_y++;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	//水平直线
	else if (m == 0) {
		while (now_x < x1) {
			now_x++;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	//0<m<=1
	else if (m > 0 && m <= 1) {
		while (now_x < x1) {
			if (p > 0) {
				now_y++;
				p = p + 2 * the_y - 2 * the_x;
			}
			else {
				p = p + 2 * the_y;
			}
			now_x++;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	//m>1
	else if (m > 1) {
		p = the_y;
		while (now_y < y1) {
			if (p > 0) {
				now_x++;
				p = p + 2 * the_x - 2 * the_y;
			}
			else {
				p = p + 2 * the_x;
			}
			now_y++;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	//-1 <= m < 0
	else if (m >= -1 && m < 0) {
		p = 2 * the_y + the_x;
		while (now_x < x1) {
			if (p > 0) {
				p = p + 2 * the_y;
			}
			else {
				now_y--;
				p = p + 2 * the_y + 2 * the_x;
			}
			now_x++;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	//m < -1
	else {
		p = 2 * the_x - the_y;
		while (now_y > y1) {
			if (p > 0) {
				p = p - 2 * the_x;
			}
			else {
				now_x++;
				p = p - 2 * the_y - 2 * the_x;
			}
			now_y--;
			point_arr[k] = now_x / w; point_arr[k + 1] = now_y / h; point_arr[k + 2] = 0; k += 3;
		}
	}
	return k;
}
//画圆
void draw_circle(int display_w, int display_h, unsigned int & VAO) {
	float w = display_w / 2.0;
	float h = display_h / 2.0;
	int k = 0;
	k = drawCircle(circle_points[0], circle_points[1], circle_points[2], w, h, k);
	glBufferData(GL_ARRAY_BUFFER, k * sizeof(float), point_arr, GL_STATIC_DRAW);	//将用户定义的数据复制到当前绑定缓冲
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, k / 3);
}
//
int drawCircle(int centerX, int centerY, int radius, float w, float h, int k) {
	int now_x = 0;
	int now_y = radius;	//整个圆分为8份圆弧，只要画出其中一份，其他圆弧都能以对称画出
						//int end_x = (int)(radius / sqrt(2));
	int d = 1 - radius;
	while (now_x <= now_y) {
		//画8个点
		point_arr[k] = (centerX + now_x) / w; point_arr[k + 1] = (centerY + now_y) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX + now_x) / w; point_arr[k + 1] = (centerY - now_y) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX - now_x) / w; point_arr[k + 1] = (centerY + now_y) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX - now_x) / w; point_arr[k + 1] = (centerY - now_y) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX + now_y) / w; point_arr[k + 1] = (centerY + now_x) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX + now_y) / w; point_arr[k + 1] = (centerY - now_x) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX - now_y) / w; point_arr[k + 1] = (centerY + now_x) / h; point_arr[k + 2] = 0; k += 3;
		point_arr[k] = (centerX - now_y) / w; point_arr[k + 1] = (centerY - now_x) / h; point_arr[k + 2] = 0; k += 3;
		now_x++;
		if (d < 0) {
			d = d + 2 * now_x + 3;
		}
		else {
			d = d + 2 * (now_x - now_y) + 5;
			now_y--;
		}
	}
	return k;
}
//三角形内部填充
int triangle_scanner(float w, float h, int k) {
	int points[3][2];
	points[0][0] = vertices[0];
	points[0][1] = vertices[1];
	points[1][0] = vertices[3];
	points[1][1] = vertices[4];
	points[2][0] = vertices[6];
	points[2][1] = vertices[7];
	int temp[1][2];
	int i = 0, j = 0;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2 - i; j++) {
			if (points[j][1] > points[j + 1][1]) {
				temp[0][0] = points[j][0]; temp[0][1] = points[j][1];
				points[j][0] = points[j + 1][0];
				points[j][1] = points[j + 1][1];
				points[j + 1][0] = temp[0][0];
				points[j + 1][1] = temp[0][1];
			}
		}
	}
	//排序后: y0 >= y1 >= y2
	int x2 = points[0][0]; int y2 = points[0][1];
	int x1 = points[1][0]; int y1 = points[1][1];
	int x0 = points[2][0]; int y0 = points[2][1];
	//先判断三角形类型
	if (y0 == y1) {	//平顶三角形
		k = flat_topped_triangle(x0, y0, x1, y1, x2, y2, w, h, k);
	}
	else if (y1 == y2) {	//平底三角形
		k = flat_triangle(x0, y0, x1, y1, x2, y2, w, h, k);
	}
	else {		//将其分割为平底 + 平顶
				//找到分界点
		int tempy = y1;
		int tempx = (int)((tempy - y0) * (x2 - x0) / (y2 - y0) + x0 + 0.5);
		k = flat_topped_triangle(x1, y1, tempx, tempy, x2, y2, w, h, k);
		k = flat_triangle(x0, y0, x1, y1, tempx, tempy, w, h, k);
	}
	return k;
}
//平底三角形
int flat_triangle(int x0, int y0, int x1, int y1, int x2, int y2, float w, float h, int k) {
	int i = 0;
	int temp1, temp2;
	for (i = y0; i >= y1; i--) {
		temp1 = (int)((i - y0) * (x1 - x0) / (y1 - y0) + x0 + 0.5);
		temp2 = (int)((i - y0) * (x2 - x0) / (y2 - y0) + x0 + 0.5);
		if (temp1 > temp2) {
			k = scan_line(temp2, temp1, i, w, h, k);
		}
		else {
			k = scan_line(temp1, temp2, i, w, h, k);
		}
	}
	return k;
}
//平顶三角形
int flat_topped_triangle(int x0, int y0, int x1, int y1, int x2, int y2, float w, float h, int k) {
	int i = 0;
	int temp1, temp2;
	for (i = y0; i >= y2; i--) {
		temp1 = (int)((i - y2) * (x0 - x2) / (y0 - y2) + x2 + 0.5);
		temp2 = (int)((i - y2) * (x1 - x2) / (y1 - y2) + x2 + 0.5);
		if (temp1 > temp2) {
			k = scan_line(temp2, temp1, i, w, h, k);
		}
		else {
			k = scan_line(temp1, temp2, i, w, h, k);
		}
	}
	return k;
}
int scan_line(int x_left, int x_right, int y, float w, float h, int k) {
	int i = 0;
	float now_y = y / h;
	for (i = x_left; i <= x_right; i++) {
		point_arr[k] = i / w; point_arr[k + 1] = now_y; point_arr[k + 2] = 0; k += 3;
	}
	return k;
}