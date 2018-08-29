#pragma once

#include "Util.h"

class Surface
{
public:
    std::string m_material;
    glm::vec4 m_baseColor;

//     glm::vec4 m_ambientColor;
//     glm::vec4 m_diffuseColor;
//     glm::vec4 m_specularColor;

    float m_diffuseReflectionRate;
    float m_specularReflectionRate;
//     float m_ambientReflectionRate;

    Surface() : m_material("Empty"), m_baseColor(COLOR(BLACK)), m_diffuseReflectionRate(1.f), m_specularReflectionRate(1.f){}
    Surface(const std::string& material, const glm::vec4& color, float diffusRate, float specularRate) : m_material(material), m_baseColor(color), m_diffuseReflectionRate(diffusRate), m_specularReflectionRate(specularRate) {}
    Surface(const Surface&) = default;
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

    std::vector<std::pair<glm::vec4, std::pair<glm::vec3, glm::mat4x4>>> m_diffusiveColorAndSource;

  //  std::vector<std::pair<glm::vec4, glm::vec3>> m_specularColorAndSource;

    Face() = default;
    Face(const Face&) = default;
    Face(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& normal, Surface* surf, const glm::vec3& vn1, const glm::vec3& vn2, const glm::vec3& vn3) : m_p1(p1), m_p2(p2), m_p3(p3), m_actualColorP1(COLOR(CLEAR)), m_actualColorP2(COLOR(CLEAR)), m_actualColorP3(COLOR(CLEAR)), m_normal(normal), m_surface(surf), m_vn1(vn1), m_vn2(vn2), m_vn3(vn3)
    {
//         if (m_surface)
//         {
//             m_actualColorP1 = surf->m_baseColor;
//             m_actualColorP3 = surf->m_baseColor;
//             m_actualColorP2 = surf->m_baseColor;
//         }
    }
    ~Face() = default;

};