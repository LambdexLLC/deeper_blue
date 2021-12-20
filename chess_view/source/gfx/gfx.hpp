#pragma once

#include "window.hpp"
#include "resources.hpp"

#include <jclib/memory.h>

#include <vector>

namespace lbx::chess_view
{
	class GraphicsState;

	/**
	 * @brief Interface for something that can be drawn on the screen
	*/
	class IArtist
	{
	public:

		/**
		 * @brief Draws the artist's data onto the screen
		*/
		virtual void draw(GraphicsState& _state) = 0;

		virtual ~IArtist() = default;
	};

	/**
	 * @brief Application wide graphics state
	*/
	class GraphicsState
	{
	public:

		bool keep_running() const
		{
			return this->window_ && !glfwWindowShouldClose(this->window_.get());
		};

		void clear()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		};

		void draw()
		{
			for (auto& _artist : this->artists_)
			{
				_artist->draw(*this);
			};
		};

		void swap_buffers()
		{
			glfwSwapBuffers(this->window_.get());
		};

		void pull_events()
		{
			glfwWaitEventsTimeout(0.01);
		};

		void update()
		{
			this->clear();
			this->draw();
			this->swap_buffers();
			this->pull_events();
		};

		void insert_artist(std::unique_ptr<IArtist> _artist)
		{
			JCLIB_ASSERT(_artist);
			this->artists_.push_back(std::move(_artist));
		};


		/**
		 * @brief Accessor method for graphics resources.
		 * @return Graphics resources object.
		*/
		GFXResources& resources() noexcept
		{
			return this->resources_;
		};

		/**
		 * @brief Accessor method for graphics resources.
		 * @return Graphics resources object.
		*/
		const GFXResources& resources() const noexcept
		{
			return this->resources_;
		};




		Window window_;
		std::vector<std::unique_ptr<IArtist>> artists_{};

	private:
		
		GFXResources resources_{};

	};


	bool init_graphics(GraphicsState& _state);
};