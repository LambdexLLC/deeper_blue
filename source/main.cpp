#include "api_glue.hpp"

#include "application/application.hpp"

#include "cpu_profiling.hpp"

int main(int _nargs, char* _vargs[])
{
	using namespace lbx;

	// Echo arguements for now
	for (int n = 0; n != _nargs; ++n)
	{
		println("arg {} = {}", n, _vargs[n]);
	};

	// Path to this executable
	const fs::path _executablePath = _vargs[0];

	// Path to the executables folder
	const auto _rootPath = _executablePath.parent_path();

	// Set the application's root path
	chess::set_application_root_path(_rootPath);


	const auto fmtd = [](const std::chrono::duration<double> _duration)
	{
		double _val = _duration.count();
		using namespace std::chrono;
		return format("{}", duration_cast<nanoseconds>(_duration));
	};

	{
		using namespace chess;

		const auto _board = BoardWithState{ make_standard_board() };
		const auto _move = Move{ (File::a, Rank::r2), (File::a, Rank::r4) };
		const auto _fn = [_board, _move]()
		{
			return is_move_valid(_board, _move, _board.turn);
		};
		auto _result = profile_cpu_usage(10000000, _fn);
		println("times = {}\nmax = {}\navg = {}", _result.runtimes, _result.max_time, fmtd(_result.average_time));
	};


	AccountAPI _accountAPI{};
	api::set_account_api(&_accountAPI);

	while (true)
	{
		api::forward_events();
		jc::sleep(0.1f);
	};

	return 0;
};
