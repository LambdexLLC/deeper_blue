#pragma once

#include "utility/http.hpp"
#include "utility/json.hpp"

#include <jclib/memory.h>

#include <thread>
#include <queue>
#include <mutex>

namespace lbx::http
{
	/**
	 * @brief Wrapper around a client event stream, this will block the thread like crazy so it is best
	 * to shove this in a seperate thread.
	*/
	class HTTPClientEventStream
	{
	public:

		class Stream
		{
		private:

			/**
			 * @brief Actual buffer state
			*/
			struct Buffer
			{
			public:

				/**
				 * @brief Acquires a unique lock of the mutex
				 * @return Unique lock
				*/
				[[nodiscard]] auto lock() const
				{
					return std::unique_lock{ this->mtx_ };
				};

				void push(json _json)
				{
					auto _lck = this->lock();
					this->events_.push(std::move(_json));
				};
				bool empty() const
				{
					auto _lck = this->lock();
					return this->events_.empty();
				};
				auto size() const
				{
					auto _lck = this->lock();
					return this->events_.size();
				};
				void ignore()
				{
					auto _lck = this->lock();
					this->events_.pop();
				};
				auto next()
				{
					auto _lck = this->lock();
					auto _out = std::move(this->events_.front());
					this->events_.pop();
					return _out;
				};

				Buffer() = default;

			private:
				mutable std::mutex mtx_;
				std::queue<json> events_;
			};

		public:

			bool has_events() const
			{
				return !this->buffer_->empty();
			};
			auto next_event()
			{
				return this->buffer_->next();
			};
			void ignore_event()
			{
				this->buffer_->ignore();
			};

		private:

			Stream(const std::shared_ptr<Buffer>& _buffer) :
				buffer_{ _buffer }
			{};

			friend HTTPClientEventStream;

			std::shared_ptr<Buffer> buffer_;
		};

	private:

		static jc::reference_ptr<http::Client> preproc_client_arg(jc::reference_ptr<http::Client> _client)
		{
			_client->set_keep_alive(true);
			return _client;
		};

		void on_recieve_event(const json& _data)
		{
			this->buffer_->push(_data);
		};
		void on_recieve_event(json&& _data)
		{
			this->buffer_->push(std::move(_data));
		};

		static void thread_main(std::stop_token _stop, HTTPClientEventStream* _this, const char* _path)
		{
			http::Headers _headers{};
			_this->client_->set_read_timeout(std::chrono::minutes{ 2 });

			auto _foo = _this->client_->Get(_path, _headers,
				[&](const http::Response& _response) -> bool
				{
					return !_stop.stop_requested();
				},
				[&](const char* _data, size_t _len) -> bool
				{
					if (_len > 1)
					{
						auto _node = json::parse(std::string_view{ _data, _len }, nullptr, false);
						if (!_node.is_discarded())
						{
							_this->on_recieve_event(std::move(_node));
						};
					}
					else
					{
						json _json{ { "still-alive", true } };
						_this->on_recieve_event(_json);
					};
					return !_stop.stop_requested();
				}
				);
			const auto _err = _foo.error();
			if (_err == Error::Success)
			{
				// good;
			};
		};

	public:

		Stream get_stream() const
		{
			return Stream{ this->buffer_ };
		};
		void set_to_close()
		{
			this->thread_.get_stop_source().request_stop();
		};

		HTTPClientEventStream(jc::reference_ptr<http::Client> _client, const char* _path) :
			client_{ preproc_client_arg(_client) },
			buffer_{ new Stream::Buffer{} },
			thread_{ &HTTPClientEventStream::thread_main, this, _path }
		{};

	private:

		/**
		 * @brief The HTTPS client
		*/
		jc::reference_ptr<http::Client> client_;

		/**
		 * @brief Buffer for holding received event
		*/
		std::shared_ptr<Stream::Buffer> buffer_;


		std::jthread thread_;
	};
};