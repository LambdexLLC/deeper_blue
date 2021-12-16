#pragma once
#ifndef CPP_FREETYPE_FACE_HPP
#define CPP_FREETYPE_FACE_HPP

/*
	Defines RAII wrapper for the FT_Face struct and some related utility
	functions
*/

#include "freetype.hpp"
#include "library.hpp"

#define _CPP_FREETYPE_FACE_

namespace ft
{
	/**
	 * @brief Raw freetype face struct pointer
	*/
	using face_ptr = ::FT_Face;

	/**
	 * @brief Owning wrapper around the freetype face struct, adding RAII
	 *
	 * This is considered null when good() returns false
	*/
	class face_handle
	{
	public:

		/**
		 * @brief The type for pointers to the held value
		*/
		using pointer = face_ptr;

		/**
		 * @brief The type for references to the held value
		*/
		using reference = decltype(*std::declval<pointer>());

		// Sanity checks for freetype type characteristics

		static_assert(std::is_pointer<pointer>::value, "");
		static_assert(std::is_reference<reference>::value, "");

		/**
		 * @brief Gets the underling face pointer
		 * @return Freetype face pointer
		*/
		constexpr pointer get() const& noexcept
		{
			return this->face_;
		};

		/**
		 * @brief Gets the underlying face pointer.
		 *
		 * same as face_handle::get().
		 *
		 * @return Freetype face pointer.
		*/
		constexpr pointer operator->() const& noexcept
		{
			return this->get();
		};

		/**
		 * @brief Gets the underlying face pointer.
		 * 
		 * May only be called on non-temporaries.
		 * Same as face_handle::get().
		 * 
		 * @return Held face pointer.
		*/
		constexpr operator pointer() const& noexcept
		{
			return this->get();
		};

		/**
		 * @brief Checks if the held pointer is valid or not.
		 * 
		 * @return True if not null, false if null.
		*/
		constexpr bool good() const noexcept
		{
			return this->get() != nullptr;
		};

		/**
		 * @brief Gets a reference to the underlying face.
		 *
		 * face_handle::good() must return true!
		 *
		 * same as "*face_handle::get()".
		 *
		 * @return Freetype face reference.
		*/
		constexpr reference operator*() const& noexcept
		{
			JCLIB_ASSERT(this->good());
			return *(this->get());
		};

		// Prevent accidental use-after-free

		pointer get() && = delete;
		operator pointer() && = delete;
		pointer operator->() && = delete;
		reference operator*() && = delete;


		/**
		 * @brief Checks if the held pointer is valid or not
		 *
		 * Same as face_handle::good()
		 *
		 * @return True if not null, false if null
		*/
		constexpr explicit operator bool() const noexcept
		{
			return this->good();
		};

		/**
		 * @brief Releases ownership of the held face
		*/
		constexpr void release() noexcept
		{
			this->face_ = nullptr;
		};

		/**
		 * @brief Releases ownership of the held face and returns it
		 * @return Owning pointer to the face
		*/
		constexpr JCLIB_NODISCARD("owning face handle") pointer extract() noexcept
		{
			const auto _out = this->get();
			this->release();
			return _out;
		};

		/**
		 * @brief Deletes and frees the held face
		*/
		void reset()
		{
			if (this->good())
			{
				// TODO: Error handling
				const auto _err = FT_Done_Face(this->face_);
				if (_err != 0)
				{
					JCLIB_ABORT();
				};
				this->release();
			};
		};

		/**
		 * @brief Null initialzes the face
		*/
		constexpr face_handle() noexcept = default;

		/**
		 * @brief Takes ownership of the given face
		*/
		constexpr explicit face_handle(pointer _face) noexcept :
			face_{ _face }
		{};

		// Ensure face is cleaned up and ownership gets transferred

		constexpr face_handle(face_handle&& other) noexcept :
			face_{ other.extract() }
		{};
		face_handle& operator=(face_handle&& other) noexcept
		{
			this->reset();
			this->face_ = other.extract();
			return *this;
		};

		~face_handle()
		{
			this->reset();
		};

	private:

		/**
		 * @brief The freetype face object managed by the object
		*/
		pointer face_ = nullptr;

		// Prevent copying

		face_handle(const face_handle&) = delete;
		face_handle& operator=(const face_handle&) = delete;
	};

	/**
	 * @brief Opens a font file and creates a font face.
	 *
	 * Assumes only one font face is defined by the file specified.
	 *
	 * This will never throw exceptions.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_new_face
	 * 
	 * @param _lib Library handle, MUST NOT BE NULL
	 * @param _filepath Path to the font to open
	 * @param _err Error output parameter, if an error occurs this is assigned.
	 *
	 * @return The loaded font face on success, or an error code on failure
	*/
	inline face_handle load_font_file(const library_ptr& _lib, const char* _filepath, error& _err) noexcept
	{
		FT_Face _face{};
		_err = error_code(FT_New_Face(_lib, _filepath, 0, &_face));

		// Sanity check that _face is null on error
#if	JCLIB_DEBUG_V
		if (_err) JCLIB_UNLIKELY
		{
			JCLIB_ASSERT(_face == nullptr);
		};
#endif

		return face_handle{ _face };
	};

