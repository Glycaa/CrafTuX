#include "OpenGLBuffer.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))
#define BUFFER_OFFSET_FLOAT(a) (BUFFER_OFFSET(a * sizeof(GLfloat)))

OpenGLBuffer::OpenGLBuffer(GLenum primitiveType) : m_primitiveType(primitiveType), b_allocated(false), b_dirty(true), i_vertexBufferId(0), i_indicesBufferId(0)
{
}

OpenGLBuffer::~OpenGLBuffer()
{
	clear();
	deleteBuffer();
}

void OpenGLBuffer::genBuffer()
{
	deleteBuffer();
	glGenBuffers(1, &i_vertexBufferId);
	glGenBuffers(1, &i_indicesBufferId);
	b_allocated = true;
}

void OpenGLBuffer::deleteBuffer()
{
	if(b_allocated) {
		glDeleteBuffers(1, &i_vertexBufferId);
		glDeleteBuffers(1, &i_indicesBufferId);
		b_allocated = false;
	}
}

void OpenGLBuffer::addVertice(const OpenGLVertice& vertice)
{
	m_vertex.push_back(vertice);
	b_dirty = true;
}

void OpenGLBuffer::addVertices(const OpenGLVertice& v1, const OpenGLVertice& v2)
{
	addVertice(v1); addVertice(v2);
}

void OpenGLBuffer::addVertices(const OpenGLVertice& v1, const OpenGLVertice& v2, const OpenGLVertice& v3, const OpenGLVertice& v4)
{
	addVertice(v1); addVertice(v2);
	addVertice(v3); addVertice(v4);
}

void OpenGLBuffer::render()
{
	if(b_allocated) {
		if(b_dirty) {
			fill();
		}

		glBindBuffer(GL_ARRAY_BUFFER, i_vertexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_indicesBufferId);

		glVertexPointer(3, // Coordinates per vertex
						GL_FLOAT, // Data type
						8*sizeof(GLfloat), // Offset between each vertice
						BUFFER_OFFSET_FLOAT(0)); // where is the first vertice


		glColorPointer(3, // Coordinates per color
					   GL_FLOAT, // Data type
					   8*sizeof(GLfloat), // Offset between each color
					   BUFFER_OFFSET_FLOAT(3)); // where is the first color

		glTexCoordPointer(2, // Coordinates per texture coordinate
						  GL_FLOAT, // Data type
						  8*sizeof(GLfloat), // Offset between each texture coordinate
						  BUFFER_OFFSET_FLOAT(6)); // where is the first texture coordinate

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Render !
		glDrawElements(m_primitiveType, m_vertex.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		// Safely disbale buffers (for other compenents of the program)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void OpenGLBuffer::clear()
{
	m_vertex.clear();
	b_dirty = true;
}

void OpenGLBuffer::fill()
{
	int numberOfVertex = m_vertex.size();
	GLuint* indicesBuffer = new GLuint[numberOfVertex];

	for(int i = 0; i < numberOfVertex; ++i)	indicesBuffer[i] = i;

	glBindBuffer(GL_ARRAY_BUFFER, i_vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, numberOfVertex * OPENGLVERTICE_SIZE * sizeof(GLfloat), &m_vertex[0], GL_STATIC_DRAW); // Send data
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Safely disbale buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfVertex * sizeof(GLuint), indicesBuffer, GL_STATIC_DRAW); // Send data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Safely disbale buffer
	// We finished to upload our indices buffer, so we can delete the temp array
	delete[] indicesBuffer;

	b_dirty = false;
}
