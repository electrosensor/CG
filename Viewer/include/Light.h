#pragma once

#include "Defs.h"
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


� Specular reflection:
� Smooth surface
� Reflects light at defined angle
� Diffuse reflection:
� Rough surface
� Reflects light in all directions
* /
/*
 * Light class. Holds light source information and data.
 */

class LightMeshModel;

class Light
{
private:
    LightMeshModel* m_pLightModel;
    float m_intensity;


public:


// 
//     virtual glm::vec4 DiffuseReflection(const glm::vec4& color) const  = 0;
//     virtual glm::vec4 SpecularReflection(const glm::vec4& color) const = 0;

    Light(LightMeshModel* pLightModel) : m_pLightModel(pLightModel), m_intensity(1.f) {}
    virtual ~Light() { }
  
    LightMeshModel*  GetLightModel() { return m_pLightModel; }

    float GetIntencity() { return m_intensity; }

};




class PointSourceLight : public Light
{
private:
public:

    PointSourceLight(LightMeshModel* pLightModel) : Light(pLightModel) {}
    ~PointSourceLight() = default;
};




class ParallelSourceLight : public Light
{
private:

public:
     ParallelSourceLight(LightMeshModel* pLightModel) : Light(pLightModel) {}
    ~ParallelSourceLight() = default;
};




class DistributedSourceLight : public Light
{
private:

public:
     DistributedSourceLight(LightMeshModel* pLightModel) : Light(pLightModel) {}
    ~DistributedSourceLight() = default;
};