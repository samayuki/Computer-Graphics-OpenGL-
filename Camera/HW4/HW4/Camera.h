#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//键盘的移动速度
const float speed = 2.5f;
//鼠标移动的灵敏度
const float sensitivity = 0.05f;
class Camera {
private:
	//GLfloat pfov, pratio, pnear, pfar;
	GLfloat pitch, yaw;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
public:
	//构造函数
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat pitch_t = 0.0f, GLfloat yaw_t = -90.0f) {
		cameraPos = pos;
		cameraFront = front;
		cameraUp = up;
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		pitch = pitch_t;
		yaw = yaw_t;

	}
	//W、向前移动摄像机，distance为按一次键移动的距离
	void moveForward(GLfloat const deltaTime) {
		cameraPos += cameraFront * deltaTime * speed;
	}
	//S、向后移动摄像机
	void moveBack(GLfloat const deltaTime) {
		cameraPos -= cameraFront * deltaTime * speed;
	}
	//D、向右移动摄像机
	void moveRight(GLfloat const deltaTime) {
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		cameraPos -= cameraRight * deltaTime * speed;
	}
	//A、向左移动摄像机
	void moveLeft(GLfloat const deltaTime) {
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		cameraPos += cameraRight * deltaTime * speed;
	}
	//移动后的观察矩阵
	glm::mat4 getViewMatrix() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	//鼠标移动
	void rotate(GLfloat xoffset, GLfloat yoffset) {
		//乘以鼠标灵敏度
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		yaw += xoffset;
		pitch += yoffset;
		//限制：-89 <= pitch <= 89
		if (pitch > 89) {
			pitch = 89.0f;
		}
		else if (pitch < -89) {
			pitch = -89.0f;
		}
		//通过欧拉角计算方向向量
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(front);
	}
};

#endif