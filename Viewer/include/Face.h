#pragma once

#include "Util.h"

class Surface
{
public:
    std::string m_material;

    glm::vec4 m_ambientColor;
    glm::vec4 m_diffuseColor;
    glm::vec4 m_specularColor;

    float m_ambientReflectionRate;
    float m_diffuseReflectionRate;
    float m_specularReflectionRate;
    int m_shininess;

    Surface() : m_material("Empty"),
        m_ambientReflectionRate(1.f),
        m_ambientColor(COLOR(BLACK)),
        m_diffuseReflectionRate(1.f),
        m_diffuseColor(COLOR(BLACK)),
        m_specularReflectionRate(0.2f),
        m_specularColor(COLOR(BLACK)),
        m_shininess(1)
    {}
    Surface(const std::string& material, 
            const glm::vec4& ambientC, float ambientI,
            const glm::vec4& diffusiveC, float diffusiveI,
            const glm::vec4& specularC, float specularI,
            float shininess) :
            m_material(material),
            m_ambientColor(ambientC), m_ambientReflectionRate(ambientI),
            m_diffuseColor(diffusiveC), m_diffuseReflectionRate(diffusiveI),
            m_specularColor(specularC), m_specularReflectionRate(specularI),
            m_shininess(shininess) {}
    Surface(const Surface& surf) : 
        Surface(surf.m_material,
        surf.m_ambientColor, surf.m_ambientReflectionRate,
        surf.m_diffuseColor, surf.m_diffuseReflectionRate,
        surf.m_specularColor, surf.m_specularReflectionRate,
        surf.m_shininess) {}

    ~Surface() = default;

};

class Face
{
public:
    glm::vec3 m_p1;
    glm::vec3 m_p2;
    glm::vec3 m_p3;
    glm::vec3 m_normal;
    Surface* m_surface; 
    glm::vec4 m_actualColorP1;
    glm::vec4 m_actualColorP2;
    glm::vec4 m_actualColorP3;
    glm::vec3 m_vn1;
    glm::vec3 m_vn2;
    glm::vec3 m_vn3;

    std::vector < std::pair<std::pair<float, glm::vec4>, std::pair<glm::vec3, glm::mat4x4>>> m_diffusiveColorAndSource;
    std::vector < std::pair<std::pair<float, glm::vec4>, std::pair<glm::vec3, glm::mat4x4>>> m_speculativeColorAndSource
        ;

  //  std::vector<std::pair<glm::vec4, glm::vec3>> m_specularColorAndSource;

    Face() = default;
    Face(const Face&) = default;
    Face(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, 
        const glm::vec3& normal, Surface* surf, 
        const glm::vec3& vn1, const glm::vec3& vn2, const glm::vec3& vn3) : 
        m_p1(p1), m_p2(p2), m_p3(p3), 
        m_actualColorP1(ZERO_VEC4), m_actualColorP2(ZERO_VEC4), m_actualColorP3(ZERO_VEC4),
        m_normal(normal), m_surface(surf), 
        m_vn1(vn1), m_vn2(vn2), m_vn3(vn3)
    {}
    ~Face() = default;

};