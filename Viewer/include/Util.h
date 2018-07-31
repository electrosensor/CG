#pragma once

#include <glm/glm.hpp>

class Util
{
public:

	Util(){}

	~Util(){}

	//Coor conversion
	static glm::vec4 toHomogeneousForm(const glm::vec3& normalForm);
	static glm::vec4 expandToVec4(const glm::vec3& vector);
	static glm::vec3 toNormalForm(const glm::vec4& homogenicForm);
	static glm::vec4 Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4);
};

