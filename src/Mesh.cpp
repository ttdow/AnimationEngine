#include "Mesh.h"

namespace Engine
{
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures) :
		vertices(vertices), indices(indices), textures(textures)
	{
		SetupMesh();
	}

	void Mesh::Draw()
	{
		// Nothing yet.
	}

	void Mesh::SetupMesh()
	{
		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescription();
	}
}