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
    virtual const glm::mat4x4& GetModelTransformation() = 0;
    virtual const glm::mat4x4& GetWorldTransformation() = 0;
    virtual const glm::mat4x4& GetNormalTransformation() = 0;
    virtual void SetModelTransformation(glm::mat4x4& transformation) = 0;
	virtual void SetWorldTransformation(glm::mat4x4& transformation) = 0;
	virtual void SetNormalTransformation(glm::mat4x4& transformation) = 0;
	virtual const pair<vector<glm::vec3>, vector<glm::vec3>>* Draw() = 0;
};
