#pragma once

#include "global.h"

namespace utils {
	template <typename T>
	class CircularArray {
	public:
		size_t size;

		CircularArray(const size_t size_ = 0) // must be multiples of buffer_size. assert later
			: size{ size_ },
			data(size_, 0.0f),
			data_array{ data.data() },
			start{ 0 }
		{
			assert((size_ % config::buffer_size) == 0);
		}

		typename std::vector<T>::iterator begin() {
			return data.begin() + start;
		}

		void resize(const size_t size_) {
			data.resize(size_);
			if (size_ > size) {
				data.insert(data.begin() + size, make_move_iterator(data.begin()), make_move_iterator(data.begin() + start));
			}
			size = size_;
			data_array = data.data();
		}

		// doesn't change underlying vector. call resize() with the known maximum size first
		void fast_resize(const size_t size_) {
			size = size_;
		}

		// // might be broken. fix
		//void pop_start_into(std::vector<T>& dest, const typename std::vector<T>::iterator dest_location_iter, const int len) {
		//	if (start + len <= size) {
		//		dest.insert(dest_location_iter, make_move_iterator(data.begin() + start), make_move_iterator(data.begin() + start + len));
		//		//fill_n(data.begin() + start, len, 0.0f);
		//		// instead of wiping the just-read section, keep it undefined? it will be overriden with add_end
		//		start += len;
		//	}
		//	else {
		//		dest.insert(dest_location_iter, make_move_iterator(data.begin() + start), make_move_iterator(data.end()));
		//		dest.insert(dest_location_iter + (size - start), make_move_iterator(data.begin()), make_move_iterator(data.begin() + (len - (size - start))));
		//		//fill_n(data.begin() + start, size - start, 0.0f);
		//		//fill_n(data.begin(), len - (size - start), 0.0f);
		//		start = start + len - size;
		//	}
		//}

		// return: pointer to first segment, length of first segment, pointer to second segment, length of second segment
		T* pop_start_with_pointer(const size_t len) {
			const size_t old_start{ start };
			if (start + len < size) {
				start += len;
			}
			else {
				start += len - size;
			}
			return &data_array[old_start];
		}

		// return: pointer to first segment, length of first segment, pointer to second segment, length of second segment
		std::tuple<T*, size_t, T*, size_t> get_subarray_ptrs(const size_t index, const size_t len) {
			const size_t subarray_start_index{ (start + index) % size };
			const size_t subarray_end_index{ (start + index + len) % size };
			if (subarray_end_index > subarray_start_index) {
				return std::tuple{ &data_array[subarray_start_index], len, nullptr, 0 };
			}
			else {
				return std::tuple{ &data_array[subarray_start_index], size - subarray_start_index, data_array, subarray_end_index };
			}
		}

		T get(const size_t index) {
			const size_t start_plus_index{ start + index };
			if (start_plus_index < size) {
				return data_array[start_plus_index];
			}
			else {
				return data_array[start_plus_index - size];
			}
		}

		void push_back(T* source_array, const int len) {
			if (start == 0) {
				memcpy(&data[size - len], source_array, len * sizeof(T));
			}
			else{
				memcpy(&data[start - len], source_array, len * sizeof(T));
			}
		}

		void push_back(const float_s value) {
			if (start == 0) {
				data[size - 1] = value;
			}
			else {
				data[start - 1] = value;
			}
		}

		void reset() {
			fill(data.begin(), data.end(), 0.0f);
		}

	private:
		std::vector<T> data;
		T* data_array;
		size_t start;
	};
}