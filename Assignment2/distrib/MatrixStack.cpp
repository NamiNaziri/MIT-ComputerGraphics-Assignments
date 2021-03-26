#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
	m_matrices.push_back(Matrix4f::identity());
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
	while (m_matrices.size() != size_t(1))
	{
		m_matrices.pop_back();
	}
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	
	return (m_matrices.back());
	
}

void MatrixStack::push( const Matrix4f& m )
{

	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m
	m_matrices.push_back(top() * m);
}

void MatrixStack::pop()
{
	m_matrices.pop_back();
	// Remove the top element from the stack
}
