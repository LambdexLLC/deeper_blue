#pragma once
#ifndef CPP_FREETYPE_ERROR_HPP
#define CPP_FREETYPE_ERROR_HPP

/*
	Defines error related types and functions.
	Also provides an exception type for freetype errors.
*/

#include "freetype.hpp"

#include <jclib/type.h>
#include <jclib/config.h>

#include <exception>

#define _CPP_FREETYPE_ERROR_

namespace ft
{
	/**
	 * @brief Raw error code value type for freetype errors.
	*/
	using error_code_t = FT_Error;

	/**
	 * @brief Named free type error codes.
	*/
	enum class error_code : error_code_t
	{
		ok = FT_Err_Ok,

		/* generic errors */

		cannot_open_resource = FT_Err_Cannot_Open_Resource,
		unknown_file_format = FT_Err_Unknown_File_Format,
		invalid_file_format = FT_Err_Invalid_File_Format,
		invalid_version = FT_Err_Invalid_Version,
		lower_module_version = FT_Err_Lower_Module_Version,
		invalid_argument = FT_Err_Invalid_Argument,
		unimplemented_feature = FT_Err_Unimplemented_Feature,
		invalid_table = FT_Err_Invalid_Table,
		invalid_offset = FT_Err_Invalid_Offset,
		array_too_large = FT_Err_Array_Too_Large,
		missing_module = FT_Err_Missing_Module,
		missing_property = FT_Err_Missing_Property,

		/* glyph/character errors */

		invalid_glyph_index = FT_Err_Invalid_Glyph_Index,
		invalid_character_code = FT_Err_Invalid_Character_Code,
		invalid_glyph_format = FT_Err_Invalid_Glyph_Format,
		cannot_render_glyph = FT_Err_Cannot_Render_Glyph,
		invalid_outline = FT_Err_Invalid_Outline,
		invalid_composite = FT_Err_Invalid_Composite,
		too_many_hints = FT_Err_Too_Many_Hints,
		invalid_pixel_size = FT_Err_Invalid_Pixel_Size,

		/* handle errors */

		invalid_handle = FT_Err_Invalid_Handle,
		invalid_library_handle = FT_Err_Invalid_Library_Handle,
		invalid_driver_handle = FT_Err_Invalid_Driver_Handle,
		invalid_face_handle = FT_Err_Invalid_Face_Handle,
		invalid_size_handle = FT_Err_Invalid_Size_Handle,
		invalid_slot_handle = FT_Err_Invalid_Slot_Handle,
		invalid_charmap_handle = FT_Err_Invalid_CharMap_Handle,
		invalid_cache_handle = FT_Err_Invalid_Cache_Handle,
		invalid_stream_handle = FT_Err_Invalid_Stream_Handle,

		/* driver errors */

		too_many_drivers = FT_Err_Too_Many_Drivers,
		too_many_extensions = FT_Err_Too_Many_Extensions,

		/* memory errors */

		out_of_memory = FT_Err_Out_Of_Memory,
		unlisted_object = FT_Err_Unlisted_Object,

		/* stream errors */

		cannot_open_stream = FT_Err_Cannot_Open_Stream,
		invalid_stream_seek = FT_Err_Invalid_Stream_Seek,
		invalid_stream_skip = FT_Err_Invalid_Stream_Skip,
		invalid_stream_read = FT_Err_Invalid_Stream_Read,
		invalid_stream_operation = FT_Err_Invalid_Stream_Operation,
		invalid_frame_operation = FT_Err_Invalid_Frame_Operation,
		nested_frame_access = FT_Err_Nested_Frame_Access,
		invalid_frame_read = FT_Err_Invalid_Frame_Read,

		/* raster errors */

		raster_uninitialized = FT_Err_Raster_Uninitialized,
		raster_corrupted = FT_Err_Raster_Corrupted,
		raster_overflow = FT_Err_Raster_Overflow,
		raster_negative_height = FT_Err_Raster_Negative_Height,

		/* cache errors */

		too_many_caches = FT_Err_Too_Many_Caches,

		/* TrueType and SFNT errors */

