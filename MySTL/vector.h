#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

#include<initializer_list>
#include<memory.h>
#include"exceptdef.h"
#include <cassert>
#include"iterator.h"
#include"allocator.h"
namespace mystl
{
#ifdef  max
#pragma message("#undefing marco max")
#undef max
#endif //  max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // 

	template<class T>
	class vector {
		static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl ");
	public:
		typedef mystl::allocator<T>  allocator_type;
		typedef mystl::allocator<T>	 data_allocator;

		typedef typename allocator_type::value_type			value_type;
		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename allocator_type::size_type			size_type;
		typedef typename allocator_type::difference_type	difference;

		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		//到底哪里出问题了。。。。。。。....原来是没有包含头文件iterator....

		typedef mystl::reverse_iterator<iterator>			reverse_iterator;
		typedef mystl::reverse_iterator<const_iterator>		const_reverse_iterator;
		allocator_type get_allocator() {
			return data_allocator();
		}
	private:
		iterator begin_;
		iterator end_;
		iterator cap_;  //表示目前存储空间的尾部
	public:
		//构造、赋值、移动、析构函数
		vector()noexcept {
			try_init();
		}
		explicit vector(size_type n) {
			fill_init(n, value_type());
		}
		vector(size_type n, const value_type& value) {
			fill_init(n, value);
		}

		template <class Iter, typename std::enable_if<
			mystl::is_input_iterator<Iter>::value, int>::type = 0>
			vector(Iter first, Iter last) {
			MYSTL_DEBUG(!(last < first));
			range_init(first, last);
		}
		vector(const vector& rhs) {
			range_init(rhs.begin_, rhs.end_);
		}
		vector(vector&& rhs)noexcept :
			begin_(rhs.begin_),
			end_(rhs.end_),
			cap_(rhs.cap_) {
			rhs.begin_ = nullptr;
			rhs.end_ = nullptr;
			rhs.cap_ = nullptr;
		}
		vector(std::initializer_list<value_type>ilist) {
			range_init(ilist.begin(), ilist.end());
		}

		//重写 =
		vector& operator=(const vector& rhs);
		vector& operator=(vector&& rhs)noexcept;
		vector& operator=(std::initializer_list<value_type>ilist) {//多个参数形成一个列表
			vector tmp(ilist.begin(), ilist.end());
			swap(tmp);
			return *this;
		}
		//析构函数
		~vector() {
			destory_and_recover(begin_, end_, cap_ - begin_);
			begin_ = end_ = cap_ = nullptr;
		}
		//迭代器相关操作
	public:
		iterator begin()noexcept {
			return begin_;
		}
		const_iterator begin()const noexcept {
			return begin_;
		}
		iterator end() noexcept {
			return end_;
		}
		const_iterator end() const noexcept {
			return end_;
		}
		//reverse_iterator
		reverse_iterator rbegin()noexcept {
			return reverse_iterator(end());
		}
		const_reverse_iterator rbegin()const noexcept {
			return const_reverse_iterator(end());
		}
		reverse_iterator rend()noexcept {
			return reverse_iterator(begin());
		}
		const_reverse_iterator rend()const noexcept {
			return reverse_iterator(begin());
		}
		const_iterator cbegin()const noexcept {
			return begin();
		}
		const_iterator cend()const noexcept {
			return end();
		}
		const_reverse_iterator crbegin()const noexcept {
			return rbegin();
		}
		const_reverse_iterator crend()const noexcept {
			return rend();
		}

		//容量相关操作
		bool empty()const noexcept {
			return begin_ == end_;
		}
		size_type size() const noexcept {
			return static_cast<size_type>(end_ - begin_);
		}
		size_type max_size()const noexcept {
			return static_cast<size_type>(-1) / sizeof(T);
		}

		size_type capacity()const noexcept {
			return static_cast<size_type>(cap_ - begin_);
		}
		void reserve(size_type n);
		void shrink_to_fit();
		//访问元素相关操作
		reference operator[](size_type n) {//函数返回的是第n个元素的指针
			MYSTL_DEBUG(n < size());
			return *(begin_ + n);
		}

