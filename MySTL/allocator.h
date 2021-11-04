#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_
#include"construct.h"
//包含一个模板类allocator用于内存分配、释放、对象的构造、析构
namespace mystl{
template <class T>
class allocator{
public :
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;//这种整数用来记录一个大小
	typedef ptrdiff_t difference_type;//一种用来记录两个指针之间的距离的数据类型
public :
	static T* allocate();
	static T* allocate(size_type n);

	static void deallocate(T* ptr);
	static void deallocate(T* ptr, size_type n);

	static void construct(T* ptr);
	static void construct(T* ptr, const T& value);
	static void construct(T* ptr, T&& value);//右值引用

	template<class ...Args>
	static void construct(T* ptr, Args&& ...args);

	static void destory(T* ptr);
	static void destory(T* first, T* last);

};

//无参情况下
template <class T>
T* allocator<T>::allocate() 
{
	return static_cast<T*>(::operator new(sizeof(T)));
}

//有参情况下分配n个大小为T类型的内存
template<typename T > 
T* allocator<T>::allocate(size_type n)
{
	if (n == 0) {
		return nullptr;
	}
	return static_cast<T*>(::operator new(n * sizeof(T)));
}

//内存释放版本1
template<class T>
void allocator<T>::deallocate(T* ptr)
{
	if (ptr == nullptr) {
		return;
	}
	::operator delete(ptr);
}

//内存释放版本2
template <class T>
void allocator<T>::deallocate(T* ptr, size_type n) {
	if (ptr == nullptr) {
		return;
	}
	::operator delete(ptr);
}

//构造版本1，只有指针
template<class T>
void allocator<T>::construct(T* ptr)
{
	mystl::construct(ptr);
}

//构造2，含值构造
template<class T>
void allocator<T>::construct(T* ptr, const T& value)
{
	mystl::construct(ptr, std::move(value));
}

//含多个值构造
template<class T>
template<class ... Args>
void allocator<T>::construct(T* prt, Args&& ...args) {
	mystl:; construct(ptr, std::forward(Args)(args));
}

//销毁一个
template<class T>
void allocator<T>::destory(T* ptr)
{
	mystl::destory(ptr);
}

//迭代销毁
template<class T>
void allocator<T>::destory(T* first, T* last) {
	mystl::destory(first, last);
}

//结束
}//namespace
#endif // !MYTINYSTL_ALLOCATOR_H_
#pragma once