		invalid_opcode = FT_Err_Invalid_Opcode,
		too_few_arguments = FT_Err_Too_Few_Arguments,
		stack_overflow = FT_Err_Stack_Overflow,
		code_overflow = FT_Err_Code_Overflow,
		bad_argument = FT_Err_Bad_Argument,
		divide_by_zero = FT_Err_Divide_By_Zero,
		invalid_reference = FT_Err_Invalid_Reference,
		debug_opcode = FT_Err_Debug_OpCode,
		endf_in_exec_stream = FT_Err_ENDF_In_Exec_Stream,
		nested_defs = FT_Err_Nested_DEFS,
		invalid_coderange = FT_Err_Invalid_CodeRange,
		execution_too_long = FT_Err_Execution_Too_Long,
		too_many_function_defs = FT_Err_Too_Many_Function_Defs,
		too_many_instruction_defs = FT_Err_Too_Many_Instruction_Defs,
		table_missing = FT_Err_Table_Missing,
		horiz_header_missing = FT_Err_Horiz_Header_Missing,
		locations_missing = FT_Err_Locations_Missing,
		name_table_missing = FT_Err_Name_Table_Missing,
		cmap_table_missing = FT_Err_CMap_Table_Missing,
		hmtx_table_missing = FT_Err_Hmtx_Table_Missing,
		post_table_missing = FT_Err_Post_Table_Missing,
		invalid_horiz_metrics = FT_Err_Invalid_Horiz_Metrics,
		invalid_charmap_format = FT_Err_Invalid_CharMap_Format,
		invalid_ppem = FT_Err_Invalid_PPem,
		invalid_vert_metrics = FT_Err_Invalid_Vert_Metrics,
		could_not_find_context = FT_Err_Could_Not_Find_Context,
		invalid_post_table_format = FT_Err_Invalid_Post_Table_Format,
		invalid_post_table = FT_Err_Invalid_Post_Table,
		def_in_glyf_bytecode = FT_Err_DEF_In_Glyf_Bytecode,
		missing_bitmap = FT_Err_Missing_Bitmap,

		/* CFF, CID, and Type 1 errors */

		syntax_error = FT_Err_Syntax_Error,
		stack_underflow = FT_Err_Stack_Underflow,
		ignore = FT_Err_Ignore,
		no_unicode_glyph_name = FT_Err_No_Unicode_Glyph_Name,
		glyph_too_big = FT_Err_Glyph_Too_Big,

		/* BDF errors */

		missing_startfont_field = FT_Err_Missing_Startfont_Field,
		missing_font_field = FT_Err_Missing_Font_Field,
		missing_size_field = FT_Err_Missing_Size_Field,
		missing_fontboundingbox_field = FT_Err_Missing_Fontboundingbox_Field,
		missing_chars_field = FT_Err_Missing_Chars_Field,
		missing_startchar_field = FT_Err_Missing_Startchar_Field,
		missing_encoding_field = FT_Err_Missing_Encoding_Field,
		missing_bbx_field = FT_Err_Missing_Bbx_Field,
		bbx_too_big = FT_Err_Bbx_Too_Big,
		corrupted_font_header = FT_Err_Corrupted_Font_Header,
		corrupted_font_glyphs = FT_Err_Corrupted_Font_Glyphs,
	};

