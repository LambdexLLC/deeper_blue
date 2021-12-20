#pragma once

/*
	This is a generalization of the code I threw together to allow for multithreaded
	move tree construction.
*/

#include <jclib/guard.h>
#include <jclib/memory.h>
#include <jclib/optional.h>

#include <mutex>
#include <thread>
#include <atomic>
#include <barrier>
#include <iostream>
#include <functional>



#pragma region THREAD_ID_FORMATTER

// Quick formatter for thread IDs

#include <sstream>

namespace lbx
{
	/**
	 * @brief Just preforms the string conversion for thread IDs
	*/
	template <>
	struct formatter<std::thread::id>
	{
		auto format(const std::thread::id& _id)
		{
			std::stringstream _sstr{};
			_sstr << _id;
			return _sstr.str();
		};
	};
};

#pragma endregion



namespace lbx
{
	/**
	 * @brief Type trait checks if a type is a task.
	 * 
	 * Type must be invocable with no arguements and return void.
	 * 
	 * @tparam T Type to check.
	 * @tparam Enable SFINAE specialization point.
	*/
	template <typename T, typename Enable = void>
	struct is_task : jc::false_type
	{};

#if JCLIB_FEATURE_CONCEPTS_V

	/**
	 * @brief Concept fufilled by types that are considered tasks.
	 *
	 * Type must be invocable with no arguements and return void.
	 *
	 * @tparam T Type to check.
	*/
	template <typename T>
	concept cx_task = requires (T a)
	{
		{ a() } -> jc::cx_same_as<void>;
	};

	/**
	 * @brief Type trait checks if a type is a task.
	 *
	 * Type must be invocable with no arguements and return void.
	 *
	 * @tparam T Type to check.
	*/
	template <typename T> requires cx_task<T>
	struct is_task<T> : jc::true_type
	{};

#else

	/**
	 * @brief Type trait checks if a type is a task.
	 *
	 * Type must be invocable with no arguements and return void.
	 *
	 * @tparam T Type to check.
	*/
	template <typename T>
	struct is_task<T, jc::enable_if_t
		<
			jc::is_invocable<T>::value && jc::is_same<jc::invoke_result_t<T>, void>::value
		>> : jc::true_type
	{};

#endif // JCLIB_FEATURE_CONCEPTS_V

#if JCLIB_FEATURE_INLINE_VARIABLES_V

	/**
	 * @brief Type trait checks if a type is a task.
	 *
	 * Type must be invocable with no arguements and return void.
	 *
	 * @tparam T Type to check.
	*/
	template <typename T>
	constexpr inline auto is_task_v = is_task<T>::value;

#endif // JCLIB_FEATURE_CONCEPTS_V






	











	/**
	 * @brief Interface for a thread that can be assigned work to process.
	 * 
	 * @tparam TaskT Task type for this worker thread to processes.
	 * @tparam Enable SFINAE specialization point.
	*/
	template <typename TaskT, typename Enable = void>
	class basic_worker_thread;


	/**
	 * @brief When true, enables some debug logging for the worker threads.
	*/
	constexpr inline bool log_debug_messages_for_worker_threads_v = false;



	/**
	 * @brief Interface for a thread that can be assigned work to process.
	 * 
	 * @tparam TaskT Task type for this worker thread to processes.
	*/
	template <typename TaskT>
	class basic_worker_thread<TaskT, jc::enable_if_t<is_task<TaskT>::value>>
	{
	public:

		/**
		 * @brief Task type for this thread.
		*/
		using task_type = TaskT;

	private:

		// Simple guard type for auto-dropping the barrier
		struct DropBarrierGuarded
		{
		public:

			void reset()
			{
				this->barrier_->arrive_and_drop();
			};

			DropBarrierGuarded(jc::reference_ptr<std::barrier<>> _barrier) :
				barrier_{ _barrier }
			{};

		private:
			jc::reference_ptr<std::barrier<>> barrier_;
		};
		using DropBarrierGuard = jc::guard<DropBarrierGuarded>;

		/**
		 * @brief State shared between the calling thread and the worker thread.
		*/
		struct shared_state
		{
			/**
			 * @brief Barrier used to hold the thread when no task is assigned.
			*/
			mutable std::barrier<> barrier{ 2, {} };

			/**
			 * @brief Mutex for the task queue.
			*/
			mutable std::mutex mtx{};

			/**
			 * @brief The tasks assigned to this thread.
			*/
			jc::optional<task_type> task{};
			
		};

		/**
		 * @brief The actual main function for the thread
		 * @param _stop Stop token
		 * @param _thread The thread object that owns this thread
		*/
		static void thread_main(std::stop_token _stop, jc::reference_ptr<shared_state> _state)
		{
			auto& _barrier = _state->barrier;

			// Ensures that the barrier is dropped when exiting the thread
			DropBarrierGuard _barrierGuard{ jc::reference_ptr{ _barrier } };

			// Run until done
			while (!_stop.stop_requested())
			{
				// Wait until a task was set or we are being queued to exit
				_barrier.arrive_and_wait();

				// Log working message if enabled
				if constexpr (log_debug_messages_for_worker_threads_v)
				{
					println("Thread {} : Started", std::this_thread::get_id());
				};

				// Process ALL assigned tasks
				while (true)
				{
					// Lock our mutex
					auto _lck = std::unique_lock{ _state->mtx };

					// Exit from processing if there is no task to process
					if (!_state->task)
					{
						// Log working message if enabled
						if constexpr (log_debug_messages_for_worker_threads_v)
						{
							println("Thread {} : Finished", std::this_thread::get_id());
						};

						break;
					}
					else
					{
						// Log working message if enabled
						if constexpr (log_debug_messages_for_worker_threads_v)
						{
							println("Thread {} : Working", std::this_thread::get_id());
						};

						auto& _task = _state->task.value();
						jc::invoke(_task);
						_state->task.reset();
					};
				};
			};

		};


