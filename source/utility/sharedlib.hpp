#pragma once

/*
	Cross-Platform shared library interaction interface
*/

#include "filesystem.hpp"

namespace lbx
{
	namespace impl
	{
		/**
		 * @brief Alias for the shared library pointer.
		*/
		using SharedLibraryPtr = void*;

		/**
		 * @brief OS independent function to open a shared library.
		 * 
		 * @param _path Null terminate string with path to the shared library.
		 * @return Owning shared library handle on success, null on error.
		*/
		[[nodiscard("owning pointer to shared library")]] SharedLibraryPtr open_shared_library(const char* _path);
		
		/**
		 * @brief OS independent function to open a shared library.
		 *
		 * @param _path Path to the shared library.
		 * @return Owning shared library handle on success, null on error.
		*/
		[[nodiscard("owning pointer to shared library")]] SharedLibraryPtr open_shared_library(const fs::path& _path);

		/**
		 * @brief OS independent function to close a shared library.
		 * 
		 * @param _lib Library to close. This will be set to null before returning.
		*/
		void close_shared_library(SharedLibraryPtr& _lib);

		/**
		 * @brief OS independent function to find a symbol in a shared library.
		 * 
		 * @param _lib Shared library handle, MUST NOT BE NULL
		 * @param _symbolName Null terminated string with the name of the symbol to load.
		 * @return The symbol's address or nullptr if not found.
		*/
		void* get_shared_library_symbol(const SharedLibraryPtr& _lib, const char* _symbolName);
	};



};
