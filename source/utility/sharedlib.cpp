#include "sharedlib.hpp"

#include <lambdex/utility/os.h>
#include <jclib/config.h>

#if LAMBDEX_OS_WINDOWS_V
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <libloaderapi.h>
#elif LAMBDEX_OS_LINUX_V
	#include <dlfcn.h>
#else
	#error "Target platform unsupported or not recognized, bug me to stop being lazy"
#endif

// Error string for unsupported feature comile errors
// "Feature is not supported for the target OS"


namespace lbx::impl
{
#if LAMBDEX_OS_WINDOWS_V

	/**
	 * @brief Windows specific function to open a shared library with wide character string.
	 *
	 * @param _path Null terminate wide character string with path to the shared library.
	 * @return Owning shared library handle on success, null on error.
	*/
	inline SharedLibraryPtr open_shared_library(const wchar_t* _wpath)
	{
		return LoadLibraryW(_wpath);
	};
#endif

	/**
	 * @brief OS independent function to open a shared library.
	 *
	 * @param _path Null terminate string with path to the shared library.
	 * @return Owning shared library handle on success, null on error.
	*/
	SharedLibraryPtr open_shared_library(const char* _path)
	{
#if LAMBDEX_OS_WINDOWS_V
		return LoadLibraryA(_path);
#elif LAMBDEX_OS_LINUX_V
		return dlopen(_path, RTLD_LAZY);
#else
#error "Feature is not supported for the target OS"
#endif
	};

	/**
	 * @brief OS independent function to open a shared library.
	 *
	 * @param _path Path to the shared library.
	 * @return Owning shared library handle on success, null on error.
	*/
	SharedLibraryPtr open_shared_library(const fs::path& _path)
	{
		return open_shared_library(_path.c_str());
	};

	/**
	 * @brief OS independent function to close a shared library.
	 *
	 * @param _lib Library to close. This will be set to null before returning.
	*/
	void close_shared_library(SharedLibraryPtr& _lib)
	{
#if LAMBDEX_OS_WINDOWS_V
		const auto _result = FreeLibrary(static_cast<HMODULE>(_lib));
		if (!_result)
		{
			JCLIB_ABORT();
		};
#elif LAMBDEX_OS_LINUX_V
		const auto _result = dlclose(_lib);
		if (_result != 0)
		{
			JCLIB_ABORT();
		};
#else
#error "Feature is not supported for the target OS"
#endif
		// Null the lib pointer
		_lib = nullptr;
	};

	/**
	 * @brief OS independent function to find a symbol in a shared library.
	 *
	 * @param _lib Shared library handle, MUST NOT BE NULL
	 * @param _symbolName Null terminated string with the name of the symbol to load.
	 * @return The symbol's address or nullptr if not found.
	*/
	void* get_shared_library_symbol(const SharedLibraryPtr& _lib, const char* _symbolName)
	{
		// Must not be null
		JCLIB_ASSERT(_lib);

		// Platform specific symbol loading
#if LAMBDEX_OS_WINDOWS_V
		return GetProcAddress(static_cast<HMODULE>(_lib), _symbolName);
#elif LAMBDEX_OS_LINUX_V
		return dlsym(_lib, _symbolName);
#else
#error "Feature is not supported for the target OS"
#endif
	};
};