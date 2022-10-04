#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", _lastRenderTime);
		//if (ImGui::Button("Render")) {
		//	
		//}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		_viewportWidth = ImGui::GetContentRegionAvail().x;
		_viewportHeight = ImGui::GetContentRegionAvail().y;

		if (_image) {
			ImGui::Image(_image->GetDescriptorSet(), { (float)_image->GetWidth(), (float)_image->GetHeight() });
		}

		ImGui::End();
		ImGui::PopStyleVar();


		Render();
	}

	void Render() {
		Timer timer;

		if (!_image || _viewportWidth != _image->GetWidth() || _viewportHeight != _image->GetHeight()) {
			_image = std::make_shared<Image>(_viewportWidth, _viewportHeight, ImageFormat::RGBA);
			delete[] _imageData;
			_imageData = new uint32_t[_viewportWidth * _viewportHeight];
		}

		for (uint32_t i = 0; i < _viewportWidth * _viewportHeight; i++) {
			_imageData[i] = Random::UInt(); //ABGR
			_imageData[i] |= 0xff000000;
		}

		_image->SetData(_imageData);

		_lastRenderTime = timer.ElapsedMillis();
	}

private:
	uint32_t* _imageData = nullptr;
	std::shared_ptr<Image> _image;
	uint32_t _viewportWidth = 0, _viewportHeight = 0;

	float _lastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

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