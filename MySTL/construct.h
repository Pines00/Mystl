#ifndef MYTINTSTL_CONSTRUCT_H_
#define MYTINTSTL_CONSTRUCT_H_
//这个头文件包含两个函数construct，destroy
//construct负责对象的构造
//destory负责对象的析构
#include<new>
#include"type_traits.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif // _MSC_VER

namespace mystl {
	template<class Ty>
	void construct(Ty* ptr) {
		::new((void*)ptr) Ty(); //::new 表示最外层operator new ,其他两句不是太懂
	}

	template<class Ty1, class Ty2>
	void construct(Ty1* ptr, const Ty2& value) {
		::new((void*)ptr)Ty1(value);
	}

	template<class Ty, class ...Args>
	void construct (Ty *ptr,Args&&... args)
	{
		::new((void*)ptr)Ty(std::forward<Args>(args)...);//forward完美转发
	}
	//destory将对象析构
	template<class Ty>
	void destory_one(Ty*, std::true_type) {}

	template <class Ty>
	void destory_one(Ty* pointer, std::false_type) {
		if (pointer != nullptr) {
			pointer->~Ty();
		}
	}

	template <class ForwardIter>
	void destory_cat(ForwardIter first, ForwardIter last, std::true_type) {}

	template <class ForwardIter>
	void destory_cat(ForwardIter first, ForwardIter last, std::false_type){
		for (; first != lase; ++first) {
			destory(&*first);
		}
	}

	template <class Ty>
	void destory(Ty* pointer)
	{
		destory_one(pointer, std::is_trivially_destructible<Ty>{});
	}

	template <class ForwardIter >
	void destory(ForwardIter first,ForwardIter last)
	{
		destpry_cat(first, last, std::is_trivially_destructiable<typename iterator_traits<ForwardIter>::value_type>{});
	}
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
#endif // !MYTINTSTL_CONSTRUCT_H_
