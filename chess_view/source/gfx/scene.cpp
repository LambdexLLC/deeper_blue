#include "scene.hpp"

#include <jclib/functor.h>
#include <jclib/functional.h>

#include <ranges>
#include <algorithm>

namespace lbx::chess_view
{
	template <typename ClassT, typename FnT>
	struct member_function_ptr;

	template <typename ClassT, typename ReturnT, typename... ArgTs>
	struct member_function_ptr<ClassT, ReturnT(ArgTs...)>
	{
		using type = ReturnT(ClassT::*)(ArgTs...);
	};

	template <typename ClassT, typename FnT>
	using member_function_ptr_t = typename member_function_ptr<ClassT, FnT>::type;





	struct call_member_t : jc::operator_tag
	{
		template <typename ClassT, typename ReturnT, typename... ArgTs>
		struct fn_t : jc::operator_tag
		{
		public:

			using ptr = std::conditional_t
			<
				std::is_const<ClassT>::value,
				ReturnT(ClassT::*)(ArgTs...) const,
				ReturnT(ClassT::*)(ArgTs...)
			>;

			template <typename... ArgTs>
			constexpr auto operator()(ClassT& _class, ArgTs&&... _args) const ->
				decltype(jc::invoke(std::declval<ptr>(), &_class, std::forward<ArgTs>(_args)...))
			{
				return jc::invoke(this->ptr_, &_class, std::forward<ArgTs>(_args)...);
			};

			template <typename... Ts>
			constexpr auto operator()(jc::wildcard wc, Ts&&... _args) const
			{
				return wc;
			};



			constexpr fn_t(ptr _ptr) :
				ptr_{ _ptr }
			{};

			ptr ptr_;
		};

		template <typename ClassT, typename ReturnT, typename... ArgTs>
		constexpr auto operator()(ReturnT(ClassT::* _ptr)(ArgTs...)) const
		{
			return fn_t<ClassT, ReturnT, ArgTs...>{ _ptr };
		};
		
		template <typename ClassT, typename ReturnT, typename... ArgTs>
		constexpr auto operator()(ReturnT(ClassT::* _ptr)(ArgTs...) const) const
		{
			return fn_t<const ClassT, ReturnT, ArgTs...>{ _ptr };
		};
	};
	constexpr extern call_member_t call_member{};


	/**
	 * @brief Adds an element to the scene.
	 * @param _element Element to add to the scene. Must not be null!
	*/
	void Scene::insert(std::unique_ptr<IElement> _element)
	{
		this->elements_.push_back(std::move(_element));
	};

	/**
	 * @brief Shows all elements.
	*/
	void Scene::show()
	{
		std::ranges::for_each(this->elements_, jc::dereference | call_member(&IElement::show));
	};

	/**
	 * @brief Hides all elements.
	*/
	void Scene::hide()
	{
		std::ranges::for_each(this->elements_, jc::dereference | call_member(&IElement::hide));
	};



	Scene::~Scene()
	{
		// Ensure references are destroyed prior to deletion
		for (auto& e : this->elements_)
		{
			e->destroy_references();
		};
	};

};
