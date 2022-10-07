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
	_activeScene = &scene;
	_activeCamera = &camera;

	const glm::vec3& rayOrigin = camera.GetPosition();

	//Render every pixel
	for (uint32_t y = 0; y < _finalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < _finalImage->GetWidth(); x++) {
			glm::vec4 color = PerPixel(x,y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			_imageData[x + y * _finalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	_finalImage->SetData(_imageData);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	const Sphere& closestSphere = _activeScene->spheres[objectIndex];


	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	glm::vec3 origin = ray.origin - closestSphere.pos;
	payload.WorldPosition = origin + ray.direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.pos;

	//return glm::vec4(spherecolor);

	return payload;
}

HitPayload Renderer::MissHit(const Ray& ray)
{
	HitPayload payload;
	payload.HitDistance = -1.0f;

	return payload;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.origin = _activeCamera->GetPosition();
	ray.direction = _activeCamera->GetRayDirections()[x + y * _finalImage->GetWidth()];


	glm::vec3 color(0.0f);

	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++) {
		HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f) {
			glm::vec3 skyColor = glm::vec3(0.5f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = _lightDir;

		float d = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

		const Sphere& sphere = _activeScene->spheres[payload.ObjectIndex];

		glm::vec3 spherecolor = sphere.albedo;
		spherecolor *= d;
		color += spherecolor * multiplier;

		multiplier *= 0.7f;

		ray.origin = payload.WorldPosition + payload.WorldNormal * 0.1f;
		ray.direction = glm::reflect(ray.direction, payload.WorldNormal);
	}



	return glm::vec4(color, 1.0f);
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	//Raytrace equation
	// A = ray origin
	// B = ray direction
	// R = sphere radius
	// T = hit distance
	//(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2  + az^2- r^2) = 0

	float lowestTDistance = std::numeric_limits<float>::max();
	int closestIndex = -1;

	for (uint32_t i = 0; i < _activeScene->spheres.size(); i++) {
		const Sphere& sphere = _activeScene->spheres[i];

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

		if (closestT > 0.0f && closestT < lowestTDistance) {
			lowestTDistance = closestT;
			closestIndex = i;
		}
	}

	//const Sphere& sphere = scene.spheres[0];

	if (closestIndex < 0) {
		return MissHit(ray);
	}

	return ClosestHit(ray, lowestTDistance, closestIndex);
}
