/*!
	\file
	\brief
		Definitions for common smart pointers.

	\author
		Alexx
*/

#ifndef WHEE_HDR_SMARTPTR_HPP__
#define WHEE_HDR_SMARTPTR_HPP__

///////////////////////////////////////////////////////////////////////////////

//#ifdef WHEE_INCLUDE_STD_HEADERS
#include <cassert>
#include <memory>
//#endif	// #ifdef WHEE_INCLUDE_STD_HEADERS

///////////////////////////////////////////////////////////////////////////////

_BEGIN_NAMESPACE_

	///////////////////////////////////////////////////////////////////////////

	template < typename T > 
		class scoped_ptr;

	template < typename T > 
		class scoped_array;

	template < typename T > 
		class shared_ptr;

	///////////////////////////////////////////////////////////////////////////



	/*************************************************************************/
	/*!
		Automated scoped pointer.

		\remarks
			The class mimics a built-in pointer except that it guarantees 
			deletion of the object pointed to (delete operator is used), 
			either on destruction of the scoped_ptr or via an explicit reset(). 
			scoped_ptr is a simple solution for simple needs.

			\par
			The class is an analog of \c boost::scoped_ptr<>.

		\note
			The class allows implicit conversion to raw pointer type.

		\sa
			scoped_array, shared_ptr

		\author
			Alexander Bogarsukov

		\version
			1.3
	*/
	template 
	<
		typename T
	> 
	class scoped_ptr
	{
		/* typedefs */
	public:
		typedef T element_type;

		/* construction/copying */
	public:
		scoped_ptr(T * ptr = 0)
			:	_m_ptr(ptr)
			{
			}

		explicit scoped_ptr(std::auto_ptr<T> & ptr)
			:	_m_ptr(ptr.release())
			{
			}

		~scoped_ptr()
			{
				delete _m_ptr;
			}

		scoped_ptr & operator =(T * ptr)
			{
				reset(ptr);
				return *this;
			}

	private:
		// not defined
		scoped_ptr(const scoped_ptr &);
		scoped_ptr & operator =(const scoped_ptr &);

		/* interface */
	public:
		void reset(T * ptr = 0)
			{
				assert(ptr == 0 || ptr != _m_ptr);

				if (ptr != _m_ptr)
				{
					scoped_ptr(ptr).swap(*this);
				}
			}

		T * release()
			{	// NOTE: like std::auto_ptr<> just resets stored pointer to NULL, delete operator is not called
				T * tmp = _m_ptr;
				_m_ptr = 0;
				return tmp;
			}

		void swap(scoped_ptr & other)
			{
				T * tmp = other._m_ptr;
				other._m_ptr = _m_ptr;
				_m_ptr = tmp;
			}

		T * get() const
			{
				return _m_ptr;
			}

	public:
		T * operator ->() const
			{
				assert(_m_ptr != 0);
				return _m_ptr;
			}

		T & operator *() const
			{
				assert(_m_ptr != 0);
				return *_m_ptr;
			}

		bool operator !() const
			{
				return (_m_ptr == 0);
			}

//		operator bool () const
//			{
//				return (_m_ptr != 0);
//			}

		operator T *() const
			{	// WARNING: use with care, may cause unexpected results in some cases
				return _m_ptr;
			}

		/* implementation */
	private:
		T * _m_ptr;
	};

	///////////////////////////////////////////////////////////////////////////



	/*************************************************************************/
	/*!
		Automated scoped array pointer.

		\remarks
			scoped_array extends scoped_ptr to arrays. Deletion of the array 
			pointed to is guaranteed (delete[] operator is used), either on 
			destruction of the scoped_array or via an explicit reset().

			\par
			The class is an analog of \c boost::scoped_array<>.

			\par
			The class allows implicit conversion to raw pointer type.

		\sa
			scoped_ptr, shared_ptr

		\author
			Alexander Bogarsukov

		\version
			1.3
	*/
	template 
	<
		typename T
	> 
	class scoped_array
	{
		/* typedefs */
	public:
		typedef T element_type;

		/* construction/copying */
	public:
		scoped_array(T * ptr = 0)
			:	_m_ptr(ptr)
			{
			}

		~scoped_array()
			{
				delete[] _m_ptr;
			}

		scoped_array & operator =(T * ptr)
			{
				reset(ptr);
				return *this;
			}

	private:
		// not defined
		scoped_array(const scoped_array &);
		scoped_array & operator =(const scoped_array &);

		/* interface */
	public:
		void reset(T * ptr = 0)
			{
				assert(ptr == 0 || ptr != _m_ptr);

				if (ptr != _m_ptr)
				{
					scoped_array(ptr).swap(*this);
				}
			}

		T * release()
			{	// NOTE: like std::auto_ptr<> just resets stored pointer to NULL, delete[] operator is not called
				T * tmp = _m_ptr;
				_m_ptr = 0;
				return tmp;
			}

		void swap(scoped_array & other)
			{
				T * tmp = other._m_ptr;
				other._m_ptr = _m_ptr;
				_m_ptr = tmp;
			}

		T * get() const
			{
				return _m_ptr;
			}

	public:
		T & operator [](size_t index) const
			{	// WARNING: no range check
				assert(_m_ptr != 0);
				return _m_ptr[index];
			}

		bool operator !() const
			{
				return (_m_ptr == 0);
			}

//		operator bool () const
//			{
//				return (_m_ptr != 0);
//			}

		operator T *() const
			{	// WARNING: use with care; may cause unexpected results in some cases
				return _m_ptr;
			}

		/* implementation */
	private:
		T * _m_ptr;
	};

	///////////////////////////////////////////////////////////////////////////



	/*************************************************************************/
	/*!
		\brief
			A shared pointer.

		\remarks
			By default the pointer manages reference counter stored by the object 
			pointed to. It increases its value when copied and decreases when 
			released or destroyed. The objects pointed to is destroyed using 
			operator delete when its reference counter is null.

			\par
			The class assumes that target object has methods \c add_ref() and 
			\c release() for reference counter increment/decrement actions. 
			Note that the pointer class is responsible for destroying the object, 
			this way it cannot be used with COM-like objects.

			\par
			The implementation is much like the boost::shared_ptr<>.

		\note
			The class doesn't allow an implicit conversion to a raw pointer type.

		\sa
			scoped_ptr, scoped_array

		\author
			Alexander Bogarsukov

		\version
			1.5
	*/
	template 
	<
		typename T
	>
	class shared_ptr
	{
		/* typedefs */
	public:
		typedef T element_type;

		/* construction/copying */
	public:
		shared_ptr()
			:	_m_ptr(0)
			{
			}

		shared_ptr(T * ptr)
			:	_m_ptr(clone(ptr))
			{
			}

		shared_ptr(const shared_ptr & other)
			:	_m_ptr(clone(other._m_ptr))
			{
			}

		template < typename U > 
			shared_ptr(const shared_ptr<U> & other)
			:	_m_ptr(clone(other.get()))
			{
			}

		~shared_ptr()
			{
				release(_m_ptr);
			}

		shared_ptr & operator =(T * ptr)
			{
				shared_ptr(ptr).swap(*this);
				return *this;
			}

		shared_ptr & operator =(const shared_ptr & other)
			{
				shared_ptr(other._m_ptr).swap(*this);
				return *this;
			}
		
		template < typename U > 
			shared_ptr & operator =(const shared_ptr<U> & other)
			{
				shared_ptr(other.get()).swap(*this);
				return *this;
			}

		/* interface */
	public:
		void reset()
			{
				shared_ptr().swap(*this);
			}

		template < typename U > 
			void reset(U * ptr)
			{
				assert(ptr == 0 || ptr != _m_ptr);
				shared_ptr(ptr).swap(*this);
			}

		void swap(shared_ptr & other)
			{
				T * tmp = other._m_ptr;
				other._m_ptr = _m_ptr;
				_m_ptr = tmp;
			}

		T * get() const
			{
				return _m_ptr;
			}

	public:
		T * operator ->() const
			{
				assert(_m_ptr != 0);
				return _m_ptr;
			}

		T & operator *() const
			{
				assert(_m_ptr != 0);
				return *_m_ptr;
			}

		bool operator !() const
			{
				return (_m_ptr == 0);
			}

		operator bool () const
			{
				return (_m_ptr != 0);
			}

		/* implementation */
	private:
		static T * clone(T * ptr)
			{
				if (ptr)
				{
					ptr->add_ref();
				}

				return ptr;
			}

		static void release(T * ptr)
			{
				if (ptr && ptr->release())
				{
					delete ptr;
				}
			}

	private:
		T * _m_ptr;
	};

	///////////////////////////////////////////////////////////////////////////

	template < class T, class U > 
		inline bool operator ==(const shared_ptr<T> & a, const shared_ptr<U> & b)
		{
			return a.get() == b.get();
		}

	template < class T, class U > 
		inline bool operator ==(const shared_ptr<T> & a, const U * b)
		{
			return a.get() == b;
		}

	template < class T, class U > 
		inline bool operator ==(const T * a, const shared_ptr<U> & b)
		{
			return a == b.get();
		}

	template < class T, class U > 
		inline bool operator !=(const shared_ptr<T> & a, const shared_ptr<U> & b)
		{
			return a.get() != b.get();
		}

	template < class T, class U > 
		inline bool operator !=(const shared_ptr<T> & a, const U * b)
		{
			return a.get() == b;
		}

	template < class T, class U > 
		inline bool operator !=(const T * a, const shared_ptr<U> & b)
		{
			return a == b.get();
		}

	///////////////////////////////////////////////////////////////////////////

	template < typename U, typename T > 
		inline shared_ptr<U> shared_ptr_scast(const shared_ptr<T> & ptr)
		{
			return shared_ptr<U>(static_cast<U*>(ptr.get()));
		}

	template < typename U, typename T > 
		inline shared_ptr<U> shared_ptr_dcast(const shared_ptr<T> & ptr)
		{
			return shared_ptr<U>(dynamic_cast<U*>(ptr.get()));
		}

	///////////////////////////////////////////////////////////////////////////



	/*************************************************************************/
	/*!
		Reference counter.

		\remarks
			The class represents an object with internal reference counter. 
			Any class that is intended to use reference counting should be 
			derived from this one.
			
			\par
			The instances of the class should be used with shared_ptr class 
			for safe references manipulations.

			\par
			The class is not intended to be used in multithread environment. 
			None of its methods are thread-safe.

		\sa
			shared_ptr

		\author
			Alexander Bogarsukov

		\version
			1.3
	*/
	class ref_counter
	{
		/* construction/copying */
	protected:
		ref_counter()
			:	_m_ref_counter(0)
			{
			}

		~ref_counter() throw()
			{
			}

		/* interface */
	public:
		void add_ref() throw()
			{
				++_m_ref_counter;
			}

		bool release() throw()
			{
				/*	NOTE: The object DOES NOT destroy itself when its reference counter 
					becomes null. Instead the method just returns 'true' to indicate 
					that no references are left.
				*/
				assert(_m_ref_counter > 0);
				return (--_m_ref_counter == 0);
			}

		bool is_referenced() const throw()
			{
				return (_m_ref_counter > 0);
			}

		/* implementation */
	private:
		long _m_ref_counter;
	};

	///////////////////////////////////////////////////////////////////////////

_END_NAMESPACE_

///////////////////////////////////////////////////////////////////////////////

#endif // WHEE_HDR_SMARTPTR_HPP__
