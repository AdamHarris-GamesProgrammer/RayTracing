#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "../Renderer.h"
#include "../Camera.h"

#include <glm/gtc/type_ptr.hpp>



using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : _camera(45.0f, 0.1f, 100.0f) {
		Sphere s1;
		s1.pos = glm::vec3{ 0.0f };
		s1.radius = 0.5f;
		s1.albedo = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);

		Sphere s2;
		s2.pos = glm::vec3{ 0.3f, 0.0f, -5.0f };
		s2.radius = 5.0f;
		s2.albedo = glm::vec4(0.4f, 1.0f, 0.4f, 1.0f);

		_scene.spheres.emplace_back(s1);
		_scene.spheres.emplace_back(s2);
	}

	virtual void OnUpdate(float ts) override {
		_camera.OnUpdate(ts);
	}


	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", _lastRenderTime);

		ImGui::End();

		ImGui::Begin("Scene");

		for (size_t i = 0; i < _scene.spheres.size(); i++) {
			ImGui::PushID(i);

			Sphere& sphere = _scene.spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f);
			ImGui::ColorEdit4("Albedo", glm::value_ptr(sphere.albedo), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.1f);

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		_viewportWidth = ImGui::GetContentRegionAvail().x;
		_viewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = _renderer.GetFinalImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();


		Render();
	}

	void Render() {
		Timer timer;

		_camera.OnResize(_viewportWidth, _viewportHeight);
		_renderer.OnResize(_viewportWidth, _viewportHeight);
		_renderer.Render(_scene ,_camera);

		_lastRenderTime = timer.ElapsedMillis();
	}

private:
	
	uint32_t _viewportWidth = 0, _viewportHeight = 0;

	Renderer _renderer;
	Camera _camera;
	Scene _scene;

	float _lastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Raytracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}