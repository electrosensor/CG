#pragma once

#include <glm/glm.hpp>

class Util
{
public:

	Util(){}

	~Util(){}

	//Coor conversion
	static glm::vec4 toHomogenicForm(const glm::vec3& normalForm);
	static glm::vec3 toNormalForm(const glm::vec4& homogenicForm);
	static glm::vec4 Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4);
};
