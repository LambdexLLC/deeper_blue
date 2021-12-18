#include "config.hpp"



namespace lbx::chess_view
{
	namespace
	{
		/**
		 * @brief Data structure for global application state
		*/
		struct ApplicationState
		{
			fs::path get_config_file_path() const
			{
				return this->root_path / "config.json";
			};

			fs::path root_path;
			json config;
		};

		inline auto& application_state()
		{
			static ApplicationState state{};
			return state;
		};

	};

	/**
	 * @brief Sets the path to the root directory of the application.
	 * @param _path
	*/
	void set_application_root(const fs::path& _path)
	{
		auto& _state = application_state();
		_state.root_path = _path;
	};

	/**
	 * @brief Gets the previously set application root path.
	 *
	 * This will return a default constructed path if set_application_root() was not called prior.
	 *
	 * @return Root directory path.
	*/
	fs::path get_application_root()
	{
		auto& _state = application_state();
		return _state.root_path;
	};

	/**
	 * @brief Loads the configuration file.
	 *
	 * Make sure to set the application root path prior to calling this.
	 *
	 * @return True on good load, false otherwise.
	*/
	bool load_config()
	{
		auto& _state = application_state();
		auto& _config = _state.config;

		_config = load_json_file(_state.get_config_file_path(), jc::nothrow);
		return !_config.is_discarded();
	};

	/**
	 * @brief Gets a configuration value as a json object.
	 * @param _name Name of the value.
	 * @return Json object with config data or a default constructed json object on error.
	*/
	json get_config_value(std::string_view _name)
	{
		auto& _state = application_state();
		if (const auto it = _state.config.find(_name); it != _state.config.end())
		{
			return *it;
		}
		else
		{
			return json{};
		};
	};
};
