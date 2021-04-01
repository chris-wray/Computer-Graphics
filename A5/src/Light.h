#pragma once
#include <glm/glm.hpp>

class Light
{
public:

	glm::vec3 position;
	float intensity;

	Light(glm::vec3 p, float i);
	~Light();

private:

};
