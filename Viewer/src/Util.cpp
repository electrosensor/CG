#include "Util.h"

const glm::vec4 Util::toHomogenicForm(const glm::vec3& normalForm)
{
	return glm::vec4(normalForm.x, normalForm.y, normalForm.z, 1);
}

const glm::vec3 Util::toNormalForm(const glm::vec4& homogenicForm)
{
	return glm::vec3(homogenicForm.x / homogenicForm[3], homogenicForm.y / homogenicForm[3], homogenicForm.z / homogenicForm[3]);
}