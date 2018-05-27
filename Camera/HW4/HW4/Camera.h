#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//���̵��ƶ��ٶ�
const float speed = 2.5f;
//����ƶ���������
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
	//���캯��
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat pitch_t = 0.0f, GLfloat yaw_t = -90.0f) {
		cameraPos = pos;
		cameraFront = front;
		cameraUp = up;
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		pitch = pitch_t;
		yaw = yaw_t;

	}
	//W����ǰ�ƶ��������distanceΪ��һ�μ��ƶ��ľ���
	void moveForward(GLfloat const deltaTime) {
		cameraPos += cameraFront * deltaTime * speed;
	}
	//S������ƶ������
	void moveBack(GLfloat const deltaTime) {
		cameraPos -= cameraFront * deltaTime * speed;
	}
	//D�������ƶ������
	void moveRight(GLfloat const deltaTime) {
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		cameraPos -= cameraRight * deltaTime * speed;
	}
	//A�������ƶ������
	void moveLeft(GLfloat const deltaTime) {
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		cameraPos += cameraRight * deltaTime * speed;
	}
	//�ƶ���Ĺ۲����
	glm::mat4 getViewMatrix() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	//����ƶ�
	void rotate(GLfloat xoffset, GLfloat yoffset) {
		//�������������
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		yaw += xoffset;
		pitch += yoffset;
		//���ƣ�-89 <= pitch <= 89
		if (pitch > 89) {
			pitch = 89.0f;
		}
		else if (pitch < -89) {
			pitch = -89.0f;
		}
		//ͨ��ŷ���Ǽ��㷽������
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(front);
	}
};

#endif