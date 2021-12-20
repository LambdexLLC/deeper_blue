#pragma once

#include "element.hpp"

#include <jclib/memory.h>

#include <vector>

namespace lbx::chess_view
{
	/**
	 * @brief Scene interface for grouping graphics elements into logical units.
	*/
	class Scene
	{
	public:

		/**
		 * @brief Adds an element to the scene.
		 * @param _element Element to add to the scene. Must not be null!
		*/
		void insert(std::unique_ptr<IElement> _element);

		/**
		 * @brief Shows all elements.
		*/
		void show();

		/**
		 * @brief Hides all elements.
		*/
		void hide();



		Scene() = default;

		~Scene();

	private:

		/**
		 * @brief Storage for the graphics elements in the scene.
		*/
		std::vector<std::unique_ptr<IElement>> elements_;

	};
};
