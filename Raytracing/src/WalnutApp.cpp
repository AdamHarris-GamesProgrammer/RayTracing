#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "../Renderer.h"
#include "../Camera.h"

#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <fstream>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : _camera(45.0f, 0.1f, 100.0f) {


	}

	virtual void OnUpdate(float ts) override {
		if (_camera.OnUpdate(ts)) {
			_renderer.ResetFrameIndex();
		}
	}


	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");

		if (ImGui::Button("Render")) {
			Render();
		}

		ImGui::Checkbox("Accumulate", &_renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset")) {
			_renderer.ResetFrameIndex();
		}

		ImGui::Text("Last render: %.3fms", _lastRenderTime);

		ImGui::End();

		ImGui::Begin("Scene");

		static char sceneName[128] = "NewScene";
		ImGui::InputText("Scene Name", sceneName, IM_ARRAYSIZE(sceneName));
		if (ImGui::Button("Save Scene")) {
			SaveScene(sceneName);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene")) {
			LoadScene(sceneName);
		}

		if (ImGui::Button("Add Sphere")) {
			_scene.spheres.emplace_back();
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Material")) {
			_scene.materials.emplace_back();
		}

		ImGui::Separator();

		ImGui::Text("Spheres");

		
		int indexToDelete = -1;
		for (size_t i = 0; i < _scene.spheres.size(); i++) {
			ImGui::PushID(i);

			Sphere& sphere = _scene.spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
			ImGui::SliderInt("Material", &sphere.materialIndex, 0, (int)_scene.materials.size() - 1);
			if (ImGui::Button("Delete Sphere")) {
				indexToDelete = 0;
			}
			ImGui::Separator();

			ImGui::PopID();
		}

		if (indexToDelete != -1) {
			_scene.spheres.erase(_scene.spheres.begin() + indexToDelete);
			indexToDelete = -1;
		}

		ImGui::Text("Materials");

		for (size_t i = 0; i < _scene.materials.size(); i++) {
			ImGui::PushID(i);

			Material& mat = _scene.materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.albedo), 0.1f);
			ImGui::DragFloat("Roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &mat.metallic, 0.01f, 0.0f, 1.0f);

			if (ImGui::Button("Delete Material")) {
				indexToDelete = i;
			}

			ImGui::Separator();

			ImGui::PopID();
		}

		if (indexToDelete != -1) {
			_scene.materials.erase(_scene.materials.begin() + indexToDelete);
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
		_renderer.Render(_scene, _camera);

		_lastRenderTime = timer.ElapsedMillis();
	}

	void SaveScene(char* sceneName) {

		std::stringstream ss;

		ss << "Scene (\n";
		ss << "\tscenename: " << sceneName << "\n";
		ss << ")\n";

		for (size_t i = 0; i < _scene.spheres.size(); i++) {
			const Sphere& sphere = _scene.spheres[i];

			ss << "sphere (\n";
			ss << "\tposition: " << sphere.pos.x << ", " << sphere.pos.y << ", " << sphere.pos.z << "\n";
			ss << "\tradius: " << sphere.radius << "\n";
			ss << "\tmaterialIndex: " << sphere.materialIndex << "\n";
			ss << ")\n";
		}

		for (size_t i = 0; i < _scene.materials.size(); i++) {
			const Material& mat = _scene.materials[i];

			ss << "material (\n";
			ss << "\talbedo: " << mat.albedo.x << ", " << mat.albedo.y << ", " << mat.albedo.z << "\n";
			ss << "\troughness: " << mat.roughness << "\n";
			ss << "\tmetallic: " << mat.metallic << "\n";
			ss << ")\n";
		}

		std::string sn = "Scenes/" + std::string(sceneName) + ".scene";
		
		std::ofstream file(sn);

		file.write(ss.str().c_str(), ss.str().length());
		file.close();

	}

	void LoadScene(const char* sceneName) {
		std::string sn = "Scenes/" + std::string(sceneName) + ".scene";

		std::ifstream inFile(sn);

		if (!inFile.good()) return;

		Scene ns;

		std::string line;
		while (getline(inFile, line)) {
			if (line.find("scene (") != std::string::npos) {
				std::vector<std::string> sceneAttributes = GetContentInNode(inFile, ")");
				LoadSceneAttributes(ns, sceneAttributes);
			}
			else if (line.find("sphere (") != std::string::npos) {
				std::vector<std::string> sphereAttributes = GetContentInNode(inFile, ")");
				LoadSphereAttributes(ns, sphereAttributes);
			}
			else if (line.find("material (") != std::string::npos) {
				std::vector<std::string> materialAttributes = GetContentInNode(inFile, ")");
				LoadMaterialAttributes(ns, materialAttributes);
			}
		}

		_scene = ns;
	}

private:
	void LoadSceneAttributes(Scene& scene, std::vector<std::string>& data) {
		for (size_t i = 0; i < data.size(); ++i) {
			const std::string& line = data[i];

			if (line.find("scenename") != std::string::npos) {
				scene.name = GetDataAfterColon(line);
			}
		}
	}

	void LoadSphereAttributes(Scene& scene, std::vector<std::string>& data) {
		Sphere ns;

		for (size_t i = 0; i < data.size(); ++i) {
			const std::string& line = data[i];

			if (line.find("position") != std::string::npos) {
				std::vector<std::string> posData = split(GetDataAfterColon(line), ',');
				ns.pos = {
					std::stof(posData[0]),
					std::stof(posData[1]),
					std::stof(posData[2])
				};
			}
			else if (line.find("radius") != std::string::npos) {
				ns.radius = std::stof(GetDataAfterColon(line));
			}
			else if (line.find("materialIndex") != std::string::npos) {
				ns.materialIndex = std::stoi(GetDataAfterColon(line));
			}
		}

		scene.spheres.emplace_back(ns);
	}

	void LoadMaterialAttributes(Scene& scene, std::vector<std::string>& data) {
		Material mat;

		for (size_t i = 0; i < data.size(); ++i) {
			const std::string& line = data[i];

			if (line.find("albedo") != std::string::npos) {
				std::vector<std::string> colData = split(GetDataAfterColon(line), ',');
				mat.albedo = {
					std::stof(colData[0]),
					std::stof(colData[1]),
					std::stof(colData[2])
				};
			}
			else if (line.find("roughness") != std::string::npos) {
				mat.roughness = std::stof(GetDataAfterColon(line));
			}
			else if (line.find("metallic") != std::string::npos) {
				mat.metallic = std::stof(GetDataAfterColon(line));
			}
		}

		scene.materials.emplace_back(mat);
	}

	std::string GetDataAfterColon(const std::string& line) {
		int locOfColon = line.find(":");
		if (locOfColon == std::string::npos) return "";

		std::string data = line.substr(locOfColon + 2);
		return data;
	}

	std::vector<std::string> GetContentInNode(std::ifstream& file, char* delim) {
		std::string line;
		std::vector<std::string> data;
		do {
			getline(file, line);
			data.emplace_back(line);
		} while (line.find(delim) == std::string::npos);

		return data;
	}

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;

		//converts the passed in string to a stringstream. Needed for getline()
		std::istringstream iss(s);

		std::string item;
		while (getline(iss, item, delim)) {
			//Adds the item to the elements vector
			elems.push_back(item);
		}

		return elems;
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

	std::shared_ptr<ExampleLayer> layer = std::make_shared<ExampleLayer>();

	layer->LoadScene("NewScene");

	app->PushLayer(layer);
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