#include "circle2D.h"

#include <vector>
#include <cmath>

#include "core/engine.h"
#include "utils/gl_utils.h"

// create circle
Mesh* circle2D::CreateCircle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float radius,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    int no_triangles = 100;
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color)
    };

    std::vector<unsigned int> indices;
    indices.push_back(0);
    for (int i = 1; i <= no_triangles + 1; i++) {
        vertices.push_back(VertexFormat(corner + glm::vec3((radius * cos(i * 2 * M_PI / no_triangles)), (radius * sin(i * 2 * M_PI / no_triangles)), 0), color));
        indices.push_back(i);
    }

    Mesh* circle = new Mesh(name);

    circle->SetDrawMode(GL_TRIANGLE_FAN);

    circle->InitFromData(vertices, indices);
    return circle;
}