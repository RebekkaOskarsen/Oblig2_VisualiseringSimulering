#include "BSplineSurface.h"
#include <iostream>

BSplineSurface::BSplineSurface()
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    normalVAO = 0;
    normalVBO = 0;

    initControlPoints();
    generateSurface();
    calculateNormals();
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
    // Kontrollpunktene
    // 3x3
    controlPoints =
    {
      glm::vec3(1, 0, 0), glm::vec3(2, 0, 0), glm::vec3(3, 0, 0),
      glm::vec3(1, 1, 2), glm::vec3(2, 1, 2), glm::vec3(3, 1, 0),
      glm::vec3(1, 2, 0), glm::vec3(2, 2, 0), glm::vec3(3, 2, 0)
    };
}

void BSplineSurface::generateSurface()
{
    float step = 0.03f;

    // Antall kontrollpunkter
    int numControlPointsU = 3; 
    int numControlPointsV = 3;

    for (float u = 0.0f; u <= 1.0f; u += step)
    {
        for (float v = 0.0f; v <= 1.0f; v += step)
        {
            glm::vec3 point(0.0f); // Startspunkt for hver surface punkt

            // Gå igjennom alle kontrollpunktene i U og V-retning
            for (int i = 0; i < numControlPointsU; ++i)  // Iterer over U-retningen
            {
                for (int j = 0; j < numControlPointsV; ++j)  // Iterer over V-retningen
                {
                    float Bu = N(i, d_u, u, knotVectorU);  // Bu er basisfunksjon i U-retning
                    float Bv = N(j, d_v, v, knotVectorV);  // Bv er basisfunksjon i V-retning

                    // Beregne et punkt på surface ved å bruke en vektet sum av alle kontrollpunktene
                    //  controlPoints[i * numControlPointsV + j] dette henter kontrollpunktet på posisjon i og j
                    point += Bu * Bv * controlPoints[i * numControlPointsV + j];
                }
            }
            surfaceVertices.push_back(point);
        }
    }

    // Genererer indekser for å lage triangler på surface
    int numU = static_cast<int>(1.0f / step) + 1;
    int numV = static_cast<int>(1.0f / step) + 1;

    for (int i = 0; i < numV - 1; ++i) {
        for (int j = 0; j < numU - 1; ++j) 
        {
            // Lager to trekanter for hvert firkantet område i gridet på surface
            // surfaceIndices lagrer indekser som peker til punktene i surfaceVertices
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

float BSplineSurface::N(int i, int d, float t, const std::vector<float>& knots) const
{
    // Beregner basisfunksjonen for B-spline
    // N(i, d, t)
    if (d == 0)  // Grad 0
    {
        // Sjekker om kontrollpunktene har noe innflytelse eller ikke
        // Returnerer funksjonen 1 så er t mellom  knots[i] og knots[i + 1], så betyr det at kontrollpunktet har innflytelse på kurven
        // hvis ikke t er i området vil det returneres 0, altså kontrollpunktene har ikke innflytelse
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }

    // Beregne vektene for kontrollpunktet under
    float term1 = 0.0f, term2 = 0.0f;
    // Beregner føste del
    if ((i + d < knots.size()) && (knots[i + d] - knots[i]) != 0) 
    {
        term1 = (t - knots[i]) / (knots[i + d] - knots[i]) * N(i, d - 1, t, knots);
    }
    // Beregner andre del
    if ((i + d + 1 < knots.size()) && (knots[i + d + 1] - knots[i + 1]) != 0) 
    {
        term2 = (knots[i + d + 1] - t) / (knots[i + d + 1] - knots[i + 1]) * N(i + 1, d - 1, t, knots);
    }

    // Summen av de to delene
    return term1 + term2;
}

void BSplineSurface::calculateNormals()
{
    surfaceNormals.clear();
    float step = 0.03f;

    for (float u = 0.0f; u <= 1.0f; u += step) {
        for (float v = 0.0f; v <= 1.0f; v += step) {
            glm::vec3 du = calculatePartialDerivativeU(u, v);
            glm::vec3 dv = calculatePartialDerivativeV(u, v);
            glm::vec3 normal = glm::normalize(glm::cross(du, dv));
            surfaceNormals.push_back(normal);
        }
    }
    // Debug output to check normal vectors
    for (const auto& normal : surfaceNormals) {
        std::cout << "Normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
    }
    setupNormalBuffers();
}

void BSplineSurface::setupNormalBuffers()
{
    // Prepare normal lines data
    std::vector<glm::vec3> normalLines;
    for (size_t i = 0; i < surfaceVertices.size(); ++i) {
        normalLines.push_back(surfaceVertices[i]);                     // Start of the line
        normalLines.push_back(surfaceVertices[i] + 0.1f * surfaceNormals[i]); // End of the line
    }

    // Generate and bind VAO for normals
    glGenVertexArrays(1, &normalVAO);
    glBindVertexArray(normalVAO);

    // Generate VBO for normals if not created
    if (normalVBO == 0) {
        glGenBuffers(1, &normalVBO);
    }

    // Bind and buffer data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3), &normalLines[0], GL_STATIC_DRAW);

    // Set up the vertex attribute for normals
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind
}



glm::vec3 BSplineSurface::calculatePartialDerivativeU(float u, float v) const
{
    glm::vec3 du(0.0f);
    int numControlPointsU = 3;
    int numControlPointsV = 3;

    for (int i = 0; i < numControlPointsU; ++i) {
        for (int j = 0; j < numControlPointsV; ++j) {
            float BuPrime = NPrime(i, d_u, u, knotVectorU); // Derivative of B-spline basis function w.r.t u
            float Bv = N(j, d_v, v, knotVectorV);
            du += BuPrime * Bv * controlPoints[i * numControlPointsV + j];
        }
    }
    return du;
}

glm::vec3 BSplineSurface::calculatePartialDerivativeV(float u, float v) const
{
    glm::vec3 dv(0.0f);
    int numControlPointsU = 3;
    int numControlPointsV = 3;

    for (int i = 0; i < numControlPointsU; ++i) {
        for (int j = 0; j < numControlPointsV; ++j) {
            float Bu = N(i, d_u, u, knotVectorU);
            float BvPrime = NPrime(j, d_v, v, knotVectorV); // Derivative of B-spline basis function w.r.t v
            dv += Bu * BvPrime * controlPoints[i * numControlPointsV + j];
        }
    }
    return dv;
}

float BSplineSurface::NPrime(int i, int d, float t, const std::vector<float>& knots) const
{
    if (d == 0) return 0.0f;

    float term1 = 0.0f, term2 = 0.0f;

    if ((i + d < knots.size()) && (knots[i + d] - knots[i]) != 0) {
        term1 = (d / (knots[i + d] - knots[i])) * N(i, d - 1, t, knots);
    }
    if ((i + d + 1 < knots.size()) && (knots[i + d + 1] - knots[i + 1]) != 0) {
        term2 = -(d / (knots[i + d + 1] - knots[i + 1])) * N(i + 1, d - 1, t, knots);
    }

    return term1 + term2;
}


void BSplineSurface::DrawBSpline(Shader shaderProgram) const
{
    shaderProgram.Activate();  // Activate the shader program

    // Draw the B-Spline surface
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, surfaceIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Draw normals as lines
    glBindVertexArray(normalVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(surfaceNormals.size() * 2));
    glBindVertexArray(0); // Unbind
}