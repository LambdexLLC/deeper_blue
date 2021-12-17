#include "application.hpp"
#include "env.hpp"

#include "utility/io.hpp"

namespace lbx::chess
{

	/**
	 * @brief Sets the file path to the directory the bot's executable is running in.
	 *
	 * If this is provided, the "set_env_folder_path()" function does not need to be
	 * invoked as the env path will be automatically determined.
	 *
	 * @param _filepath Path to the root directory.
	*/
	void set_application_root_path(const fs::path& _filepath)
	{
		// Ensure env folder exists
		const auto _envPath = _filepath / "env";
		if (!fs::exists(_envPath))
		{
			println("FATAL ERROR : Missing \"env\" folder, contact https://github.com/JonathanCline for help");
			std::exit(-1);
		};

		// Set env folder path
		set_env_folder_path(_envPath);

		// Ensure good env
		if (!load_env())
		{
			println("FATAL ERROR : Invalid \"env\" folder, contact https://github.com/JonathanCline for help");
			std::exit(-1);
		};

	};

};