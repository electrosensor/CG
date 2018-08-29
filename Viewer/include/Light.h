#pragma once

#include "Defs.h"
#include "MeshModel.h"
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


class Light
{
protected:

    LIGHT_SOURCE_TYPE type;

    LightMeshModel* m_pLightModel;

    glm::vec4 m_ambientLightColor;
    float m_ambientLightIntensity;

    glm::vec4 m_specularLightColor;
    float m_specularLightIntensity;

    glm::vec4 m_diffusiveLightColor;
    float m_diffusiveLightIntensity;

public:

    Light(LIGHT_SOURCE_TYPE type, const glm::vec3& location, const glm::vec4& ambientC, float ambientI, const glm::vec4& diffusiveC, float diffusiveI) : m_pLightModel(new LightMeshModel(type, location)), m_ambientLightColor(ambientC), m_ambientLightIntensity(ambientI), m_diffusiveLightColor(diffusiveC), m_diffusiveLightIntensity(diffusiveI) {}
    virtual ~Light() { delete m_pLightModel; }
  
    LightMeshModel&  GetLightModel() { return *m_pLightModel; }

    void SetAmbientIntensity(float intensity)   { m_ambientLightIntensity   = intensity; }
    void SetDiffusiveIntensity(float intensity) { m_diffusiveLightIntensity = intensity; }
    void SetSpecularIntensity(float intensity)  { m_specularLightIntensity  = intensity; }

    float GetAmbientIntensity()   { return m_ambientLightIntensity;   }
    float GetDiffusiveIntensity() { return m_diffusiveLightIntensity; }
    float GetSpecularIntensity()  { return m_specularLightIntensity;  }

    void SetAmbientColor(const glm::vec4& color)    { m_ambientLightColor   = color; }
    void SetDiffusiveColor(const glm::vec4& color)  { m_diffusiveLightColor = color; }
    void SetSpecularColor(const glm::vec4& color)  { m_specularLightColor   = color; }

    glm::vec4 GetAmbientColor()   { return m_ambientLightColor; }
    glm::vec4 GetDiffusiveColor() { return m_diffusiveLightColor; }
    glm::vec4 GetSpecularColor()  { return m_specularLightColor; }

    virtual void Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf)
    {
        glm::vec4 ambientColor = m_ambientLightIntensity * m_ambientLightColor; // TODO calculate with modification

       polygon.m_diffusiveColorAndSource.push_back({ m_diffusiveLightIntensity * m_diffusiveLightColor, { GetLightModel().getCentroid(), lightModelTransf } }); // TODO calculate with modification
 
//        glm::vec4 specularColor = 

        polygon.m_actualColorP1 += ambientColor;
        polygon.m_actualColorP2 += ambientColor;
        polygon.m_actualColorP3 += ambientColor;
    }
};




class PointSourceLight : public Light
{
private:

    glm::vec3 lightSource = { 2.f, 2.f, 2.f };

public:
    PointSourceLight(const glm::vec3& location, const glm::vec4& ambientC, float ambientI, const glm::vec4& diffusiveC, float diffusiveI) : Light(LST_POINT, location, ambientC, ambientI, diffusiveC, diffusiveI) {}
    ~PointSourceLight() = default;
    void Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf);

};




class ParallelSourceLight : public Light
{
private:
    std::vector< glm::vec3 > lightSource;
public:
     ParallelSourceLight(const glm::vec3& location, const glm::vec4& ambientC, float ambientI, const glm::vec4& diffusiveC, float diffusiveI) : Light(LST_PARALLEL, location, ambientC, ambientI, diffusiveC, diffusiveI) {}
    ~ParallelSourceLight() = default;
    void Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf);
};




class DistributedSourceLight : public Light
{
private:
    std::vector< glm::vec3 > lightSource;
public:
     DistributedSourceLight(const glm::vec3& location, const glm::vec4& ambientC, float ambientI, const glm::vec4& diffusiveC, float diffusiveI) : Light(LST_AREA, location, ambientC, ambientI, diffusiveC, diffusiveI) {}
    ~DistributedSourceLight() = default;
    void Illuminate(Face & polygon, const glm::mat4x4& lightModelTransf);
};