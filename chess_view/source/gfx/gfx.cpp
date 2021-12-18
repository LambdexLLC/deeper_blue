#include "gfx.hpp"

namespace lbx::chess_view
{
	inline void APIENTRY test_debug_callback
	(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
	)
	{
		std::cout << std::string_view{ message, (size_t)length } << '\n';

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			break;
		case GL_DEBUG_SEVERITY_LOW:
			break;
		case GL_DEBUG_SEVERITY_MEDIUM: [[fallthrough]];
		case GL_DEBUG_SEVERITY_HIGH:
			__debugbreak();
			break;
		default:
			JCLIB_ABORT();
			break;
		};
	};

	bool init_graphics(GraphicsState& _state)
	{
		if (!glfwInit()) { return false; };

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		_state.window_ = GLFWWindowHandle{ glfwCreateWindow(800, 600, "ChessView", nullptr, nullptr) };
		glfwMakeContextCurrent(_state.window_);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return false; };


		gl::enable_debug_output_synchronous();
		gl::set_debug_callback(test_debug_callback, nullptr);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return true;
	};
}
