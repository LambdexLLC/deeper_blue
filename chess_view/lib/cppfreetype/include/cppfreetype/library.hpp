#pragma once
#ifndef CPP_FREETYPE_LIBRARY_HPP
#define CPP_FREETYPE_LIBRARY_HPP

/*
	Defines the freetype library owning handle and functions for initalization
*/

#include "freetype.hpp"
#include "error.hpp"

#include <jclib/type.h>
#include <jclib/config.h>

#define _CPP_FREETYPE_LIBRARY_

namespace ft
{
	/**
	 * @brief Raw freetype library struct pointer
	*/
	using library_ptr = ::FT_Library;

	/**
	 * @brief Wrapper around the freetype library struct, adding RAII
	 *
	 * This is considered null when good() returns false
	*/
	class library_handle
	{
	public:

		/**
		 * @brief The held type
		*/
		using pointer = library_ptr;

		/**
		 * @brief Gets the underling library pointer
		 * @return Freetype library pointer
		*/
		constexpr pointer get() const& noexcept
		{
			return this->lib_;
		};

		// Prevent accidental use after free
		pointer get() && = delete;

		/**
		 * @brief Checks if the held pointer is valid or not
		 * @return True if not null, false if null
		*/
		constexpr bool good() const noexcept
		{
			return this->get() != nullptr;
		};

		/**
		 * @brief Checks if the held pointer is valid or not
		 *
		 * Same as good()
		 *
		 * @return True if not null, false if null
		*/
		constexpr explicit operator bool() const noexcept
		{
			return this->good();
		};

		/**
		 * @brief Releases ownership of the held library
		*/
		constexpr void release() noexcept
		{
			this->lib_ = nullptr;
		};

		/**
		 * @brief Releases ownership of the held library and returns it
		 * @return Owning pointer to the library
		*/
		constexpr JCLIB_NODISCARD("owning library handle") FT_Library extract() noexcept
		{
			const auto _out = this->get();
			this->release();
			return _out;
		};

		/**
		 * @brief Deletes and frees the held library
		*/
		void reset()
		{
			if (this->good())
			{
				// TODO: Error handling
				const auto _err = FT_Done_FreeType(this->lib_);
				if (_err != 0)
				{
					JCLIB_ABORT();
				};
				this->release();
			};
		};

		/**
		 * @brief Null initialzes the library
		*/
		constexpr library_handle() noexcept = default;

		/**
		 * @brief Takes ownership of the given library
		*/
		constexpr explicit library_handle(pointer _lib) noexcept :
			lib_{ _lib }
		{};

		// Ensure library is cleaned up and ownership gets transferred

		constexpr library_handle(library_handle&& other) noexcept :
			lib_{ other.extract() }
		{};
		library_handle& operator=(library_handle&& other) noexcept
		{
			this->reset();
			this->lib_ = other.extract();
			return *this;
		};

		~library_handle()
		{
			this->reset();
		};

	private:

		/**
		 * @brief The freetype library object managed by the object
		*/
		pointer lib_ = nullptr;

		// Prevent copying

		library_handle(const library_handle&) = delete;
		library_handle& operator=(const library_handle&) = delete;
	};

	/**
	 * @brief Initializes the freetype library.
	 *
	 * This will never throw exceptions.
	 *
	 * @param _err Error output parameter, if an error occurs this is assigned.
	 *
	 * @return The freetype library handle, may be null if an error occured.
	*/
	JCLIB_NODISCARD("owning library handle") inline library_handle new_library(error& _err) noexcept
	{
		library_handle::pointer _lib{};
		_err = error_code(FT_Init_FreeType(&_lib));

		// Sanity check that _lib is null on error
#if	JCLIB_DEBUG_V
		if (_err) JCLIB_UNLIKELY
		{
			JCLIB_ASSERT(_lib == nullptr);
		};
#endif

		return library_handle{ _lib };
	};

	/**
	 * @brief Initializes the freetype library.
	 *
	 * @return The freetype library handle, never null
	 *
	 * @exception ft::exception Thrown if an error occurs
	*/
	JCLIB_NODISCARD("owning library handle") inline library_handle new_library() JCLIB_NOEXCEPT
	{
		error _err{};
		auto _out = new_library(_err);

		// Check for errors and throw if there was one
		if (_err || !_out) JCLIB_UNLIKELY
		{
			JCLIB_THROW(ft::exception(_err));
		};

		return _out;
	};

};

#endif // CPP_FREETYPE_LIBRARY_HPP