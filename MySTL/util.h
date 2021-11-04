#pragma once
#include"type_traits.h"
namespace mystl {
	template<class T>
	typename std::remove_reference<T>::type&& move(T&& arg)noexcept {
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}
}