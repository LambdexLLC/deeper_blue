#pragma once

#include "window.hpp"

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
			glfwWaitEventsTimeout(0.1);
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



		Window window_;
		std::vector<std::unique_ptr<IArtist>> artists_{};
	};


	bool init_graphics(GraphicsState& _state);
};