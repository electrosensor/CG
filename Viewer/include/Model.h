#pragma once
#include <vector>
#include <glm/glm.hpp>

using namespace std;
/*
 * Model class. An abstract class for all types of geometric data.
 */
class Model
{
public:
	virtual ~Model() {}
	virtual const glm::mat4x4& GetWorldTransform() = 0;
	virtual const glm::mat4x4& GetNormalTransform() = 0;
	virtual void SetWorldTransform(glm::mat4x4& transformation) = 0;
	virtual void SetNormalTransform(glm::mat4x4& transformation) = 0;
	virtual const vector<glm::vec3>* Draw() = 0;
};
