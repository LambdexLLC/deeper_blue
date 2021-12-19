#pragma once

#include <jclib/timer.h>
#include <jclib/algorithm.h>

#include <chrono>
#include <vector>

namespace lbx
{
	struct CPUProfileResult
	{
		using duration = std::chrono::duration<double>;

		size_t runtimes;
		duration average_time;
		duration max_time;
	};
	
	template <typename OpT, typename... ArgTs>
	JCLIB_REQUIRES((jc::cx_invocable<OpT, const ArgTs&...> && !jc::cx_same_as<jc::invoke_result_t<OpT, const ArgTs&...>, void>))
	inline CPUProfileResult profile_cpu_usage(size_t _times, OpT&& _op, const ArgTs&... _args)
	{
		JCLIB_ASSERT(_times != 0);

		static thread_local volatile jc::invoke_result_t<OpT, const ArgTs&...> _resultRedirect{};

		using clock = std::chrono::steady_clock;
		using duration = CPUProfileResult::duration;

		std::vector<duration> _durations(_times, clock::duration{});

		jc::timer _tm{};
		for (auto& d : _durations)
		{
			_tm.start();
			const auto _invokeResult = jc::invoke(_op, _args...);
			d = std::chrono::duration_cast<duration>(_tm.elapsed());
			_resultRedirect = _invokeResult;
		};

		CPUProfileResult _profileResult{};

		const auto _sum = jc::accumulate(_durations);
		_profileResult.average_time = _sum / _times;
		_profileResult.runtimes = _times;
		_profileResult.max_time = *std::max_element(_durations.begin(), _durations.end());

		return _profileResult;
	};

};