#pragma once

#include <glm/glm.hpp>

class Util
{
public:

	Util(){}

	~Util(){}

	//Coor conversion
	static const glm::vec4 toHomogenicForm(const glm::vec3& normalForm);
	static const glm::vec3 toNormalForm(const glm::vec4& homogenicForm);

};

