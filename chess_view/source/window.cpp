#include "window.hpp"

namespace lbx::chess_view
{
	void Window::on_framebuffer_resize(int _width, int _height)
	{
		glViewport(0, 0, _width, _height);
		this->size_buffer_->invoke(*this, _width, _height);
	};
	void Window::on_window_refresh()
	{
		int _width, _height;
		glfwGetFramebufferSize(this->window_.get(), &_width, &_height);
		this->on_framebuffer_resize(_width, _height);
	};


	/**
	 * @brief Gets the user data pointer for a window
	 * @param _window Window to get pointer from
	 * @return chess_view Window pointer
	*/
	inline Window* get_userdata(GLFWwindow* _window)
	{
		return static_cast<Window*>(glfwGetWindowUserPointer(_window));
	};

	void Window::set_callbacks()
	{
		const auto _window = this->window_.get();
		if (!_window)
		{
			return;
		};

		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* _window, int w, int h)
			{
				auto _userdata = get_userdata(_window);
				_userdata->on_framebuffer_resize(w, h);
			});
		glfwSetWindowRefreshCallback(_window, [](GLFWwindow* _window)
			{
				auto _userdata = get_userdata(_window);
				_userdata->on_window_refresh();
			});

	};
	void Window::clear_callbacks()
	{
		const auto _window = this->window_.get();
		if (!_window)
		{
			return;
		};

		glfwSetWindowUserPointer(_window, nullptr);
		glfwSetFramebufferSizeCallback(_window, nullptr);
		glfwSetWindowRefreshCallback(_window, nullptr);
	};

};

