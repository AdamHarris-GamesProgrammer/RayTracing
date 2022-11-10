#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

Camera::Camera(float verticalFOV, float nearClip, float farClip)
	: _verticalFOV(verticalFOV), _nearClip(nearClip), _farClip(farClip)
{
	_forwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	_position = glm::vec3(0.0f, 0.0f, 3.0f);
}

bool Camera::OnUpdate(float ts)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	glm::vec2 delta = (mousePos - _lastMousePosition) * 0.002f;
	_lastMousePosition = mousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return false;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool moved = false;

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(_forwardDirection, upDirection);

	float speed = 5.0f;

	// Movement
	if (Input::IsKeyDown(KeyCode::W))
	{
		_position += _forwardDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		_position -= _forwardDirection * speed * ts;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::A))
	{
		_position -= rightDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		_position += rightDirection * speed * ts;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::Q))
	{
		_position -= upDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::E))
	{
		_position += upDirection * speed * ts;
		moved = true;
	}

	// Rotation
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitchDelta = delta.y * GetRotationSpeed();
		float yawDelta = delta.x * GetRotationSpeed();

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
		_forwardDirection = glm::rotate(q, _forwardDirection);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == _viewportWidth && height == _viewportHeight)
		return;

	_viewportWidth = width;
	_viewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
	return 0.8f;
}

void Camera::RecalculateProjection()
{
	_projection = glm::perspectiveFov(glm::radians(_verticalFOV), (float)_viewportWidth, (float)_viewportHeight, _nearClip, _farClip);
	_inverseProjection = glm::inverse(_projection);
}

void Camera::RecalculateView()
{
	_view = glm::lookAt(_position, _position + _forwardDirection, glm::vec3(0, 1, 0));
	_inverseView = glm::inverse(_view);
}

void Camera::RecalculateRayDirections()
{
	_rayDirections.resize(_viewportWidth * _viewportHeight);

	//Cycle through each pixel in the Y and X axis
	for (uint32_t y = 0; y < _viewportHeight; y++)
	{
		for (uint32_t x = 0; x < _viewportWidth; x++)
		{
			//Calculate pixel in -1 to 1 space
			glm::vec2 coord = { (float)x / (float)_viewportWidth, (float)y / (float)_viewportHeight };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			//Calculate the target by multiplying our inverse projection by the coordinate, converts -1 to 1 back into world space, from NDC to world
			glm::vec4 target = _inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(_inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));

			//Cache the ray direction due to this being very slow on CPU.
			_rayDirections[x + y * _viewportWidth] = rayDirection;
		}
	}
}
