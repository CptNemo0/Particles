#pragma once

#include "Shader.h"
#include "gltf_load.h"
#include "glad.h"
#include "BallRepository.h"

class BallRenderer
{
public:
	Shader* shader_;
	SOARepository* repository_;

	tinygltf::Model model_;

	std::pair<GLuint, std::map<int, GLuint>> vao_and_ebo_;

	glm::mat4 view_matrix_;// = ViewMatrix({ 450.0f, 350.0f, 250.0f });
	glm::mat4 projection_matrix_;// = glm::perspective(90.0f, 1600.0f / 900.0f, 0.01f, 10000.00f);
	
	unsigned int position_ssbo_;
	unsigned int binding_point_;// = 1;

	BallRenderer() = default;
	BallRenderer(SOARepository* repository, Shader* shader);
	void UpdateSSBO();
	void Draw();

	glm::mat4 CreateViewMatrix(const glm::vec3 camera_position, const glm::vec3 camera_target);
	void UpdateViewMatrix(const glm::vec3 camera_position, const glm::vec3 camera_target);
	void UpdateProjectionPatrix(const glm::mat4 matrix);
};