	/**
	 * @brief Gets the error text string for a freetype error code.
	 * 
	 * This will abort execution if not in debug mode when _err does not
	 * have a string defined (in the switch statement below).
	 * 
	 * @param _err The error code to get the text for.
	 * 
	 * @return The error text for an error code.
	 */
	constexpr inline const char* get_error_text(error_code _err) noexcept
	{
		switch (_err)
		{
		case error_code::ok:
			return "ok";

		/* generic errors */

		case error_code::cannot_open_resource:
			return "cannot open resource";
		case error_code::unknown_file_format:
			return "unknown file format";
		case error_code::invalid_file_format:
			return "broken file";
		case error_code::invalid_version:
			return "invalid FreeType version";
		case error_code::lower_module_version:
			return "module version is too low";
		case error_code::invalid_argument:
			return "invalid argument";
		case error_code::unimplemented_feature:
			return "unimplemented feature";
		case error_code::invalid_table:
			return "broken table";
		case error_code::invalid_offset:
			return "broken offset within table";
		case error_code::array_too_large:
			return "array allocation size too large";
		case error_code::missing_module:
			return "missing module";
		case error_code::missing_property:
			return "missing property";

		/* glyph/character errors */

		case error_code::invalid_handle:
			return "invalid object handle";
		case error_code::invalid_library_handle:
			return "invalid library handle";
		case error_code::invalid_driver_handle:
			return "invalid module handle";
		case error_code::invalid_face_handle:
			return "invalid face handle";
		case error_code::invalid_size_handle:
			return "invalid size handle";
		case error_code::invalid_slot_handle:
			return "invalid glyph slot handle";
		case error_code::invalid_charmap_handle:
			return "invalid charmap handle";
		case error_code::invalid_cache_handle:
			return "invalid cache manager handle";
		case error_code::invalid_stream_handle:
			return "invalid stream handle";

		/* handle errors */

		case error_code::invalid_glyph_index:
			return "invalid glyph index";
		case error_code::invalid_character_code:
			return "invalid character code";
		case error_code::invalid_glyph_format:
			return "unsupported glyph image format";
		case error_code::cannot_render_glyph:
			return "cannot render this glyph format";
		case error_code::invalid_outline:
			return "invalid outline";
		case error_code::invalid_composite:
			return "invalid composite glyph";
		case error_code::too_many_hints:
			return "too many hints";
		case error_code::invalid_pixel_size:
			return "invalid pixel size";

		/* driver errors */

		case error_code::too_many_drivers:
			return "too many modules";
		case error_code::too_many_extensions:
			return "too many extensions";

		/* memory errors */

		case error_code::out_of_memory:
			return "out of memory";
		case error_code::unlisted_object:
			return "unlisted object";

		/* stream errors */

		case error_code::cannot_open_stream:
			return "cannot open stream";
		case error_code::invalid_stream_seek:
			return "invalid stream seek";
		case error_code::invalid_stream_skip:
			return "invalid stream skip";
		case error_code::invalid_stream_read:
			return "invalid stream read";
		case error_code::invalid_stream_operation:
			return "invalid stream operation";
		case error_code::invalid_frame_operation:
			return "invalid frame operation";
		case error_code::nested_frame_access:
			return "nested frame access";
		case error_code::invalid_frame_read:
			return "invalid frame read";

		/* raster errors */

		case error_code::raster_uninitialized:
			return "raster uninitialized";
		case error_code::raster_corrupted:
			return "raster corrupted";
		case error_code::raster_overflow:
			return "raster overflow";
		case error_code::raster_negative_height:
			return "negative height while rastering";

		/* cache errors */

		case error_code::too_many_caches:
			return "too many registered caches";

		/* TrueType and SFNT errors */

		case error_code::invalid_opcode:
			return "invalid opcode";
		case error_code::too_few_arguments:
			return "too few arguments";
		case error_code::stack_overflow:
			return "stack overflow";
		case error_code::code_overflow:
			return "code overflow";
		case error_code::bad_argument:
			return "bad argument";
		case error_code::divide_by_zero:
			return "division by zero";
		case error_code::invalid_reference:
			return "invalid reference";
		case error_code::debug_opcode:
			return "found debug opcode";
		case error_code::endf_in_exec_stream:
			return "found ENDF opcode in execution stream";
		case error_code::nested_defs:
			return "nested DEFS";
		case error_code::invalid_coderange:
			return "invalid code range";
		case error_code::execution_too_long:
			return "execution context too long";
		case error_code::too_many_function_defs:
			return "too many function definitions";
		case error_code::too_many_instruction_defs:
			return "too many instruction definitions";
		case error_code::table_missing:
			return "SFNT font table missing";
		case error_code::horiz_header_missing:
			return "horizontal header (hhea) table missing";
		case error_code::locations_missing:
			return "locations (loca) table missing";
		case error_code::name_table_missing:
			return "name table missing";
		case error_code::cmap_table_missing:
			return "character map (cmap) table missing";
		case error_code::hmtx_table_missing:
			return "horizontal metrics (hmtx) table missing";
		case error_code::post_table_missing:
			return "PostScript (post) table missing";
		case error_code::invalid_horiz_metrics:
			return "invalid horizontal metrics";
		case error_code::invalid_charmap_format:
			return "invalid character map (cmap) format";
		case error_code::invalid_ppem:
			return "invalid ppem value";
		case error_code::invalid_vert_metrics:
			return "invalid vertical metrics";
		case error_code::could_not_find_context:
			return "could not find context";
		case error_code::invalid_post_table_format:
			return "invalid PostScript (post) table format";
		case error_code::invalid_post_table:
			return "invalid PostScript (post) table";
		case error_code::def_in_glyf_bytecode:
			return "found FDEF or IDEF opcode in glyf bytecode";
		case error_code::missing_bitmap:
			return "missing bitmap in strike";

		/* CFF, CID, and Type 1 errors */

		case error_code::syntax_error:
			return "opcode syntax error";
		case error_code::stack_underflow:
			return "argument stack underflow";
		case error_code::ignore:
			return "ignore";
		case error_code::no_unicode_glyph_name:
			return "no Unicode glyph name found";
		case error_code::glyph_too_big:
			return "glyph too big for hinting";

		/* BDF errors */

		case error_code::missing_startfont_field:
			return "`STARTFONT' field missing";
		case error_code::missing_font_field:
			return "`FONT' field missing";
		case error_code::missing_size_field:
			return "`SIZE' field missing";
		case error_code::missing_fontboundingbox_field:
			return "`FONTBOUNDINGBOX' field missing";
		case error_code::missing_chars_field:
			return "`CHARS' field missing";
		case error_code::missing_startchar_field:
			return "`STARTCHAR' field missing";
		case error_code::missing_encoding_field:
			return "`ENCODING' field missing";
		case error_code::missing_bbx_field:
			return "`BBX' field missing";
		case error_code::bbx_too_big:
			return "`BBX' too big";
		case error_code::corrupted_font_header:
			return "Font header corrupted or missing fields";
		case error_code::corrupted_font_glyphs:
			return "Font glyphs corrupted or missing fields";
		default:
			JCLIB_ASSERT(false);
			return "no error text specified";
		};
	};

