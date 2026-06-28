#pragma once

#include "utils/global.h"

#include <new>

// https://andreleite.com/posts/2025/deque/work-stealing-deque-part-1-locks-and-contention/

namespace synthesis {
	template <typename T, size_t N> // N must be power of 2
	class AtomicCircularDeque {
	public:
		size_t size;

		AtomicCircularDeque() // must be multiples of buffer_size (=> also power of 2). assert later
			: data{},
			front{ 1 },
			back{ 1 }
		{
			static_assert((N & (N - 1)) == 0); // N must be power of 2
		}
		
		// owner
		int push_back(T item) {
			uint32_t back_local{ back.load(std::memory_order_relaxed) };
			data[back_local % N] = item;
			back_local++;
			back.store(back_local, std::memory_order_release);
			return 0;
		}

		// owner
		int pop_back(T* out) {
			uint32_t back_local{ back.load(std::memory_order_relaxed) };
			back_local--;
			back.store(back_local, std::memory_order_relaxed);
			std::atomic_thread_fence(std::memory_order_seq_cst);

			uint32_t front_local{ front.load(std::memory_order_acquire) };
			if (front_local <= back_local) {
				T item{ data[back_local % N] };

				if (front_local == back_local) { // popped the last item
					if (!front.compare_exchange_strong(front_local, front_local + 1,
						std::memory_order_seq_cst, std::memory_order_relaxed))
					{
						back.store(back_local + 1, std::memory_order_relaxed);
						return 1; // empty
					}
					back.store(back_local + 1, std::memory_order_relaxed);
				}
				*out = item;
				return 0;
			} else {
				back.store(back_local + 1, std::memory_order_relaxed);
				return 1; // already stolen
			}
		}

		// thief
		int pop_front(T* out) {
			uint32_t front_local{ front.load(std::memory_order_relaxed) };
			std::atomic_thread_fence(std::memory_order_seq_cst);
			uint32_t back_local{ back.load(std::memory_order_acquire) }; // acquire means i pull the variable for subsequent instructions to see

			if (front_local < back_local) {
				T item{ data[front_local % N] };
				if (front.compare_exchange_strong(front_local, front_local + 1,
					std::memory_order_seq_cst, std::memory_order_relaxed))
				{
					*out = item;
					return 0;
				}
			}

			return 1;
		}

	private:
		std::array<T, N> data;

		alignas(std::hardware_destructive_interference_size)
			std::atomic<uint32_t> front;
		alignas(std::hardware_destructive_interference_size)
			std::atomic<uint32_t> back; // next EMPTY entry. not inclusive. > front
	};
}