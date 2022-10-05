#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"
#include "../Renderer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", _lastRenderTime);

		glm::vec3 lightDir = _renderer.GetLightDir();
		float ld[3]{ lightDir.x, lightDir.y, lightDir.z };
		

		ImGui::SliderFloat3("Light Direction", ld, -1.0f, 1.0f, "%.3f", 1.f);

		lightDir.x = ld[0];
		lightDir.y = ld[1];
		lightDir.z = ld[2];

		_renderer.SetLightDir(lightDir);

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

		//renderer resize

		//renderer render

		_renderer.OnResize(_viewportWidth, _viewportHeight);
		_renderer.Render();

		_lastRenderTime = timer.ElapsedMillis();
	}

private:
	
	uint32_t _viewportWidth = 0, _viewportHeight = 0;

	Renderer _renderer;

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