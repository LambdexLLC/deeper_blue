#pragma once

namespace lbx::chess_view
{
	/**
	 * @brief Base interface for graphical elements
	*/
	class IElement
	{
	public:

		/**
		 * @brief Sets the element to be displayed.
		*/
		virtual void show() {};

		/**
		 * @brief Sets the element to be hidden.
		*/
		virtual void hide() {};

		/**
		 * @brief Clears any references between this and other elements.
		*/
		virtual void destroy_references() {};


		// Polymorphic destruction baby
		virtual ~IElement() = default;
	};
};