	/**
	 * @brief Opens a font file and creates a font face.
	 *
	 * Assumes only one font face is defined by the file specified.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_new_face
	 * 
	 * @param _lib Library handle, MUST NOT BE NULL
	 * @param _filepath Path to the font to open
	 *
	 * @return The loaded font face on success, or an error code on failure
	 *
	 * @exception ft::exception Thrown if an error occured
	*/
	inline face_handle load_font_file(const library_ptr& _lib, const char* _filepath) JCLIB_NOEXCEPT
	{
		error _err{};
		auto _out = load_font_file(_lib, _filepath, _err);

		// Throw if an error occured
		if (_err || !_out) JCLIB_UNLIKELY
		{
			JCLIB_THROW(ft::exception(_err));
		};

		return _out;
	};

	/**
	 * @brief Sets the size for a font face directly in pixels.
	 * 
	 * One of _widthPx or _heightPx may be set to 0 to have that dimension be automatically determined.
	 * Does NOT throw exceptions.
	 * 
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_set_pixel_sizes
	 * 
	 * @param _face Face to set size for.
	 * @param _widthPx Glyph width in pixels.
	 * @param _heightPx Glyph height in pixels.
	 * 
	 * @param _err Output parameter for getting errors.
	 * This will be set to an error if one occurs.
	*/
	inline void set_pixel_sizes(face_ptr _face, FT_UInt _widthPx, FT_UInt _heightPx, error& _err) noexcept
	{
		JCLIB_ASSERT(_face);
		_err = error_code(FT_Set_Pixel_Sizes(_face, _widthPx, _heightPx));
	};

	/**
	 * @brief Sets the size for a font face directly in pixels.
	 *
	 * One of _widthPx or _heightPx may be set to 0 to have that dimension be automatically determined.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_set_pixel_sizes
	 * 
	 * @param _face Face to set size for.
	 * @param _widthPx Glyph width in pixels.
	 * @param _heightPx Glyph height in pixels.
	 *
	 * @exception Thrown if an error occurs.
	*/
	inline void set_pixel_sizes(face_ptr _face, FT_UInt _widthPx, FT_UInt _heightPx) JCLIB_NOEXCEPT
	{
		error _err{};
		set_pixel_sizes(_face, _widthPx, _heightPx, _err);
		
		// Throw if an error occured
		if (_err) JCLIB_UNLIKELY
		{
			JCLIB_THROW(exception(_err));
		};
	};

	/**
	 * @brief Gets the index of the glyph for a unicode character.
	 * 
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_get_char_index
	 * 
	 * @param _face Font face to get glyph from.
	 * @param _codepoint Unicode character codepoint.
	 * 
	 * @return Glyph index on success, or 0 (zero) on glyph not found.
	*/
	inline FT_UInt get_char_index(face_ptr _face, FT_ULong _codepoint)
	{
		return FT_Get_Char_Index(_face, _codepoint);
	};

	/**
	 * @brief Loads a glyph for a font face.
	 *
	 * Does NOT throw exceptions.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_glyph
	 *
	 * @param _face Font face to load glyph for.
	 * @param _glyphIndex Index of the glyph to load
	 * @param _loadFlags Flags used when loading the glyph.
	 * @param _err Output variable for getting the error that occured if one occures.
	 *
	 * @return True if no error occured, false if an error occured.
	*/
	inline bool load_glyph(face_ptr _face, FT_UInt _glyphIndex, FT_Int32 _loadFlags, error& _err) noexcept
	{
		_err = error_code(FT_Load_Glyph(_face, _glyphIndex, _loadFlags));
		return !_err.is_error();
	};

	/**
	 * @brief Loads a glyph for a font face.
	 *
	 * Uses default flags when loading the glyph through freetype.
	 * Does NOT throw exceptions.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_glyph
	 *
	 * @param _face Font face to load glyph for.
	 * @param _glyphIndex Index of the glyph to load
	 * @param _err Output variable for getting the error that occured if one occures.
	 *
	 * @return True if no error occured, false if an error occured.
	*/
	inline bool load_glyph(face_ptr _face, FT_UInt _glyphIndex, error& _err) noexcept
	{
		return load_glyph(_face, _glyphIndex, FT_LOAD_DEFAULT, _err);
	};

	/**
	 * @brief Loads a glyph for a font face.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_glyph
	 *
	 * @param _face Font face to load glyph for.
	 * @param _glyphIndex Index of the glyph to load
	 * @param _loadFlags Flags used when loading the glyph.
	 * @return True if no error occured, false if an error occured.
	 *
	 * @exception ft::exception Thrown if an error occurs.
	*/
	inline bool load_glyph(face_ptr _face, FT_UInt _glyphIndex, FT_Int32 _loadFlags) JCLIB_NOEXCEPT
	{
		error _err{};
		const auto _out = load_glyph(_face, _glyphIndex, _loadFlags, _err);

		// Throw if an error occured
		if (_err || !_out) JCLIB_UNLIKELY
		{
			JCLIB_THROW(exception(_err));
		};

		return _out;
	};

	/**
	 * @brief Loads a glyph for a font face.
	 *
	 * Uses the default flags when loading the glyph through freetype.
	 *
	 * See https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_glyph
	 *
	 * @param _face Font face to load glyph for.
	 * @param _glyphIndex Index of the glyph to load
	 * @return True if no error occured, false if an error occured.
	 *
	 * @exception ft::exception Thrown if an error occurs.
	*/
	inline bool load_glyph(face_ptr _face, FT_UInt _glyphIndex) JCLIB_NOEXCEPT
	{
		error _err{};
		const auto _out = load_glyph(_face, _glyphIndex, _err);

		// Throw if an error occured
		if (_err || !_out) JCLIB_UNLIKELY
		{
			JCLIB_THROW(exception(_err));
		};

		return _out;
	};

};

#endif // CPP_FREETYPE_FACE_HPP