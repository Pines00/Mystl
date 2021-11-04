#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_
#include"construct.h"
//����һ��ģ����allocator�����ڴ���䡢�ͷš�����Ĺ��졢����
namespace mystl{
template <class T>
class allocator{
public :
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;//��������������¼һ����С
	typedef ptrdiff_t difference_type;//һ��������¼����ָ��֮��ľ������������
public :
	static T* allocate();
	static T* allocate(size_type n);

	static void deallocate(T* ptr);
	static void deallocate(T* ptr, size_type n);

	static void construct(T* ptr);
	static void construct(T* ptr, const T& value);
	static void construct(T* ptr, T&& value);//��ֵ����

	template<class ...Args>
	static void construct(T* ptr, Args&& ...args);

	static void destory(T* ptr);
	static void destory(T* first, T* last);

};

//�޲������
template <class T>
T* allocator<T>::allocate() 
{
	return static_cast<T*>(::operator new(sizeof(T)));
}

//�в�����·���n����СΪT���͵��ڴ�
template<typename T > 
T* allocator<T>::allocate(size_type n)
{
	if (n == 0) {
		return nullptr;
	}
	return static_cast<T*>(::operator new(n * sizeof(T)));
}

//�ڴ��ͷŰ汾1
template<class T>
void allocator<T>::deallocate(T* ptr)
{
	if (ptr == nullptr) {
		return;
	}
	::operator delete(ptr);
}

//�ڴ��ͷŰ汾2
template <class T>
void allocator<T>::deallocate(T* ptr, size_type n) {
	if (ptr == nullptr) {
		return;
	}
	::operator delete(ptr);
}

//����汾1��ֻ��ָ��
template<class T>
void allocator<T>::construct(T* ptr)
{
	mystl::construct(ptr);
}

//����2����ֵ����
template<class T>
void allocator<T>::construct(T* ptr, const T& value)
{
	mystl::construct(ptr, std::move(value));
}

//�����ֵ����
template<class T>
template<class ... Args>
void allocator<T>::construct(T* prt, Args&& ...args) {
	mystl:; construct(ptr, std::forward(Args)(args));
}

//����һ��
template<class T>
void allocator<T>::destory(T* ptr)
{
	mystl::destory(ptr);
}

//��������
template<class T>
void allocator<T>::destory(T* first, T* last) {
	mystl::destory(first, last);
}

//����
}//namespace
#endif // !MYTINYSTL_ALLOCATOR_H_
#pragma once
