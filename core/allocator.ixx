export module eqallocator;

import <memory>;
import <type_traits>;
import <utility>;
import <new>;

export namespace eqlib {

	// Functions for memory allocation
	export void* eqAlloc(std::size_t sz);
	export void eqFree(void* ptr);

	// EverQuest-specific allocator template
	export template <typename T>
		struct everquest_allocator
	{
		using value_type = T;
		using propogate_on_container_move_assignment = std::true_type;
		using is_always_equal = std::true_type;

		everquest_allocator() noexcept = default;
		everquest_allocator(const everquest_allocator&) noexcept = default;
		~everquest_allocator() = default;

		template <class U>
		everquest_allocator(const everquest_allocator<U>&) noexcept {}

		T* allocate(std::size_t count)
		{
			return reinterpret_cast<T*>(eqAlloc(count * sizeof(T)));
		}

		void deallocate(T* p, std::size_t)
		{
			eqFree(p);
		}
	};

	export template <typename T>
		bool operator==(const everquest_allocator<T>&, const everquest_allocator<T>&) noexcept
	{
		return true;
	}

	export template <typename T>
		bool operator!=(const everquest_allocator<T>&, const everquest_allocator<T>&) noexcept
	{
		return false;
	}

	namespace detail
	{
		// Tag type for custom allocation
		export constexpr struct EqAllocate {} eqAllocator;
	}

	// Functions for creating and destroying objects
	export template <typename T>
		void eqDelete(T* m)
	{
		if (m)
		{
			std::destroy_at(m);
			eqFree(m);
		}
	}

	export template <typename T>
		[[nodiscard]] T* eqNew()
	{
		return new (detail::eqAllocator) T;
	}

	export template <typename T, typename... Types>
		[[nodiscard]] T* eqNew(Types&&... args)
	{
		return new (detail::eqAllocator) T(std::forward<Types>(args)...);
	}

	export template <typename T>
		void eqVecDelete(T* m)
	{
		if (m)
		{
			if constexpr (std::is_trivially_destructible_v<T>)
			{
				eqFree(m);
			}
			else
			{
				std::size_t* ptr = reinterpret_cast<std::size_t*>(m) - 1;
				std::destroy_n(m, *ptr);
				eqFree(ptr);
			}
		}
	}

	export template <typename T>
		[[nodiscard]] T* eqVecNew(std::size_t count)
	{
		if constexpr (std::is_trivially_destructible_v<T>)
		{
			T* ptr = reinterpret_cast<T*>(eqAlloc(count * sizeof(T)));
			if (ptr && !std::is_trivially_default_constructible_v<T>)
			{
				std::uninitialized_default_construct_n(ptr, count);
			}
			return ptr;
		}
		else
		{
			std::size_t sizeBytes = sizeof(std::size_t) + sizeof(T) * count;
			std::size_t* ptr = reinterpret_cast<std::size_t*>(eqAlloc(sizeBytes));
			if (ptr)
			{
				*ptr = count;
				T* result = reinterpret_cast<T*>(ptr + 1);
				if constexpr (!std::is_trivially_default_constructible_v<T>)
				{
					std::uninitialized_default_construct_n(result, count);
				}
				return result;
			}
		}
		return nullptr;
	}

	// allocate memory as if by using eq's malloc.
	using eqAllocFn = void* (*)(std::size_t amount);
	eqAllocFn eqAlloc_ = nullptr;

	void* eqAlloc(std::size_t sz)
	{
		return eqAlloc_(sz);
	}

	// free memory as if by using eq's free.
	using eqFreeFn = void (*)(void*);
	eqFreeFn eqFree_ = nullptr;

	void eqFree(void* ptr)
	{
		eqFree_(ptr);
	}


} // namespace eqlib

// Operator overloads for eqlib::detail::EqAllocate tag
export inline void* operator new (std::size_t bytes, eqlib::detail::EqAllocate)
{
	return eqlib::eqAlloc(bytes);
}

export inline void* operator new[](std::size_t bytes, eqlib::detail::EqAllocate)
{
	return eqlib::eqAlloc(bytes);
}

export inline void operator delete(void* m, eqlib::detail::EqAllocate)
{
	eqlib::eqFree(m);
}

export inline void operator delete[](void* m, eqlib::detail::EqAllocate)
{
	eqlib::eqFree(m);
}