		const_reference operator[](size_type n)const {
			MYSTL_DEBUG(n < size());
			return *(begin_ + n);
		}
		reference at(size_type n) {
			THROW_LENGTH_ERROR_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}
		const_reference at(size_type n) const {
			THROW_LENGTH_ERROR_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}
		reference front() {
			MYSTL_DEBUG(!empty());
			return *begin_;
		}
		const_reference front()const {
			MYSTL_DEBUG(!empty());
			return *begin_;
		}
		reference back() {
			MYSTL_DEBUG(!empty());
			return *(end_ - 1);
		}
		const_reference back()const {
			MYSTL_DEBUG(!empty());
			return *(end_ - 1)
		}
		pointer data()noexcept {
			return begin_;
		}
		const_pointer data()noexcept {
			return begin_;
		}
		//修改容器相关操作
		//assign
		void assign(size_type n, const value_type& value) {
			fill_assign(n, value);
		}
		template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void assign(Iter first, Iter last) {
			MYSTL_DEBUG(!(last < first));
			copy_assign(first, last, iterator_category(first));//copy_assign还没有实现，待会实现
		}
		void assign(std::initializer_list<value_type>il) {
			copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});
		}
		//emplace 
		template <class ... Args>
		iterator emplace(const_iterator pos, Args&& ... args);
		template<class ...Args>
		void emplace_back(Args&& ...args);
		//push_back()
		void push_back(const value_type& value);
		void push_back(value_type&& value) {
			emplace_back(mystl::move(value));
		}
		void pop_back();

		//insert
		iterator insert(const_iterator pos, const value_type& value);
		iterator insert(const_iterator pos, value_type&& value) {
			return emplace(pos, mystl::move(value));
		}
		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			MYSTL_DEBUG(pos >= begin() && pos <= end());
			return fill_insert(const_cast<iterator>(pos), n, value);
		}
		template<class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0 >
		void insert(const_iterator pos, Iter first, Iter last) {
			MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
			copy_insert(const_cast<iterator>(pos), first, last);//借助copy_insert来实现
		}
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void clear() { erase(begin(), end()); }
		//resize   //reverse
		void resize(size_type new_size) { return resize(new_size, value_type()); }
		void resize(size_type new_size, const value_type& value);
		void reverse() {
			std::reverse(begin(), end());
		}
		//swap()
		void swap(vector& rhs)noexcept;
	private:
		void try_init() noexcept;
		void init_space(size_type size, size_type cap);
		void fill_init(size_type n, const value_type& value);
		template <class Iter>
		void range_init(Iter first, Iter last);
		void destory_and_recover(iterator first, iterator last, size_type n);
		size_type get_new_cap(size_type add_size);
		void fill_assign(size_type n, const value_type& value);
		template<class IIter>
		void copy_assign(IIter first, IIter last, input_iterator_tag);
		template<typename FIter>
		void copy_assign(FIter first, FIter last, forward_iterator_tag);
		//reallocate
		template<class... Args>
		void reallocate_emplace(iterator pos, Args&&... args);
		void reallocate_insert(iterator pos, const value_type& value);
		//insert
		iterator fill_insert(iterator pos, size_type n, const value_type& value);
		template<class IIter>
		void copy_insert(iterator pos, IIter first, IIter last);
		void reinsert(size_type size);
	};
	//赋值操作符
	template<class T>
	vector<T>& vector<T>::operator=(const vector& rhs) {
		if (this != &rhs) {
			const auto len = rhs.size();
			if (len > capacity()) {//len>this.capacity情况下，需要重现构造一个vector，然后swap
				vector tmp(rhs.begin(), rhs.end());
				swap(tmp);
			}
			else if (size() > len) {//当this.size()>=len，这个len是rhs的长度,首先copy到this中，然后销毁i
				auto i = std::copy(rhs.begin(), rhs.end(), begin());
				data_allocator::destory(i, end_);
				end_ = begin_ + len;
			}
			else {//直接复制就行了
				std::copy(rhs.begin(), rhs.begin()+size(), begin_);
				mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
				cap_ = end_ = begin_ + len;
			}
			return *this;
		}
	}
	//移动赋值操作
	template<class T>
	vector<T>& vector<T>::operator=(vector&& rhs)noexcept {
		destory_and_recover(begin_, end_, cap_ - begin_);
		begin_ = rhs.begin_;
		end_ = rhs.end_;
		cap_ = rhs.cap_;
		rhs.begin_ = nullptr;
		rhs.end_ = nullptr;
		rhs.cap_ = nullptr;
		return *this;
	}

	 //预留空间大小，当原容量小于要求大小时，才会重新分配
	 template <class T>
	 void vector<T>::reserve(size_type n) {
		 if (capacity() < n) {
			 THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size() in vector<T>::reserve(n)");
			 const auto old_size = size();
			 auto tmp = data_allocator::allocatoe(n);
			 mystl::uninitialized_move(begin_, end_, tmp);
			 data_allocator::deallocate(begin_, cap_ - begin_);
			 begin_ = tmp;
			 end_ = tmp + old_size;
			 cap_ = begin_ + n;
		 }
	 }
	 //抛弃多余容量
	 template<class T>
	 void vector<T>::shrink_to_fit() {
		 if (end_ < cap_) {
			 reinsert(size());
		 }
	 }
	 //在pos位置就地构造元素，避免额外的复制和移动开销
	 template<class T>
	 template<class ... Args>
	 typename vector<T>::iterator vector<T>::emplace(const_iterator pos, Args&&... args) {
		 MYSTL_DEBUG(pos >= begin() && pos <= end());
		 iterator xpos = const_cast<iterator>(pos);
		 const size_type n = xpos - begin_;
		 if (end_ != cap_ && xpos == end_) {
			 data_allocator::construct(mystl::address_of(*end_), mystl::forward(Args)(args)...);
			 ++end_;
		 }
		 else if (end_!=cap_) {
			 auto new_end = end_;
			 data_allocator::construct(mystl::address(*end), *(end_ - 1));
			 /*
			 * void construct(Ty1* ptr, const Ty2& value) {
					::new((void*)ptr)Ty1(value);
				}
			 */
			 ++new_end;
			 mystl::copy_backward(xpos, end_ - 1, end_);
			 *xpos = value_type(mystl::forward<Args>(args)...);
		 }
		 else {
			 reallocate_emplace(xpos, mystl::forward<Args>(args)...);
		 }
		 return begin() + n;
	 }
	 //在尾部就地构造元素，避免额外的复制或移动开销
	 template< class T>
	 template<class ...Args>
	 void vector<T>::emplace_back(Args&& ... args) {
		 if (end_ < cap_) {
			 data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
			 ++end_;
		 }
		 else {
			 reallocate_emplace(end_, mystl::forward<Args>(args)...);
		 }
	 }
	 //在尾部插入元素
	 template<class T>
	 void vector<T>::push_back(const value_type& value) {
		 if (end_ != cap_) {
			 data_allocator::construct(mystl::addres_of(*end_), value);
			 ++end_;
		 }
		 else {
			 reallocate_insert(end_, value);
		 }
	 }
	 //弹出尾部元素
	 template<class T>
	 void vector<T>::pop_back() {
		 MYSTL_DEBUG(!empty());
		 data_allocator::destory(end_ - 1);
		 --end_;
	 }
	 //在pos处插入元素
	 template<class T>
	 typename vector<T>::iterator vector<T>::insert(const_iterator pos, const value_type & value) {
		 MYSTL_DEBUG(pos >= begin() && pos <= end());
		 iterator xpos = const_cast<iterator>(pos);
		 const size_type n = pos - begin_;
		 if (end_!= cap_ && xpos == end_) {//插入位置在末尾的时候
			 data_allocator::construct(mystl::address_of(*end_), value);
			 ++end_;
		 }
		 else if (end_ != cap_) {//插入位置不在末尾，需要先在末尾构建一个空间，然后元素后移，然后插入元素
			 auto new_end = end_;
			 data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
			 ++new_end;
			 auto value_copy = value;
			 mystl::copy_backward(xpos, end_ - 1, end);
			 *xpos = mystl::move(value_copy);
			 end_ = new_end;
		 }
		 else {//其他情况下
			 reallocate_insert(xpos, value);
		 }
		 return begin_ + n;
	 }
	 //删除pos位置上的元素
	 template<class T>
	 typename vector<T>::iterator vector<T>::erase(const_iterator pos) {//返回erase
		 MYSTL_DEBUG(pos >= begin() && pos < end());
		 iterator xpos = begin_ + (pos - begin());
		 mystl::move(xpos+1,end_,xpos);//这个函数暂时理解为前移元素
		 data_allocator::destory(end_ - 1);//删除最后一个元素所占空间
		 --end_;
		 return xpos;
	 }
	 //删除[first,last)中的元素
	 template<class T>
	 typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
		 MYSTL_DEBUG(first>=begin()&& last<=end()&&!(last<first));
		 const auto n = first - begin();
		 iterator r = begin_ + (first - begin());
		 data_allocator::destory(mystl::move(r+(last-first),end_,r),end_);
		 end_ = end_ - (last - first);
		 return begin_ + n;
	 }
	 //重置容器大小
	 template<class T>
	 void vector<T>::resize(size_type new_size,const value_type& value) {
		 if (new_size < size()) {
			 erase(begin() + new_size, end());
		 }
		 else {
			 insert(end(), new_size - size(), value);
		 }
	 }
	 //与另一个vector交换
	 template<class T>
	 void vector<T>::swap(vector& rhs)noexcept {
		 if (this != rhs) {
			 mystl::swap(begin_, rhs.begin);
			 mystl::swap(end_, rhs.end);
			 mystl::swap(cap_, rhs.cap_);
		 }
	 }



	 //下面是一些辅助函数
	 // 	 	//一些辅助函数	
	 template<class T>
	 void vector<T>::try_init() noexcept
	 {
		 try {
			 begin_ = data_allocator::allocate(16);
			 end_ = begin_;
			 cap_ = begin_ + 16;
		 }
		 catch (...) {
			 begin_ = nullptr;
			 end_ = nullptr;
			 cap_ = nullptr;
		 }
	 }
	 template<class T>
	 void vector<T>::init_space(size_type size, size_type cap) {
		 try {
			 begin_ = data_allocator::allocate(cap);
			 end_ = begin_ + size;
			 cap_ = begin_ + cap;
		 }
		 catch (...) {
			 begin_ = nullptr;
			 end_ = nullptr;
			 cap_ = nullptr;
			 throw;
		 }
	 }

	 template<class T>
	 void vector<T>::fill_init(size_type n, const value_type& value) {
		 const size_type init_size = std::max(static_cast<size_type>(16), n);
		 init_space(n, init_size);
		 mystl::uninitialized_fill_n(begin_, n, value);//uninitialized.h头文件下的一个函数，作用是填充值
	 }

	 template<class T>
	 template<class Iter>
	 void vector<T>::range_init(Iter first, Iter last) {
		 const size_type init_size = std::max(static_cast<size_type>(last - first), static_cast<size_type>(16));
		 init_space(static_cast<size_type>(last - first), init_size);
		 mystl::uninitialized_copy(first, last, begin_);
	 }
	 template <class T>
	 void vector<T>::destory_and_recover(iterator first, iterator last, size_type n) {
		 data_allocator::destory(first, last);
		 data_allocator::deallocate(first, last);
	 }
	 template<class T>
	 typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size){
		 const auto old_size = capacity();//记录旧空间大小
		 THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, "vector<T>'s size too big");
		 if (old_size > max_size() - old_size / 2) {
			 return old_size + add_size > max_size() - 16 ? old_size + add_size : old_size + add_size + 16;
		 }
		 const size_type new_size = old_size == 0 ? std::max(add_size, static_cast<size_type>(16)) : std::max(old_size + old_size / 2, old_size + add_size);
		 return new_size;
	 }
	 //fill_assign
	 template<class T>
	 void vector<T>::fill_assign(size_type n,const value_type& value) {
		 if (n > capacity()) {//n大于容器的最大容量重新构造一个容器，然后交换
			 vector tmp(n, value);
			 swap(tmp);
		 }
		 else if (n>size()) {//size是end-begin，代表元素个数，如果n大于元素个数，需要将前面的值覆盖，然后填充后面的值
			 std::fill(begin(), end(), value);
			 end_ = mystl::uninitialized_fill_n(end_, n - size(), value);//从起始位置开始，填充n个元素值，返回填充结束的位置
		 }
		 else {//直接将值覆盖
			 erase(std::fill_n(begin_, n, value), end_);
		 }
	 }
	 //copy_assign函数
	 template<class T>
	 template<class IIter>
	 void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
		 auto cur = begin_;
		 for (; first != last && cur != end_; ++first, ++cur) {
			 *cur = *first;
		 }
		 if (first == last) {
			 erase(cur, end_);//copy之后将他们删除
		 }
		 else {
			 insert(end_, first, last);
		 }
	 }
	 //用[first,last）为容器赋值
	 template<class T>
	 template<class FIter>
	 void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
		 const size_type  len = mystl::distance(first, last);
		 if (len > capacity()) {//大于空间长度需要重新构造
			 vector tmp(first,last);
			 swap(tmp);
		 }
		 else if (size()>len) {//元素个数大于需要长度，需要将后面位置的元素销毁
			 auto new_end = std::copy(first, last, begin_);//返回end位置
			 data_allocator::destory(new_end,end_);//销毁后面位置的元素
			 end_ = new_end;
		 }
		 else {
			 auto mid = first;
			 mystl::advance(mid, size());
			 mystl::copy(first, mid, begin_);
			 auto new_end = std::uninitialized_copy(mid, last, end_);
			 end_ = new_end;
		 }
	 }
	 //重新分配空间并在pos处就地构造元素
	 template<typename T>
	 template<typename ...Args>
	 void vector<T>::reallocate_emplace(iterator pos, Args && ...args) {
		 const auto new_size = get_new_cap(1);
		 auto new_begin = data_allocator::allocate(new_size);
		 auto new_end = new_begin;
		 try {
			 new_end = mystl::uninitialized_move(begin_,pos,new_begin);
			 data_allocator::constuct(mystl::address_of(*new_end), value_copy);
			 ++new_end;
			 new_end = mystl::uninitialized_move(pos, end_, new_end);
		 }
		 catch (...) {
			 data_allocator::deallocate(new_begin, new_size);
			 throw;
		 }
		 destory_and_recover(begin_, end_, cap_ - begin_);
		 begin_ = new_begin;
		 end_ = new_end;
		 cap_ = new_begin + new_size;
	 }
	 //fill_insert函数
	 template<typename T>
	 typename vector<T>::iterator vector<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
		 if (n == 0) {
			 return pos;
		 }
		 const size_type xpos = pos - begin_;
		 const value_type value_copy = value;
		 if (static_cast<size_type>(cap_ - end_) >= n) {
			 //如果备用空间大于等于增加的空间
			 const size_type after_elems = end_ - pos;
			 auto old_end = end_;
			 if (after_elems > n) {
				 mystl::uninitialized_copy(end_ - n, end_, end_);
				 end_ += n;
				 mystl::move_backward(pos, old_end - n, old_end);
				 mystl::uninitialized_fill_b(pos, n, value_copy);
			 }
			 else {
				 end_ = mystl::uninitialized_fill_n(end_, n - after_elems, value_copy);
				 end_ = mystl::uninitialized_move(pos, old_end, end_);
				 mystl::uninitialized_fill_n(pos, after_elems, value_copy);
			 }
		 }
		 else {
			 //如果备用空间不足
			 const auto new_size = get_new_cap(n);
			 auto new_begin = data_allocator::allocate(new_size);
			 auto new_end = new_begin;
			 try {
				 new_end = mystl::uninitialized_move(begin_, pos, new_begin);
				 new_end = mystl::uninitialized_fill_n(new_end, n, value);
				 new_end = mystl::uninitialized_move(pos, end_, new_end);
			 }
			 catch(...){
				 destory_and_recover(new_begin, new_end, new_size);
				 throw;
			 }
			 data_allocator::deallocator(begin_, cap_ - begin_);
			 begin_ = new_begin;
			 end_ = new_end;
			 cap_ = begin_ + new_size;
		 }
		 return begin_ + xpos;
	 }
	 //copy_insert函数
	 template<typename T>
	 template<typename IIter>
	 void vector<T>::copy_insert(iterator pos,IIter first,IIter last) {
		 if (first == last) {
			 return;
		 }
		 const auto n = mystl::distance(first, last);
		 if ((cap_ - begin_) >= n) {
			 //如果备用空间大小足够
			 const auto after_elems = end_ - pos;
			 auto old_end = end_;
			 if (after_elems > n) {
				 end_ = mystl::uninitialized_copy(end_ - n, end_, end_);
				 mystl::move_backward(pos,old_end-n,old_end);
				 mystl::uninitialized_copy(first,last,pos);
			 }
			 else {
				 auto mid = first;
				 mystl::advance(mid,after_elems);
				 end_ = mystl::uninitialized_copy(mid, last, end_);
				 end_ = mystl::uninitialized_move(pos, old_end, end_);
				 mystl::uninitialized_copy(first,mid,pos);
			 }
		 }
		 else {
			 //备用空间不足
			 const auto new_size = get_new_cap(n);
			 auto new_begin = data_allocator::allocate(new_size);
			 auto new_end = new_begin;
			 try {
				 new_end = mystl::uninitialized_move(begin_, pos, new_begin);
				 new_end = mystl::uninitialized_copy(first, last, new_end);
				 new_end = mystl::uninitialized_move(pos, end_, new_end);
			 }
			 catch (...) {
				 destory_and_recover(new_begin, new_end, new_size);
				 throw;
			 }
			 data_allocator::deallocate(begin_, cap_ - begin_);
			 begin_ = new_begin;
			 end_ = new_end;
			 cap_ = begin_ + new_size;
		 }
	 }
	 //reinsert函数
	 template<typename T>
	 void vector<T>::reinsert(size_type size) {
		 auto new_begin = data_allocator::allocate(size);
		 try {
			 mystl::uninitialized_move(begin_, end_, new_begin);
		 }
		 catch (...) {
			 data_allocator::deallocate(begin_, cap_ - begin_);
			 begin_ = new_begin;
			 end_ = begin_ + size;
			 cap = begin_ + size;
		 }
	 }
	 //重载比较操作符
	 template<typename T>
	 bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
		 return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	 }
	 template<typename T>
	 bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
		 return std::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end())//检查字典序的函数
	 }
	 template<typename T>
	 bool operator!=(const vector<T>&lhs,const vector<T>&rhs) {
		 return !(lhs==rhs)
	 }
	 template<typename T>
	 bool operator>(const vector<T>&lhs,const vector<T>&rhs) {
		 return !(lhs<rhs)&&!(rhs==lhs)
	 }
	 template<typename T>
	 bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
		 return !(lhs > rhs);
	 }
	 template<typename T>
	 bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
		 return !(lhs < rhs);
	 }
	 //重载mystl的swap
	 template<typename T>
	 void swap(vector<T>&lhs,vector<T>&rhs) {
		 lhs.swap(rhs);
	 }
}

#endif // MYTINYSTL_VECTOR_H_
