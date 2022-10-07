#include "Renderer.h"
#include <Walnut/Random.h>

#include "Camera.h"

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	const glm::vec3& rayOrigin = camera.GetPosition();

	Ray ray;
	ray.origin = camera.GetPosition();

	//Render every pixel
	for (uint32_t y = 0; y < _finalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < _finalImage->GetWidth(); x++) {
			const glm::vec3& rayDirection = camera.GetRayDirections()[x+y * _finalImage->GetWidth()];

			ray.direction = camera.GetRayDirections()[x + y * _finalImage->GetWidth()];

			glm::vec4 color = TraceRay(scene, ray);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			_imageData[x + y * _finalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	_finalImage->SetData(_imageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	//glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);

	if(scene.spheres.size() == 0.0f)
		return glm::vec4(0.5, 0.7, 0.9, 1);

	//Raytrace equation
	// A = ray origin
	// B = ray direction
	// R = sphere radius
	// T = hit distance
	//(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2  + az^2- r^2) = 0

	float lowestTDistance = std::numeric_limits<float>::max();
	const Sphere* closestSphere = nullptr;

	for (const Sphere& sphere : scene.spheres) {
		//quadratic variables
		glm::vec3 origin = ray.origin - sphere.pos;

		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

		//Full quadratic formula
		//-b +- sqrt(b^ - 4ac)
		//		2a

		//Quadratic formula discriminant
		//b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		//(-b +- sqrt(discriminant)) / 2a

		//less than zero is black
		if (discriminant < 0.0f) {
			continue;
		}

		//Gets the t which will always be closest to the camera
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

		if (closestT < lowestTDistance) {
			lowestTDistance = closestT;
			closestSphere = &sphere;
		}
	}

	//const Sphere& sphere = scene.spheres[0];

	if (closestSphere == nullptr) {
		return glm::vec4(0.5, 0.7, 0.9, 1);
	}

	
	glm::vec3 origin = ray.origin - closestSphere->pos;
	glm::vec3 h1 = origin + ray.direction * lowestTDistance;

	glm::vec3 normal = glm::normalize(h1);

	glm::vec3 lightDir = _lightDir;

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec4 spherecolor = closestSphere->albedo;
	spherecolor *= d;

	return glm::vec4(spherecolor);
}