	/**
	 * @brief Type for holding a freetype error.
	*/
	class error
	{
	public:

		/**
		 * @brief Gets the error code for this error.
		 * @return Error code value.
		*/
		constexpr error_code code() const noexcept
		{
			return this->code_;
		};

		/**
		 * @brief Checks if this is actually an error.
		 * 
		 * If error::code() returns error_code::ok, then this is not actually an error.
		 * This should return true for a default constructed error object.
		 * 
		 * @return True if error, false otherwise.
		*/
		constexpr bool is_error() const noexcept
		{
			return this->code() != error_code::ok;
		};

		/**
		 * @brief Checks if this is actually an error.
		 *
		 * If error::code() returns error_code::ok, then this is not actually an error.
		 * This should return true for a default constructed error object.
		 *
		 * @return True if error, false otherwise.
		*/
		constexpr explicit operator bool() const noexcept
		{
			return this->is_error();
		};

		/**
		 * @brief Gets the held error code.
		 * 
		 * This is the same as error::code() and is defined to allow this type to be
		 * used in a natural manner with switch statements.
		 * 
		 * @return Error code value.
		*/
		constexpr operator error_code() const noexcept
		{
			return this->code();
		};

		/**
		 * @brief Gets the error text associated with this error.
		 * 
		 * This is equivalent to calling get_error_text() using the value returned
		 * by called error::code();
		 * 
		 * @return Error text string
		*/
		constexpr const char* what() const noexcept
		{
			return get_error_text(this->code());
		};


		// Comparison operators just compare the error codes

		friend constexpr inline bool operator==(const error& lhs, const error& rhs) noexcept
		{
			return lhs.code() == rhs.code();
		};
		friend constexpr inline bool operator!=(const error& lhs, const error& rhs) noexcept
		{
			return lhs.code() != rhs.code();
		};

