
#ifndef BSPLINESURFACE_H
#define BSPLINESURFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class BSplineSurface 
{
public:
	BSplineSurface();
	~BSplineSurface();

	void render() const;

private:
    void initControlPoints();  // Initialize control points
    void generateSurface();    // Generate the B-Spline surface
    void setupBuffers();       // Setup VAO, VBO, and EBO
    float N(int i, int d, float t, const std::vector<float>& knots);  // B-Spline basis function

    // Control points, vertices, and indices
    std::vector<glm::vec3> controlPoints;
    std::vector<glm::vec3> surfaceVertices;
    std::vector<unsigned int> surfaceIndices;

    // Knot vectors
    std::vector<float> knotVectorU{ 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 2.0f, 2.0f };  // Knot vector for U direction
    std::vector<float> knotVectorV{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };  // Knot vector for V direction

    // Degree for the B-Spline surface
    int d_u = 2;  // Degree in U direction
    int d_v = 2;  // Degree in V direction

    GLuint VAO, VBO, EBO;
};

#endif // !BSPLINESURFACE_H



