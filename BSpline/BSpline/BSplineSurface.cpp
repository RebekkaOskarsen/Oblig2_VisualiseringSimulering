#include "BSplineSurface.h"
#include <iostream>

BSplineSurface::BSplineSurface()
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    initControlPoints();
    generateSurface();
    setupBuffers();
}

BSplineSurface::~BSplineSurface()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void BSplineSurface::initControlPoints()
{
    controlPoints =
    {
       /* glm::vec3(0, 0, 0), */glm::vec3(1, 0, 0), glm::vec3(2, 0, 0), glm::vec3(3, 0, 0),
      /*  glm::vec3(0, 1, 0),*/ glm::vec3(1, 1, 2), glm::vec3(2, 1, 2), glm::vec3(3, 1, 0),
      /*  glm::vec3(0, 2, 0),*/ glm::vec3(1, 2, 0), glm::vec3(2, 2, 0), glm::vec3(3, 2, 0)
    };
}

void BSplineSurface::generateSurface()
{
    float step = 0.03f;

    // Number of control points in each direction
    int numControlPointsU = 3;  // Corresponds to the number of columns
    int numControlPointsV = 3;  // Corresponds to the number of rows

    for (float u = 0.0f; u <= 1.0f; u += step)
    {
        for (float v = 0.0f; v <= 1.0f; v += step)
        {
            glm::vec3 point(0.0f);

            for (int i = 0; i < numControlPointsU; ++i)  // Iterate over control points in the U direction
            {
                for (int j = 0; j < numControlPointsV; ++j)  // Iterate over control points in the V direction
                {
                    float Bu = N(i, d_u, u, knotVectorU);  // Basis function in U
                    float Bv = N(j, d_v, v, knotVectorV);  // Basis function in V
                    point += Bu * Bv * controlPoints[i * numControlPointsV + j];  // Correct index access
                }
            }
            surfaceVertices.push_back(point);

            // Debug print to check if vertices are generated
            std::cout << "Vertex: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
        }
    }

    // Generate indices for triangle mesh
    int numU = static_cast<int>(1.0f / step) + 1;
    int numV = static_cast<int>(1.0f / step) + 1;

    for (int i = 0; i < numV - 1; ++i) {
        for (int j = 0; j < numU - 1; ++j) {
            surfaceIndices.push_back(i * numU + j);
            surfaceIndices.push_back((i + 1) * numU + j);
            surfaceIndices.push_back(i * numU + (j + 1));

            surfaceIndices.push_back(i * numU + (j + 1));
            surfaceIndices.push_back((i + 1) * numU + j);
            surfaceIndices.push_back((i + 1) * numU + (j + 1));
        }
    }
}

void BSplineSurface::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, surfaceVertices.size() * sizeof(glm::vec3), &surfaceVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceIndices.size() * sizeof(unsigned int), &surfaceIndices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

float BSplineSurface::N(int i, int d, float t, const std::vector<float>& knots)
{
    if (d == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }

    float term1 = 0.0f, term2 = 0.0f;

    if ((i + d < knots.size()) && (knots[i + d] - knots[i]) != 0) {
        term1 = (t - knots[i]) / (knots[i + d] - knots[i]) * N(i, d - 1, t, knots);
    }

    if ((i + d + 1 < knots.size()) && (knots[i + d + 1] - knots[i + 1]) != 0) {
        term2 = (knots[i + d + 1] - t) / (knots[i + d + 1] - knots[i + 1]) * N(i + 1, d - 1, t, knots);
    }

    return term1 + term2;
}


void BSplineSurface::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, surfaceIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}