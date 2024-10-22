#ifndef BOX_H
#define BOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class Box
{
	public:
		Box();
		~Box();

		void DrawBox();

	private:
		GLuint VAO, VBO, EBO;
};
#endif // !BOX_H
