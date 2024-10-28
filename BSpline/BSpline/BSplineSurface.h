
#ifndef BSPLINESURFACE_H
#define BSPLINESURFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shaderClass.h"

class BSplineSurface 
{
public:
	BSplineSurface();
	~BSplineSurface();

	void DrawBSpline(Shader shaderProgram) const;

private:
    // Initialiserer kontrollpunktene
    void initControlPoints();

    // Basert på kontrollpunktene genererer det B-Spline surface
    void generateSurface();

    // Buffere
    void setupBuffers();

    // Brukes til å konstruere surface ved å beregne B-Spline basisfunksjonen
    // Kontrollpunktindex = i, grad = d, parameteren = t, knot sekvens = knots
    // t Parameter man bruker for å tegne kurven. Finner ut av hvor mye et punkt bidrar til formen på kurven
    float N(int i, int d, float t, const std::vector<float>& knots) const;

    // Har lista over kontrollpunkter
    std::vector<glm::vec3> controlPoints;

    // Liste over vertices
    std::vector<glm::vec3> surfaceVertices;

    // Surface sin indekser som brukes for å tegne surface
    std::vector<unsigned int> surfaceIndices;

    // Disse er knot vektorene for parameterene U og V
    // Bestemmer hvordan kontrollpunktene på virker formen på surface
    std::vector<float> knotVectorU{ 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 2.0f, 2.0f };  
    std::vector<float> knotVectorV{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };  

    // Grad for B-Spline
    // u og v retning
    // Påvirker formen
    // [eit kvadrat av eit kvadrat]
    int d_u = 2; 
    int d_v = 2; 

    std::vector<glm::vec3> surfaceNormals; // Normals for each vertex
    void calculateNormals(); // New method to calculate normals
    void setupNormalBuffers(); // Sets up buffers for normal lines

    float NPrime(int i, int d, float t, const std::vector<float>& knots) const;

    glm::vec3 calculatePartialDerivativeU(float u, float v) const;
    glm::vec3 calculatePartialDerivativeV(float u, float v) const;

    GLuint normalVAO, normalVBO;

    GLuint VAO, VBO, EBO;
};

#endif // !BSPLINESURFACE_H



