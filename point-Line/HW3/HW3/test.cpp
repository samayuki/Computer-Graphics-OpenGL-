
#include "imgui.h"
#include "glad.h"
#include <GL/glfw3.h>
#include <iostream>
#include "imgui_impl_glfw_gl3.h"
#include "shader_m.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
//void processInput(GLFWwindow *window);
void change_color();

int window_width = 800;
int window_height = 500;
//4个顶点
float vertices[] = {
    -0.5, -0.5, 0.0,
    0.5, -0.5, 0.0,
    0.5,  0.5, 0.0,
    -0.5, 0.5, 0.0
};

unsigned int indices[] = {
    0, 1, 2, 3      //第一个三角形 2
};
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

    Shader shader("shader/bezierCurve.vs", "shader/bezierCurve.fs");

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
    //创建选择颜色的菜单
    bool show_menu = true;

    //渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        //激活程序
        shader.use();
        glm::vec3 curveColor(0.5, 0.4, 0.7);
        shader.setVec3("curveColor", curveColor);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  //将用户定义的数据复制到当前绑定缓冲
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        //绘制三角形
        glBindVertexArray(VAO);
        glPointSize(5);
        glLineWidth(5);
        glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, 0);

        glfwPollEvents();

        ImGui_ImplGlfwGL3_NewFrame();

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

/*
 * 鼠标点击&移动事件规则
 * 鼠标点击（按下+松开）-> 画点（最多只能画4个点）
 * 鼠标点击&移动（按下+移动+松开 -> 移动点的位置）
 */
/*
 //左键按下画点
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
//鼠标移动监控
void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    mouseCoor.x = xpos;
    mouseCoor.y = ypos;
}

/*
* 绘制由4个点决定的三阶贝塞尔曲线
* 数组points中是4个Point结构体
* 数组内点的顺序如下：
* [0]是起始点；[1]、[2]是控制点；[3]是终点
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
