#pragma once
#include <iostream>
#include "Debug.h"

using namespace std;
namespace smart_ptrs
{
	template<class T>
	class my_shared_ptr;

	template <class _Ty0, class... _Types>
	my_shared_ptr<_Ty0> make_my_shared(_Types&&... args);

	template<class T>
	class my_weak_ptr
	{
	public:
		my_weak_ptr()
		{
			_sharedPtr = nullptr;
			_pControlBlock = nullptr;
		}
		my_weak_ptr(const my_weak_ptr<T>& ptr)
		{
			_sharedPtr = ptr._sharedPtr;
			_pControlBlock = ptr._pControlBlock;
			if (_pControlBlock)
			{
				_pControlBlock->weakCount.fetch_add(1);
			}
		}
		my_weak_ptr(my_shared_ptr<T>& ptr)
		{
			_sharedPtr = &ptr;
			_pControlBlock = ptr._pControlBlock;
			if (_pControlBlock)
			{
				_pControlBlock->weakCount.fetch_add(1);
			}
		}

		~my_weak_ptr()
		{
			if (!_pControlBlock)
			{
				return;
			}
			descruct();
		}

		bool isExpired() const
		{
			return !_pControlBlock || _pControlBlock->refCount.load() == 0;
		}

		my_shared_ptr<T> lock()
		{
			my_shared_ptr<T> tempPtr(*_sharedPtr);
			return tempPtr;
		}

		void reset()
		{
			if (!_pControlBlock)
			{
				return;
			}
			descruct(); 
			_pControlBlock = nullptr;
			_sharedPtr = nullptr;
		}

		long use_count() const
		{
			return _pControlBlock ? _pControlBlock->refCount.load() : 0;
		}

		my_weak_ptr<T>& operator=(my_weak_ptr<T>& otherPtr)
		{
			if (this == &otherPtr)
			{
				return *this;
			}
			_sharedPtr = otherPtr._sharedPtr;
			if (_pControlBlock)
			{
				descruct();
			}
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount.fetch_add(1);
			return *this;
		}

		my_weak_ptr<T>& operator=(my_weak_ptr<T>&& otherPtr) noexcept
		{
			if (this == &otherPtr)
			{
				return *this;
			}
			_sharedPtr = otherPtr._sharedPtr;
			otherPtr._sharedPtr = nullptr;
			if (_pControlBlock)
			{
				descruct();
			}
			_pControlBlock = otherPtr._pControlBlock;
			otherPtr._pControlBlock = nullptr;
			return *this;
		}


	private:
		my_shared_ptr<T>* _sharedPtr;
		struct my_shared_ptr<T>::ControlBlock* _pControlBlock;

		void descruct()
		{
			_pControlBlock->weakCount.fetch_sub(1);
			if (_pControlBlock->weakCount.load() == 0 && _pControlBlock->refCount.load() == 0)
			{
				delete _pControlBlock;
#ifdef DEBUG
				DEBUG_CONTROL_BLOCK_DESTRUCTOR++;
#endif // DEBUG
			}
		}
	};


	template<class T>
	class my_shared_ptr
	{
		friend class my_weak_ptr<T>; 
		
		template<class T2> friend class my_shared_ptr;
		template <class _Ty0, class... _Types>
		friend my_shared_ptr<_Ty0> make_my_shared(_Types&&... _Args);

	public:
		typedef void(*Deleter)(T*);

		my_shared_ptr()
		{
			_ptr = nullptr;
			_pControlBlock = nullptr;
		}

		my_shared_ptr(T* ptr)
		{
			_ptr = ptr;
			_pControlBlock = new SeparateControlBlock(ptr);
#ifdef DEBUG
			DEBUG_OBJ_CONSTRUCTOR++;
			DEBUG_CONTROL_BLOCK_CONSTRUCTOR++;
#endif // DEBUG

		}

		my_shared_ptr(T* ptr, Deleter deleter)
		{
			_ptr = ptr;
			_pControlBlock = new SeparateControlBlock(ptr, deleter);
#ifdef DEBUG
			DEBUG_OBJ_CONSTRUCTOR++; 
			DEBUG_CONTROL_BLOCK_CONSTRUCTOR++;
#endif // DEBUG
		}

		my_shared_ptr(const my_shared_ptr<T>& otherPtr)
		{
			_ptr = otherPtr._ptr; 
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount.fetch_add(1);
		}

		my_shared_ptr(my_shared_ptr<T>&& otherPtr) noexcept
		{
			_pControlBlock = otherPtr._pControlBlock;
			otherPtr._pControlBlock = nullptr;
			_ptr = otherPtr._ptr;
			otherPtr._ptr = nullptr;
		}

		template<class T2, enable_if_t<_SP_pointer_compatible<T2, T>::value, int> = 0>
		my_shared_ptr(const my_shared_ptr<T2>& otherPtr)
		{
			_ptr = otherPtr._ptr; 
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount.fetch_add(1);
		}

		template<class T2, enable_if_t<_SP_pointer_compatible<T2, T>::value, int> = 0>
		my_shared_ptr(my_shared_ptr<T2>&& otherPtr) noexcept
		{
			_pControlBlock = otherPtr._pControlBlock;
			otherPtr._pControlBlock = nullptr;
			_ptr = otherPtr._ptr;
			otherPtr._ptr = nullptr;
		}

