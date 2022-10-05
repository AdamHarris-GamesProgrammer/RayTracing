#include "Renderer.h"
#include <Walnut/Random.h>

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

			_imageData[x + y * _finalImage->GetWidth()] = PerPixel(coord);
		}
	}

	_finalImage->SetData(_imageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	//rayDirection = glm::normalize(rayDirection);

	glm::vec3 rayOrigin(0.0f, 0.0f, -3.0f);

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

	//greater than 0 means there is atleast 1 solution
	if (discriminant >= 0.0f) {
		float sqrt = sqrtf(discriminant);
		float bs = 2.0f * a;

		float t[2];
		t[0] = (-b + sqrt) / bs;
		t[1] = (-b - sqrt) / bs;

		for (int i = 0; i < 2; i++) {
			glm::vec3 hitPos = rayOrigin + rayDirection * t[i];
			glm::vec3 normal = hitPos - glm::vec3(0.0f);
			normal = glm::normalize(normal);

			uint8_t g = (hitPos.z * 255.0f);
			uint8_t r = (normal.x * 255.0f);
			return 0xff000000 | (g << 8) | r;
		}

	}
	else
	{
		return 0x00000000;
	}

	return 0xffff00ff;
}
