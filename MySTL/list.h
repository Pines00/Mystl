#ifndef MYTINYSTL_LIST_H_
#define MYTINYSTL_LIST_H_
#include<initializer_list>
#include<memory.h>
//#include"function.h"
#include"util.h"
#include"exceptdef.h"
#include"iterator.h"
#include"allocator.h"
namespace mystl {
	template<class T>struct list_node_base;
	template<class T>struct list_node;
	template<class T>
	struct node_traits
	{
		typedef list_node<T>*  base_ptr;
		typedef list_node<T>*  node_ptr;
	};
	template<class T>
	struct list_node_base {
		typedef typename node_traits<T>::base_ptr base_ptr;
		typedef typename node_traits<T>::node_ptr node_ptr;
		base_ptr prev;
		base_ptr next;
		list_node_base() = default;
		node_ptr as_node() {
			return static_cast<node_ptr>(self());
		}
		void unlink() {
			prev = next = self();
		}
		base_ptr self() {
			return static_cast<base_ptr>(&*this);
		}
	};

	template<class T>
	struct list_node:public list_node_base<T>
	{
		typedef typename node_traits<T>::base_ptr base_ptr;
		typedef typename node_traits<T>::node_ptr node_ptr;
		T value;
		list_node() = default;
		list_node(const T& v) :value(v) {}
		list_node(T&& v):value(mystl::move(v)) {}
		base_ptr as_base() {
			return static_cast<base_ptr>(&*this);
		}
		node_ptr self() {
			return static_cast<node_ptr>(&*this);
		}
	};
	//迭代器list迭代器是双向的，所以继承bidirectional_iterator
	template<class T>
	struct list_iterator:public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;

		typedef typename node_traits<T>::base_ptr base_ptr;
		typedef typename node_traits<T>::node_ptr node_ptr;
		typedef list_iterator<T> self; 
		base_ptr node_;//用来指向当前节点

		//构造函数
		list_iterator() = default;
		list_iterator(base_ptr x):node_(x) {}
		list_iterator(node_ptr x) :node_(x->as_base()) {}
		list_iterator(const list_iterator& rhs) :node_(rhs.node_) {}
		//重载操作符
		reference operator*()const {
			return node_->as_node()->value;
		}
		pointer operator->()const { return &(operator*()); }
		self& operator++()const {//++i
			MYSTL_DEBUG(node_!=nullptr);
			node_ = node_->next;
			return *this;
		}
		self operator++(int) {//表示i++
			self tmp = *this;
			++*this;
			return tmp;
		}
		self& operator--() {//返回其引用类型
			MYSTL_DEBUG(node_!=nullptr);
			node_ = node_->prev;
			return *this;
		}
			/*return *this
			返回的是当前对象的克隆或者本身
			（若返回类型为A， 则是克隆， 若返回类型为A & ， 则是本身 ）
			。return this返回当前对象的地址（指向当前对象的指针）*/
		self operator--(int) {
			self temp = *this;
			--* this;
			return temp;
		}
		bool operator==(const self& rhs)const {
			return node_ == rhs.node_;
		}
		bool operator!=(cnost self& rhs)const {
			return !(node_ == rhs.node_);
		}
	};
	//模板类list
	template<class T>
	class list {
	public:
		typedef mystl::allocator<T>                      allocator_type;
		typedef mystl::allocator<T>                      data_allocator;
		typedef mystl::allocator<list_node_base<T>>      base_allocator;
		typedef mystl::allocator<list_node<T>>           node_allocator;

		typedef typename allocator_type::value_type      value_type;
		typedef typename allocator_type::pointer         pointer;
		typedef typename allocator_type::const_pointer   const_pointer;
		typedef typename allocator_type::reference       reference;
		typedef typename allocator_type::const_reference const_reference;
		typedef typename allocator_type::size_type       size_type;
		typedef typename allocator_type::difference_type difference_type;

		typedef list_iterator<T>                         iterator;
		/*typedef list_const_iterator<T>                   const_iterator;*/
		typedef mystl::reverse_iterator<iterator>        reverse_iterator;
		/*typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;*/

		typedef typename node_traits<T>::base_ptr        base_ptr;
		typedef typename node_traits<T>::node_ptr        node_ptr;

		allocator_type get_allocator() { return node_allocator(); }
	private:
		base_ptr node_;
		size_type size_;
	public:
		list() {
			fill_init(0, value_type());
		}
	};
}


#endif