		friend constexpr inline bool operator==(const error& lhs, const error_code& rhs) noexcept
		{
			return lhs.code() == rhs;
		};
		friend constexpr inline bool operator==(const error_code& lhs, const error& rhs) noexcept
		{
			return rhs == lhs;
		};
		
		friend constexpr inline bool operator!=(const error& lhs, const error_code& rhs) noexcept
		{
			return lhs.code() != rhs;
		};
		friend constexpr inline bool operator!=(const error_code& lhs, const error& rhs) noexcept
		{
			return rhs != lhs;
		};


		/**
		 * @brief Constructs the error with an error code.
		 *
		 * This is no explicit to allow more natural usage.
		 *
		 * @param _code Error code for this error.
		*/
		constexpr error(error_code _code) noexcept :
			code_{ _code }
		{};

		/**
		 * @brief Assigns the error code for this error.
		 * 
		 * @param _code Error code to assign.
		 * @return This error object.
		*/
		constexpr error& operator=(error_code _code) noexcept
		{
			this->code_ = _code;
			return *this;
		};

		/**
		 * @brief Null construction for the error.
		 * 
		 * error::is_error() will return false for a default constructed Error.
		 * 
		 * @param _nt Null type value tag.
		*/
		constexpr error(jc::null_t _nt) noexcept :
			error{ error_code::ok }
		{};

		/**
		 * @brief Null assigns the error.
		 * 
		 * error::is_error() will return false after invoking this assignment.
		 * 
		 * This is the same as calling error::operator=(error_code::ok).
		 * 
		 * @param _nt Null type value tag.
		 * @return This error object.
		*/
		constexpr error& operator=(jc::null_t _nt) noexcept
		{
			return *this = error_code::ok;
		};

		/**
		 * @brief Null construction for the error.
		 *
		 * error::is_error() will return false for a default constructed Error.
		*/
		constexpr error() noexcept :
			error{ jc::null }
		{};

	private:

		/**
		 * @brief The error code for this error.
		*/
		error_code code_;

	};

	/**
	 * @brief Exception type for freetype exceptions.
	 * 
	 * This is implemented as a mirror of the error type but inherits from
	 * std::exception to allow more typical usage.
	*/
	class exception :
		public std::exception,  // multiple inheritance, sue me
		public error
	{
	public:

		/**
		 * @brief Gets the error text associated with the freetype error.
		 * 
		 * Same as error::what().
		 * 
		 * @return Error text string.
		*/
		const char* what() const override
		{
			return error::what();
		};


		// Allow construction and assignment from error type

		exception(error _err) noexcept :
			error{ _err },
			std::exception{}
		{};
		exception& operator=(error _err) noexcept
		{
			error::operator=(_err);
			return *this;
		};

		/**
		 * @brief Constructs the error with an error code.
		 *
		 * This is no explicit to allow more natural usage.
		 *
		 * @param _code Error code for this error.
		*/
		exception(error_code _code) noexcept :
			error{ _code },
			std::exception{}
		{};

		/**
		 * @brief Assigns the error code for this error.
		 *
		 * @param _code Error code to assign.
		 * @return This error object.
		*/
		exception& operator=(error_code _code) noexcept
		{
			error::operator=(_code);
			return *this;
		};

		/**
		 * @brief Null construction for the error.
		 *
		 * error::is_error() will return false for a default constructed Error.
		 *
		 * @param _nt Null type value tag.
		*/
		exception(jc::null_t _nt) noexcept :
			exception{ error_code::ok }
		{};

		/**
		 * @brief Null assigns the error.
		 *
		 * error::is_error() will return false after invoking this assignment.
		 *
		 * This is the same as calling error::operator=(error_code::ok).
		 *
		 * @param _nt Null type value tag.
		 * @return This error object.
		*/
		exception& operator=(jc::null_t _nt) noexcept
		{
			return *this = error_code::ok;
		};

		/**
		 * @brief Null construction for the error.
		 *
		 * error::is_error() will return false for a default constructed Error.
		*/
		exception() noexcept :
			exception{ jc::null }
		{};
	};

};

#endif // CPP_FREETYPE_ERROR_HPP