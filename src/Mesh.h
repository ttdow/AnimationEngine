#pragma once

#include "VulkanStructs.h"

#include <vector>

namespace Engine
{
	class Mesh
	{
	public:

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		Mesh() = delete;
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
		~Mesh() = default;

		void Draw();

	private:

		uint32_t VAO, VBO, EBO;

		void SetupMesh();
	};
}