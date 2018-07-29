#include "Util.h"

glm::vec4 Util::toHomogeniousForm(const glm::vec3& normalForm)
{
	return glm::vec4(normalForm.x, normalForm.y, normalForm.z, 1);
}

glm::vec4 Util::expandToVec4(const glm::vec3 & vector)
{
	return glm::vec4(vector.x, vector.y, vector.z, 0);
}

glm::vec3 Util::toNormalForm(const glm::vec4& homogenicForm)
{
	return glm::vec3(homogenicForm.x / homogenicForm[3], homogenicForm.y / homogenicForm[3], homogenicForm.z / homogenicForm[3]);
}

glm::vec4 Util::Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4)
{
	return toHomogeniousForm(glm::cross(toNormalForm(lVector4), toNormalForm(rVector4))); // TODO_YURI: check
}
