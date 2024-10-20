#pragma once

// class representing a vertex array object, which encapsulates a vertex buffer, an index
// buffer, and the vertex layout which specifies what data you store for each vertex in the model
class VertexArray {
public:
	VertexArray(const float* verts, unsigned int numVerts, 
		const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	// activate this vertex array (so we can draw it)
	void SetActive();

	unsigned int GetNumIndices() const {
		return mNumIndices;
	}

	unsigned int GetNumVerts() const {
		return mNumVerts;
	}

private:
	// how many vertices in the vertex buffer?
	unsigned int mNumVerts;
	// how many indices in the index buffer?
	unsigned int mNumIndices;
	// OpenGL ID of the vertex buffer
	unsigned int mVertexBuffer;
	// OpenGL ID of the index buffer
	unsigned int mIndexBuffer;
	// OpenGL ID of the vertex array object
	unsigned int mVertexArray;
};