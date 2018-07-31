#include "Util.h"

glm::vec4 Util::toHomogeneousForm(const glm::vec3& normalForm)
{
	return glm::vec4(normalForm.x, normalForm.y, normalForm.z, 1);
}

glm::vec4 Util::expandToVec4(const glm::vec3 & vector)
{
	return glm::vec4(vector.x, vector.y, vector.z, 0);
}

glm::vec3 Util::toNormalForm(const glm::vec4& homogeneousForm)
{
	return glm::vec3(homogeneousForm.x / homogeneousForm[3], homogeneousForm.y / homogeneousForm[3], homogeneousForm.z / homogeneousForm[3]);
}

glm::vec4 Util::Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4)
{
	return toHomogeneousForm(glm::cross(toNormalForm(lVector4), toNormalForm(rVector4))); // TODO_YURI: check
}

glm::vec3 Util::findCentralVec(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    return 0.5f*(p2 + p3) - p1;  
}
