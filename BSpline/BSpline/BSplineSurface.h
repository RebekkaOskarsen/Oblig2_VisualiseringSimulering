
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

	void DrawBSpline() const;

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
    float N(int i, int d, float t, const std::vector<float>& knots);

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

    GLuint VAO, VBO, EBO;
};

#endif // !BSPLINESURFACE_H



