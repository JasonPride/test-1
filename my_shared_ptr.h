#pragma once
#include <iostream>
#include "Debug.h"
namespace smart_ptrs
{
	template<class T>
	class my_shared_ptr;

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
				_pControlBlock->weakCount++;
			}
		}
		my_weak_ptr(my_shared_ptr<T>& ptr)
		{
			_sharedPtr = &ptr;
			_pControlBlock = ptr._pControlBlock;
			if (_pControlBlock)
			{
				_pControlBlock->weakCount++;
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
			return !_pControlBlock || _pControlBlock->refCount == 0;
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
			return _pControlBlock ? _pControlBlock->refCount : 0;
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
			_pControlBlock->refCount++;
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
			(_pControlBlock->weakCount)--;
			if (_pControlBlock->weakCount == 0 && _pControlBlock->refCount == 0)
			{
				delete _pControlBlock;
#ifdef DEBUG
				DEBUG_STRONG_REF_COUNT_DESTRUCTOR++;
				DEBUG_WEAK_REF_COUNT_DESTRUCTOR++;
#endif // DEBUG
			}
		}
	};


	template<class T>
	class my_shared_ptr
	{
		friend class my_weak_ptr<T>;
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
			_pControlBlock = new ControlBlock;
#ifdef DEBUG
			DEBUG_OBJ_CONSTRUCTOR++;
			DEBUG_STRONG_REF_COUNT_CONSTRUCTOR++;
			DEBUG_WEAK_REF_COUNT_CONSTRUCTOR++;
#endif // DEBUG

		}

		my_shared_ptr(T* ptr, Deleter deleter)
		{
			_ptr = ptr;
			_pControlBlock = new ControlBlock(deleter);
#ifdef DEBUG
			DEBUG_OBJ_CONSTRUCTOR++;
			DEBUG_STRONG_REF_COUNT_CONSTRUCTOR++;
			DEBUG_WEAK_REF_COUNT_CONSTRUCTOR++;
#endif // DEBUG
		}

		my_shared_ptr(const my_shared_ptr<T>& otherPtr)
		{
			_ptr = otherPtr._ptr; 
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount++;
		}

		my_shared_ptr(my_shared_ptr<T>&& otherPtr) noexcept
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
			if (_pControlBlock->refCount == 1)
			{
				destruct();
			}
			else
			{
				_pControlBlock->refCount--;
			}
		}

		my_shared_ptr<T>& operator=(my_shared_ptr<T>& otherPtr)
		{
			if (this == &otherPtr)
			{
				return *this;
			}
			if (_pControlBlock)
			{
				if (_pControlBlock->refCount == 1)
				{
					destruct();
				}
				else
				{
					_pControlBlock->refCount--;
				}
			}
			_ptr = otherPtr._ptr; 
			_pControlBlock = otherPtr._pControlBlock;
			_pControlBlock->refCount++;
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
				if (_pControlBlock->refCount == 1)
				{
					destruct();
				}
				else
				{
					_pControlBlock->refCount--;
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
			return (_pControlBlock) ? _pControlBlock->refCount : 0L;
		}

		bool unique() const
		{
			(_pControlBlock) && (_pControlBlock->refCount == 1);
		}
		void reset()
		{
			if (_pControlBlock->refCount == 1)
			{
				destruct();
			}
			else
			{
				_pControlBlock->refCount--;
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
			_pControlBlock->deleter(_ptr);
			if (_pControlBlock->weakCount <= 0)
			{
				delete _pControlBlock;
#ifdef DEBUG
				DEBUG_STRONG_REF_COUNT_DESTRUCTOR++;
				DEBUG_WEAK_REF_COUNT_DESTRUCTOR++;
#endif // DEBUG
			}
			else
			{
				_pControlBlock->refCount--;
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
			ControlBlock() 
			{
				deleter = defaultDeleter;
			}
			ControlBlock(Deleter aDeleter)
			{
				deleter = aDeleter;
			}
			unsigned long refCount = 1L;
			unsigned long weakCount = 0L;
			Deleter deleter;
		};
	};

}

