#pragma once

#include "camera.h"

class Light :public Camera
{
public:

	Light(
		glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat yaw = YAW,
		GLfloat pitch = PITCH)
		: Camera(position, up, yaw, pitch){};
	

	virtual ~Light() {};
	void set_color(glm::vec3 clr) { m_color = clr; };
	glm::vec3 get_color(){ return m_color; }
	glm::vec3 m_color;

};