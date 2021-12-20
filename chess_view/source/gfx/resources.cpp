#include "resources.hpp"

#include "application/config.hpp"

namespace lbx::chess_view
{
	namespace
	{
		/**
		 * @brief Loads shaders from app config.
		 * @return Named shader pairs.
		*/
		inline auto load_shaders()
		{
			// Load in shaders
			std::unordered_map<std::string, gl::unique_program> _shaderPrograms{};
			try
			{
				const auto _shadersJson = get_config_value("shaders");
				const auto _root = get_application_root();

				for (auto& s : _shadersJson)
				{
					const std::string _name = s.at("name");
					const std::string_view _vertexPath = s.at("vertex_path");
					const std::string_view _fragmentPath = s.at("fragment_path");

					auto _program = load_simple_shader_program(_root / _vertexPath, _root / _fragmentPath);
					_shaderPrograms.insert_or_assign(_name, std::move(_program));
				};
			}
			catch (const lbx::json::exception& _exc)
			{
				std::cout << _exc.what() << '\n';
			};

			return _shaderPrograms;
		};
	};

	/**
	 * @brief Loads in resources using application config variables.
	 * @return True on good load, false otherwise.
	*/
	bool GFXResources::load()
	{
		auto _shaders = load_shaders();
		if (_shaders.empty())
		{
			return false;
		};

		this->board_shader = std::move(_shaders.at("board"));
		this->text_shader = std::move(_shaders.at("text"));

		return true;
	};
};
