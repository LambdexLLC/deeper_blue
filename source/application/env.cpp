#include "env.hpp"

#include "utility/json.hpp"
#include "utility/http.hpp"

#include <jclib/type_traits.h>

#include <optional>

namespace lbx::chess
{
	/**
	 * @brief Environment variable storage
	*/
	struct Env
	{
		/**
		 * @brief Path to the environment folder
		*/
		fs::path env_root;

		/**
		 * @brief Holds the env variables
		*/
		struct Vars
		{
			/**
			 * @brief Lichess oath token string
			*/
			std::string lichess_oath_token{};
		};

		/**
		 * @brief Holds the env variables if they have been loaded
		*/
		std::optional<Vars> vars{ std::nullopt };
	};

	namespace
	{
		inline Env& get_env()
		{
			static Env _env{};
			return _env;
		};
	};




	/**
	 * @brief Sets the environment variable folder path
	 * @param _filepath File path
	*/
	void set_env_folder_path(const fs::path& _filepath)
	{
		auto& _env = get_env();
		JCLIB_ASSERT(fs::is_directory(_filepath));
		_env.env_root = _filepath;
	};

	/**
	 * @brief Loads in the environmnet variables. Make sure you've called set_env_folder_path()
	 * prior to calling this.
	 * @return True if everything loaded correctly, false otherwise.
	*/
	bool load_env()
	{
		auto& _env = get_env();
		if (!fs::is_directory(_env.env_root))
		{
			return false;
		};

		auto& _envRoot = _env.env_root;
		
		Env::Vars _envVars{};

		// Read lichess json
		json _lichessJson{};
		try
		{
			_lichessJson = read_json_file(_envRoot / "lichess.json");
			_envVars.lichess_oath_token = _lichessJson["token"];
		}
		catch (const json_exception& _exc)
		{
			return false;
		};

		// Set variables
		_env.vars = std::move(_envVars);

		// Good load
		return true;
	};

	/**
	 * @brief Creates a HTTP(s) authentication bearer token header using the lichess oath token
	*/
	std::pair<std::string, std::string> make_lichess_bearer_authentication_token_header()
	{
		auto& _env = get_env();
		JCLIB_ASSERT(_env.vars);
		auto& _vars = *_env.vars;

		return http::make_bearer_token_authentication_header(_vars.lichess_oath_token);
	};

	/**
	 * @brief Sets the bearer auth token for an http client to the env's lichess oath token
	 * @param _client HTTP client
	*/
	void set_lichess_bearer_token_auth(httplib::Client& _client)
	{
		auto& _env = get_env();
		JCLIB_ASSERT(_env.vars);
		auto& _vars = *_env.vars;
		_client.set_bearer_token_auth(_vars.lichess_oath_token.c_str());
	};
};