		~my_shared_ptr()
		{
			if (!_pControlBlock)
			{
				return;
			}
			if (_pControlBlock->refCount.load() == 1)
			{
				destruct();
			}
			else
			{
				_pControlBlock->refCount.fetch_sub(1);
			}
		}

		template<class T2, enable_if_t<_SP_pointer_compatible<T2, T>::value, int> = 0>
		my_shared_ptr<T>& operator=(const my_shared_ptr<T2>& otherPtr)
		{
			my_shared_ptr(otherPtr).swap(*this);
			return *this;
		}

		template<class T2>
		my_shared_ptr<T>& operator=(my_shared_ptr<T2>&& otherPtr) noexcept
		{
			my_shared_ptr(move(otherPtr)).swap(*this);
			return *this;
		}

		my_shared_ptr<T>& operator=(my_shared_ptr<T>& otherPtr)
		{
			if (this == &otherPtr)
			{
				return *this;
			}
			if (_pControlBlock)
			{
				if (_pControlBlock->refCount.load() == 1)
				{
					destruct();
				}
				else
				{
					_pControlBlock->refCount.fetch_sub(1);
				}
			}
			_ptr = otherPtr._ptr; 
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount.fetch_add(1);
			return *this;
		}

		my_shared_ptr<T>& operator=(my_shared_ptr<T>&& otherPtr) noexcept
		{
			if (this == &otherPtr)
			{
				return *this;
			}
			if (_pControlBlock)
			{
				if (_pControlBlock->refCount.load() == 1)
				{
					destruct();
				}
				else
				{
					_pControlBlock->refCount.fetch_sub(1);
				}
			}
			_ptr = otherPtr._ptr;
			otherPtr._ptr = nullptr;
			_pControlBlock = otherPtr._pControlBlock;
			otherPtr._pControlBlock = nullptr;

			return *this;
		}

		T& operator[](int i)
		{
			return _ptr[i];
		}

		T& operator*()
		{
			return *_ptr;
		}

		T* operator->()
		{
			return _ptr;
		}

		long use_count() const
		{
			return (_pControlBlock) ? _pControlBlock->refCount.load() : 0L;
		}

		bool unique() const
		{
			(_pControlBlock) && (_pControlBlock->refCount.load() == 1);
		}
		void reset()
		{
			if (_pControlBlock->refCount.load() == 1)
			{
				destruct();
			}
			else
			{
				_pControlBlock->refCount.fetch_sub(1);
			}
			_pControlBlock = nullptr;
			_ptr = nullptr;
		}
		void swap(my_shared_ptr ptr)
		{
			std::swap(*this, ptr);
		}

	private:
		struct ControlBlock;

		T* _ptr = nullptr;
		ControlBlock* _pControlBlock = nullptr;

		void destruct()
		{
			if (!_pControlBlock)
			{
				return;
			}
			_pControlBlock->dispose();
			if (_pControlBlock->weakCount.load() <= 0)
			{
				delete _pControlBlock;
#ifdef DEBUG
				DEBUG_CONTROL_BLOCK_DESTRUCTOR++;
#endif // DEBUG
			}
			else
			{
				_pControlBlock->refCount.fetch_sub(1);
			}
			_pControlBlock = nullptr;
#ifdef DEBUG
			DEBUG_OBJ_DESTRUCTOR++;
#endif // DEBUG
		}

		static void defaultDeleter(T* ptr)
		{
			delete ptr;
		}

		struct ControlBlock
		{
			virtual void dispose() = 0;
			std::atomic<unsigned long> refCount = { 1L };
			std::atomic<unsigned long> weakCount = { 0L };

			ControlBlock& operator=(const ControlBlock& other)
			{

			}
		};

		struct SeparateControlBlock : ControlBlock
		{
			SeparateControlBlock(T* ptr) : ControlBlock()
			{
				this->ptr = ptr;
				deleter = defaultDeleter;
			}

			SeparateControlBlock(T* ptr, Deleter deleter) : ControlBlock()
			{
				this->ptr = ptr;
				this->deleter = deleter;
			}

			virtual void dispose() noexcept override
			{
				deleter(ptr);
			}

			
			Deleter deleter;
			T* ptr;
		};

		struct CombinedControlBlock : ControlBlock
		{
			virtual void dispose() noexcept override
			{
				ptr()->~T();
			}

			T* ptr()
			{
				return reinterpret_cast<T*>(buffer);
			}

			alignas(T) char buffer[sizeof(T)];
		};
	};


	template <class _Ty0, class... _Types>
	my_shared_ptr<_Ty0> make_my_shared(_Types&&... args)
	{
		my_shared_ptr<_Ty0> ret;
		struct my_shared_ptr<_Ty0>::CombinedControlBlock* pControlBlock = new struct my_shared_ptr<_Ty0>::CombinedControlBlock;
		new (pControlBlock->buffer) _Ty0(args...);
		ret._pControlBlock = pControlBlock;
		ret._ptr = pControlBlock->ptr();
#ifdef DEBUG
		DEBUG_OBJ_CONSTRUCTOR++;
		DEBUG_CONTROL_BLOCK_CONSTRUCTOR++;
#endif // DEBUG
		return ret;
	}

}

