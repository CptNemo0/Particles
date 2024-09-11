#include "pch.h"
#include "BallRenderer.h"

BallRenderer::BallRenderer(SOARepository* repository, Shader* shader)
{
	shader_ = shader;
	repository_ = repository;

	LoadModel(model_, "C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\Models\\ball.gltf");
	vao_and_ebo_ = BindModel(model_);

	binding_point_ = 1;
	glGenBuffers(1, &position_ssbo_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * BALL_NUMBER * 3, static_cast<const void*>(&(repository_->output_position_)), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_, position_ssbo_);

	shader_->Use();
	shader_->SetMatrix4("projection_matrix", projection_matrix_);
}

void BallRenderer::UpdateSSBO()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo_);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * BALL_NUMBER * 3, static_cast<const void*>(&(repository_->output_position_)));
}

void BallRenderer::Draw()
{
	shader_->Use();
	UpdateSSBO();
	InstanceModel(vao_and_ebo_, model_, BALL_NUMBER);
}

glm::mat4 BallRenderer::CreateViewMatrix(const glm::vec3 camera_position, const glm::vec3 camera_target)
{
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

	glm::vec3 forward = glm::normalize(camera_position - camera_target);
	glm::vec3 right = glm::normalize(cross(up, forward));
	glm::vec3 camera_up = glm::cross(forward, right);

	glm::mat4 view = glm::mat4
	(
		glm::vec4(right, 0.0),
		glm::vec4(camera_up, 0.0),
		glm::vec4(forward, 0.0),
		glm::vec4(0.0, 0.0, 0.0, 1.0)
	);

	view = transpose(view);
	view[3] = glm::vec4(-glm::dot(right, camera_position), -glm::dot(camera_up, camera_position), -glm::dot(forward, camera_position), 1.0);

	return view;
}

void BallRenderer::UpdateViewMatrix(const glm::vec3 camera_position, const glm::vec3 camera_target)
{
	view_matrix_ = CreateViewMatrix(camera_position, camera_target);
	shader_->Use();
	shader_->SetMatrix4("view_matrix", view_matrix_);
}

void BallRenderer::UpdateProjectionPatrix(const glm::mat4 matrix)
{
	projection_matrix_ = matrix;
	shader_->Use();
	shader_->SetMatrix4("projection_matrix", projection_matrix_);
}