#include "Light.h"

Light::Light(glm::vec3 p, float i) {
	position = p;
	intensity = i;
}

Light::~Light(void)
{
}
