#include "VertexArray.hpp"

VertexArray::VertexArray(const float* verts, unsigned int numVerts,
	const unsigned int* indices, unsigned int numIndices) {
	mNumVerts = numVerts;
	mNumIndices = numIndices;

	// create the vertex array object and store its ID in the mVertexArray variable
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// create a vertex buffer
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	// copy "verts" data into our vertex buffer
	glBufferData(
		GL_ARRAY_BUFFER,  // the active buffer type to write to (in this case, it's the vertex buffer type)
		(unsigned long long)(numVerts) * 8 * sizeof(float),  // number of bytes to copy - each vertex has 3 floats for its 3D position, 3 floats for its normal, and 2 floats for its texture coord
		verts,  // source to copy from (pointer)
		GL_STATIC_DRAW  // how will we use this data (in this case, we only want to load the data once and use it frequently for drawing)
	);

	// create an index buffer
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

	// copy "indices" data into our index buffer
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,  // index buffer type
		numIndices * sizeof(unsigned int),  // size of data
		indices,
		GL_STATIC_DRAW
	);

	// specify a vertex layout (also called the vertex attributes)
	// enable the first vertex attribute (attribute 0) for vertex position
	glEnableVertexAttribArray(0);
	// specify the size, type, and format of the attribute
	glVertexAttribPointer(
		0,  // attribute index (0 for first one)
		3,  // number of components (3 in this case for x, y, z coordinates of a vertex's position)
		GL_FLOAT,  // type of the components
		GL_FALSE,  // (only used for integral type)
		sizeof(float) * 8,  // stride - byte offset between consecutive vertices' attributes (usually size of each vertex)
		0  // offset from start of vertex to this attribute
	);

	// enable the second vertex attribute (attribute 1) for normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,  // vertex attribute index
		3,  // number of components (3 for normal xyz)
		GL_FLOAT,  // type of each component
		GL_FALSE,  // not used for GL_FLOAT
		sizeof(float) * 8,  // stride
		reinterpret_cast<void*>(sizeof(float) * 3)  // offset pointer
	);

	// enable the third vertex attribute (attribute 2) for texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,  // 2 components for u,v coords
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 8,
		reinterpret_cast<void*>(sizeof(float) * 6)
	);
}

VertexArray::~VertexArray() {
	// destroy the vertex buffer, index buffer, and vertex array object
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() {
	glBindVertexArray(mVertexArray);
}