#pragma once

#include "gl.hpp"
#include "slot.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <ranges>
#include <algorithm>

/**
 * @brief Alias of glfw's opaque window struct
*/
using GLFWWindow = GLFWwindow;

/**
 * @brief Deleter function object type for glfw windows
*/
struct GLFWWindow_Deleter
{
	void operator()(GLFWWindow* _ptr)
	{
		glfwDestroyWindow(_ptr);
	};
};

/**
 * @brief RAII owning handle to a glfw window
*/
using GLFWWindowHandle = std::unique_ptr<GLFWWindow, GLFWWindow_Deleter>;

namespace lbx::chess_view
{
	// Forward decl for window size buffer
	class Window;

	/**
	 * @brief Underlying buffer object for things that need to stay in sync with the window size
	*/
	using WindowSizeBuffer = jc::slot<Window&, int, int>;

	/**
	 * @brief Listens for window size change events
	*/
	using WindowSizeListener = WindowSizeBuffer::listener_type;



	/**
	 * @brief Small interface for interacting with a window
	*/
	class Window
	{
	private:

		void on_framebuffer_resize(int _width, int _height);
		void on_window_refresh();

		void set_callbacks();
		void clear_callbacks();

	public:

		/**
		 * @brief Gets the underlying window pointer
		 * @return Pointer to a glfw window
		*/
		GLFWWindow* get() const& noexcept
		{
			return this->window_.get();
		};

		/**
		 * @brief Gets the underlying window pointer
		 * @return Pointer to a glfw window
		*/
		operator GLFWWindow* () const& noexcept
		{
			return this->get();
		};

		// Prevent accidental use after free
		GLFWWindow* get() && = delete;
		operator GLFWWindow* () && = delete;

		// Accessor for the size buffer object
		const auto& size_buffer() const
		{
			return this->size_buffer_;
		};




		Window() = default;

		Window(GLFWWindowHandle _window) :
			window_{ std::move(_window) }
		{
			this->set_callbacks();
		};
		Window& operator=(GLFWWindowHandle _window)
		{
			this->clear_callbacks();
			this->window_ = std::move(_window);
			this->set_callbacks();
			return *this;
		};

		~Window()
		{
			this->clear_callbacks();
		};

	private:
		
		GLFWWindowHandle window_;
		std::shared_ptr<WindowSizeBuffer> size_buffer_{ new WindowSizeBuffer{} };
	};


	/**
	 * @brief Magic listener tied to a window size buffer
	*/
	class WindowOrthoProjection : public WindowSizeListener
	{
	public:

		glm::mat4 matrix() const noexcept
		{
			return this->mat_;
		};

		void invoke(Window& _window, int _width, int _height) final
		{
			this->mat_ = glm::ortho(0.0f, static_cast<float>(_width), 0.0f, static_cast<float>(_height));
		};
		void invoke(Window& _window)
		{
			int _width, _height;
			glfwGetFramebufferSize(_window.get(), &_width, &_height);
			this->invoke(_window, _width, _height);
		};

		using WindowSizeListener::WindowSizeListener;

	private:
		glm::mat4 mat_{ 1.0f };
	};


};