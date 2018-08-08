#include "Util.h"
#include <limits>

glm::vec4 Util::toHomogeneousForm(const glm::vec3& normalForm)
{
	return glm::vec4(normalForm.x, normalForm.y, normalForm.z, 1);
}

glm::vec4 Util::expandToVec4(const glm::vec3 & vector)
{
	return glm::vec4(vector.x, vector.y, vector.z, 0);
}

glm::vec3 Util::toCartesianForm(const glm::vec4& homogeneousForm)
{
	return glm::vec3(homogeneousForm.x / homogeneousForm[3], homogeneousForm.y / homogeneousForm[3], homogeneousForm.z / homogeneousForm[3]);
}

glm::vec4 Util::Cross(const glm::vec4 & lVector4, const glm::vec4 & rVector4)
{
	return toHomogeneousForm(glm::cross(toCartesianForm(lVector4), toCartesianForm(rVector4))); // TODO_YURI: check
}

glm::vec3 Util::findCentralVec(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    return 0.5f*(p2 + p3) - p1;  
}

bool Util::isVecEqual(glm::vec3 v1, glm::vec3 v2)
{
    bool bEquals = true;

    bEquals &= fabs(v1.x - v2.x) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.y - v2.y) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.z - v2.z) < std::numeric_limits<float>::epsilon();

    return bEquals;
    

}

bool Util::isVecEqual(glm::vec4 v1, glm::vec4 v2)
{
    bool bEquals = true;

    bEquals &= fabs(v1.x - v2.x) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.y - v2.y) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.z - v2.z) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.w - v2.w) < std::numeric_limits<float>::epsilon();

    return bEquals;
}

glm::vec3 Util::getColor(R_COLOR color)
{
    switch (color)
    {
    case WHITE:
        return { 1.0f, 1.0f, 1.0f };
    case BLACK:
        return { 0.f, 0.f, 0.f };
    case LIME:
        return { 0, 1.0f ,0 };
    case GREEN:
        return { 0, 0.5f, 0 };
    case BLUE:
        return { 0, 0 ,1.0f };
    case RED:
        return { 1.0f, 0 ,0 };
    case YELLOW:
        return { 1.0f,1.0f,0 };
    default:
        return { 0.f, 0.f, 0.f };

    }
}
