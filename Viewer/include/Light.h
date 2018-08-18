#pragma once
#include "Model.h"

/*
� Point source : All light originates at a point
� Rays hit planar surface at different incidence angles
� Parallel source : All light rays are parallel
� Rays hit a planar surface at identical incidence angles
� May be modeled as point source at infinity
� Also called directional source
� Area source : Light originates at finite area in space.
� In between the point and parallel sources
� Also called distributed source
� Ambient light : Light reflected many times, comes equally from all directions


� Specular reflection
� Smooth surface
� Reflects light at defined angle
� Diffuse reflection
� Rough surface
� Reflects light in all directions
* /
/*
 * Light class. Holds light source information and data.
 */
class Light
{
private:
    PModel m_pLightSourceModel;
    //vec4 operator() (const vec3& point, const vec4& color, const Surface& surf) = 0;

public:
    PModel GetLightModel() { return m_pLightSourceModel; }
	Light();
	~Light();
};

class PointSourceLight : public Light
{
private:

public:
    PointSourceLight() = default;
    ~PointSourceLight() = default;
};

class ParallelSourceLight : public Light
{
private:

public:
    ParallelSourceLight() = default;
    ~ParallelSourceLight() = default;
};

class DistributedSourceLight : public Light
{
private:

public:
    DistributedSourceLight() = default;
    ~DistributedSourceLight() = default;
};