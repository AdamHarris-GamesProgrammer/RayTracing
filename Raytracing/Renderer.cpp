#include "Renderer.h"
#include <Walnut/Random.h>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;

		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (!_finalImage) {
		_finalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	else {
		if (_finalImage->GetWidth() == width && _finalImage->GetHeight() == height) {
			return;
		}

		_finalImage->Resize(width, height);
	}


	delete[] _imageData;
	_imageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	//Render every pixel
	for (uint32_t y = 0; y < _finalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < _finalImage->GetWidth(); x++) {
			glm::vec2 coord{ (float)x / (float)_finalImage->GetWidth(), (float)y / (float)_finalImage->GetHeight() };

			coord = coord * 2.0f - 1.0f; //-1 to 1

			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			_imageData[x + y * _finalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	_finalImage->SetData(_imageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	//rayDirection = glm::normalize(rayDirection);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);

	float radius = 0.5f;

	//Raytrace equation
	// A = ray origin
	// B = ray direction
	// R = sphere radius
	// T = hit distance
	//(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2  + az^2- r^2) = 0

	//quadratic variables
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//Full quadratic formula
	//-b +- sqrt(b^ - 4ac)
	//		2a

	//Quadratic formula discriminant
	//b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;


	//(-b +- sqrt(discriminant)) / 2a

	//less than zero is black
	if (discriminant < 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}

	//Two quadratic solutions
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	//glm::vec3 h0 = rayOrigin + rayDirection * t0;
	glm::vec3 h1 = rayOrigin + rayDirection * t1;

	glm::vec3 normal = glm::normalize(h1);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 spherecolor(1, 0, 1);
	spherecolor *= d;

	return glm::vec4(spherecolor, 1.0f);
}
