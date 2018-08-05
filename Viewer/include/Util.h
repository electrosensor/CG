#pragma once

#include <glm/glm.hpp>
#include "Defs.h"
#define COLOR(color) Util::getColor(color)

class Util
{
public:

	Util(){}

	~Util(){}

	//Coor conversion
	static glm::vec4 toHomogeneousForm(const glm::vec3& normalForm);
	static glm::vec4 expandToVec4(const glm::vec3& vector);
	static glm::vec3 toCartesianForm(const glm::vec4& homogeneousForm);
	static glm::vec4 Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4);
    static glm::vec3 findCentralVec(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);


    //Color handling


    static glm::vec3 getColor(R_COLOR color);
};

