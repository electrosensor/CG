#include "Util.h"
#include <limits>

using namespace std;
using namespace glm;

vec4 Util::toHomogeneousForm(const vec3& normalForm)
{
	return vec4(normalForm.x, normalForm.y, normalForm.z, 1);
}

vec4 Util::expandToVec4(const vec3 & vector)
{
	return vec4(vector.x, vector.y, vector.z, 0);
}

vec3 Util::toCartesianForm(const vec4& homogeneousForm)
{
    assert(homogeneousForm.w != 0);
	return vec3(homogeneousForm.x / homogeneousForm[3], homogeneousForm.y / homogeneousForm[3], homogeneousForm.z / homogeneousForm[3]);
}

vec4 Util::Cross(const vec4 & lVector4, const vec4 & rVector4)
{
    return toHomogeneousForm(cross(toCartesianForm(lVector4), toCartesianForm(rVector4)));
}

vec3 Util::findCentralVec(vec3 p1, vec3 p2, vec3 p3)
{
    return 0.5f*(p2 + p3) - p1;  
}

bool Util::isVecEqual(vec3 v1, vec3 v2)
{
    bool bEquals = true;

    bEquals &= fabs(v1.x - v2.x) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.y - v2.y) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.z - v2.z) < std::numeric_limits<float>::epsilon();

    return bEquals;
    

}

bool Util::isVecEqual(vec4 v1, vec4 v2)
{
    bool bEquals = true;

    bEquals &= fabs(v1.x - v2.x) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.y - v2.y) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.z - v2.z) < std::numeric_limits<float>::epsilon();
    bEquals &= fabs(v1.w - v2.w) < std::numeric_limits<float>::epsilon();

    return bEquals;
}

vec4 Util::getColor(R_COLOR color)
{
    switch (color)
    {
    case CLEAR:
        return { 0.4f, 0.55f, 0.60f, 1 };
    case WHITE:
        return { 1.0f, 1.0f, 1.0f, 1 };
    case BLACK:
        return { 0.f, 0.f, 0.f, 1 };
    case LIME:
        return { 0, 1.0f ,0, 1 };
    case GREEN:
        return { 0, 0.5f, 0, 1 };
    case BLUE:
        return { 0, 0 ,1.0f, 1 };
    case RED:
        return { 1.0f, 0 ,0, 1 };
    case YELLOW:
        return { 1.0f,1.0f,0, 1 };
    case X_COL:
        return { 1.f,102.f / 255.f,0.f, 1 };
    case Y_COL:
        return { 153.f / 255.f,204.f / 255.f,0.f, 1 };
    case Z_COL:
        return {11.f / 255.f, 62.f / 255, 215.f / 255, 1 };

        
    default:
        return { 0.f, 0.f, 0.f, 1 };

    }
}
