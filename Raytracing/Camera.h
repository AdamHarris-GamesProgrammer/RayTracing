#pragma once

#include <glm/glm.hpp>
#include <vector>

class Camera
{
public:
	Camera(float verticalFOV, float nearClip, float farClip);

	bool OnUpdate(float ts);
	void OnResize(uint32_t width, uint32_t height);

	const glm::mat4& GetProjection() const { return _projection; }
	const glm::mat4& GetInverseProjection() const { return _inverseProjection; }
	const glm::mat4& GetView() const { return _view; }
	const glm::mat4& GetInverseView() const { return _inverseView; }
	
	const glm::vec3& GetPosition() const { return _position; }
	const glm::vec3& GetDirection() const { return _forwardDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const { return _rayDirections; }

	float GetRotationSpeed();
private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();

private:
	glm::mat4 _projection{ 1.0f };
	glm::mat4 _view{ 1.0f };
	glm::mat4 _inverseProjection{ 1.0f };
	glm::mat4 _inverseView{ 1.0f };

	float _verticalFOV = 45.0f;
	float _nearClip = 0.1f;
	float _farClip = 100.0f;

	glm::vec3 _position{ 0.0f };
	glm::vec3 _forwardDirection{ 0.0f };

	std::vector<glm::vec3> _rayDirections;

	glm::vec2 _lastMousePosition{ 0.0f };

	uint32_t _viewportWidth = 0;
	uint32_t _viewportHeight = 0;
};

