#pragma once

#include <jclib/concepts.h>
#include <jclib/type_traits.h>

#include <memory>
#include <vector>

namespace jc
{
	// Forward decl for slot type
	template <typename... ArgTs>
	class listener;

	// Indirection to allow invoking listeners from slot
	template <typename... ArgTs, typename... _ArgTs>
	requires (jc::cx_forwardable_to<_ArgTs&&, ArgTs> && ...)
		static void invoke_listener(listener<ArgTs...>& _listener, _ArgTs&&... _args);

	template <typename... ArgTs>
	class slot
	{
	public:

		// Listener type
		using listener_type = listener<ArgTs...>;

		template <typename... _ArgTs>
		requires (jc::cx_forwardable_to<_ArgTs&&, ArgTs> && ...)
			void invoke(_ArgTs&&... _args)
		{
			for (auto& l : this->listeners_)
			{
				invoke_listener(*l, std::forward<_ArgTs>(_args)...);
			};
		};

		void insert(listener_type* _listener)
		{
			JCLIB_ASSERT(_listener && !jc::contains(this->listeners_, _listener));
			this->listeners_.push_back(_listener);
		};
		void erase(listener_type* _listener)
		{
			std::erase(this->listeners_, _listener);
		};

		slot() = default;

	private:
		std::vector<listener_type*> listeners_;
	};

	/**
	 * @brief Listens for window size change events
	*/
	template <typename... ArgTs>
	class listener
	{
	public:
		using slot_type = slot<ArgTs...>;

	private:

		void insert_listener()
		{
			if (this->slot_)
			{
				this->slot_->insert(this);
			};
		};
		void erase_listener()
		{
			if (this->slot_)
			{
				this->slot_->erase(this);
			};
		};

	public:

		virtual void invoke(ArgTs... _args) = 0;



		listener() = default;

		listener(const std::shared_ptr<slot_type>& _slot) :
			slot_{ _slot }
		{
			this->insert_listener();
		};

		listener(const listener& other) :
			slot_{ other.slot_ }
		{
			this->insert_listener();
		};
		listener& operator=(const listener& other)
		{
			this->erase_listener();
			this->slot_ = other.slot_;
			this->insert_listener();
			return *this;
		};

		listener(listener&& other) noexcept :
			slot_{ other.slot_ }
		{
			other.erase_listener();
			this->insert_listener();
		};
		listener& operator=(listener&& other) noexcept
		{
			this->erase_listener();
			other.erase_listener();
			this->slot_ = std::move(other.slot_);
			this->insert_listener();
			return *this;
		};

		~listener()
		{
			this->erase_listener();
		};

	private:
		std::shared_ptr<slot_type> slot_;
	};

	// Indirection to allow invoking listeners from slot
	template <typename... ArgTs, typename... _ArgTs>
	requires (jc::cx_forwardable_to<_ArgTs&&, ArgTs> && ...)
		static void invoke_listener(listener<ArgTs...>& _listener, _ArgTs&&... _args)
	{
		_listener.invoke(std::forward<_ArgTs>(_args)...);
	};
};