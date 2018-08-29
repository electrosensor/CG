#include "Light.h"

void PointSourceLight::Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf)
{
    Light::Illuminate(polygon, lightModelTransf);
}

void ParallelSourceLight::Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf)
{
    Light::Illuminate(polygon, lightModelTransf);
}

void DistributedSourceLight::Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf)
{
    Light::Illuminate(polygon, lightModelTransf);
}