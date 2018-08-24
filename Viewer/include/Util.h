#pragma once

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

    static bool isVecEqual(glm::vec3 v1, glm::vec3 v2);
    static bool isVecEqual(glm::vec4 v1, glm::vec4 v2);
    static bool isVecEqual(glm::vec2 v1, glm::vec2 v2);
    static bool isInRange(float x, float min, float max);
    //Color handling


    static glm::vec4 getColor(R_COLOR color);
};