	public:

		/**
		 * @brief Stops the worker thread.
		*/
		void stop_thread()
		{
			// Activate stop token
			if (this->thread_.request_stop())
			{
				// Allow thread to continue
				this->state_->barrier.arrive_and_drop();
			};
		};


		/**
		 * @brief Checks if the thread is currently processing a task.
		 * @return True if working, false otherwise.
		*/
		bool is_working() const
		{
			return this->state_->task.has_value();
		};

		/**
		 * @brief Assigns a task to the thread.
		 * 
		 * This will block if the thread is already working.
		 * 
		 * @param _task Task to assign.
		*/
		template <typename _TaskT>
		JCLIB_REQUIRES((jc::cx_forwardable_to<_TaskT, task_type>))
		auto assign_work(_TaskT&& _task) ->
		JCLIB_RET_SFINAE_CXSWITCH(void, jc::is_forwardable_to<_TaskT, task_type>::value)
		{
			// Set the task
			{
				auto _lck = std::unique_lock{ this->state_->mtx };
				this->state_->task = std::forward<_TaskT>(_task);
			};

			// Allow thread to continue
			this->state_->barrier.arrive_and_wait();
		};

		/**
		 * @brief Blocks execution until the worker thread is finished.
		*/
		void wait_until_finished()
		{
			if(this->is_working())
			{
				this->state_->barrier.arrive_and_wait();
			};
		};





		basic_worker_thread() :
			state_{ new shared_state{} },
			thread_{ thread_main, jc::reference_ptr{ *this->state_ } }
		{};

		~basic_worker_thread()
		{
			this->stop_thread();
		};

	private:

		/**
		 * @brief State shared between the interface and the worker thread.
		*/
		std::unique_ptr<shared_state> state_{};

		/**
		 * @brief The actual thread object.
		*/
		std::jthread thread_;



		// Prevent copy

		basic_worker_thread(const basic_worker_thread& other) = delete;
		basic_worker_thread& operator=(const basic_worker_thread& other) = delete;

	};

	/**
	 * @brief Interface for a thread that can be assigned work to process.
	 * 
	 * This is an alias to make the most common case easier to use.
	*/
	using worker_thread = basic_worker_thread<std::function<void()>>;




	/**
	 * @brief Manages a pool of worker threads and assigns them work.
	*/
	template <typename TaskT, typename Enable = void>
	class basic_worker_pool;

	/**
	 * @brief Manages a pool of worker threads and assigns them work.
	*/
	template <typename TaskT>
	class basic_worker_pool<TaskT, jc::enable_if_t<is_task<TaskT>::value>>
	{
	public:

		/**
		 * @brief The tasks that this thread pool will assign.
		*/
		using task_type = TaskT;

		/**
		 * @brief The worker thread type for this pool.
		*/
		using worker_thread_type = basic_worker_thread<task_type>;

		/**
		 * @brief Size type for this container
		*/
		using size_type = size_t;



		/**
		 * @brief Assigns a task to a worker thread.
		 * @param _task Task to assign.
		*/
		template <typename _TaskT>
		JCLIB_REQUIRES((jc::cx_forwardable_to<_TaskT, task_type>))
			auto assign_work(_TaskT&& _task) ->
			JCLIB_RET_SFINAE_CXSWITCH(void, jc::is_forwardable_to<_TaskT, task_type>::value)
		{
			while (true)
			{
				for (auto& _worker : this->workers_)
				{
					if (!_worker.is_working())
					{
						_worker.assign_work(std::forward<_TaskT>(_task));
						return;
					};
				};
			};
		};

		/**
		 * @brief Waits until all worker threads have finished their tasks
		*/
		void wait_until_all_finished()
		{
			for (auto& _worker : this->workers_)
			{
				_worker.wait_until_finished();
			};
		};




		/**
		 * @brief Creates a new worker thread pool.
		 * @param _size Number of workers for this pool.
		*/
		explicit basic_worker_pool(size_type _size) :
			workers_(_size)
		{
			JCLIB_ASSERT(_size != 0);
		};

	private:

		/**
		 * @brief The set of worker threads in this pool
		*/
		std::vector<worker_thread_type> workers_{};

		/**
		 * @brief The next worker to assign work to
		*/
		size_t next_worker_ = 0;

	};

	/**
	* 
	 * @brief Manages a pool of worker threads and assigns them work.
	 * This is an alias to make the most common case easier to use.
	*/
	using worker_pool = basic_worker_pool<std::function<void()>>;

};

