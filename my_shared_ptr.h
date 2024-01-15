#pragma once
#include <iostream>


template<class T, class Deleter = std::default_delete<T>>
class my_shared_ptr;


template<class T>
class my_weak_ptr
{
public:
	my_weak_ptr()
	{
		_sharedPtr = nullptr;
		_count = nullptr;
		_weakCountShared = nullptr;
	}
	my_weak_ptr(const my_weak_ptr<T>& ptr)
	{
		_sharedPtr = ptr._sharedPtr;
		_count = ptr._count;
		_weakCountShared = ptr._weakCountShared;
		if (_weakCountShared)
		{
			(*_weakCountShared)++;
		}
	}
	my_weak_ptr(my_shared_ptr<T>& ptr)
	{
		_sharedPtr = &ptr;
		_count = ptr._count;
		_weakCountShared = ptr._weakCount;
		if (_weakCountShared)
		{
			(*_weakCountShared)++;
		}
	}

	~my_weak_ptr()
	{
		if (!_weakCountShared || !_count)
		{
			return;
		}
		descruct();
	}

	bool isExpired() const
	{
		return !_count || *_count == 0;
	}

	my_shared_ptr<T> lock()
	{
		my_shared_ptr<T> tempPtr(*_sharedPtr);
		return tempPtr;
	}

	void reset()
	{
		if (!_weakCountShared || !_count)
		{
			return;
		}
		descruct();
		_count = nullptr;
		_weakCountShared = nullptr;
		_sharedPtr = nullptr;
	}

	long use_count() const
	{
		return (_count) ? *_count : 0;
	}

	my_weak_ptr<T>& operator=(my_weak_ptr<T>& otherPtr)
	{
		if (this == &otherPtr)
		{
			return *this;
		}
		_sharedPtr = otherPtr._sharedPtr;
		if (_weakCountShared && _count)
		{
			descruct();
		}
		_count = otherPtr._count;
		_weakCountShared = otherPtr._weakCountShared;
		(*_weakCountShared)++;
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
		if (_weakCountShared && _count)
		{
			descruct();
		}
		_count = otherPtr._count;
		otherPtr._count = nullptr;
		_weakCountShared = otherPtr._weakCountShared;
		otherPtr._weakCountShared = nullptr;
		return *this;
	}


private:
	my_shared_ptr<T>* _sharedPtr;
	long* _count;
	long* _weakCountShared;

	void descruct()
	{
		(*_weakCountShared)--;
		if (*_weakCountShared == 0 && *_count == 0)
		{
			std::cout << "Weak destructor" << std::endl;
			delete _weakCountShared;
			delete _count;
		}
	}
};


template<class T, class Deleter>
class my_shared_ptr 
{
	friend class my_weak_ptr<T>;
public:
	
	my_shared_ptr()
	{
		_ptr = nullptr;
		_count = nullptr;
		_weakCount = nullptr;
	}

	my_shared_ptr(T* ptr)
	{
		_ptr = ptr;
		_count = new long(1);
		_weakCount = new long(0);
	}

	my_shared_ptr(T* ptr, Deleter deleter)
	{
		_ptr = ptr;
		_count = new long(1);
		_weakCount = new long(0);
		_deleter = deleter;
	}

	my_shared_ptr(const my_shared_ptr<T>& otherPtr)
	{
		_count = otherPtr._count;
		_weakCount = otherPtr._weakCount;
		_ptr = otherPtr._ptr;
		_deleter = otherPtr._deleter;
		(*_count)++;
	}

	~my_shared_ptr()
	{
		if (!_count)
		{
			return;
		}
		if (*_count == 1)
		{
			destruct();
		}
		else
		{
			(*_count)--;
		}
	}

	my_shared_ptr<T>& operator=(my_shared_ptr<T>& otherPtr)
	{
		if (this == &otherPtr)
		{
			return *this;
		}
		if (_count)
		{
			if (*_count == 1)
			{
				destruct();
			}
			else
			{
				(*_count)--;
			}
		}
		_ptr = otherPtr._ptr;
		_count = otherPtr._count;
		_weakCount = otherPtr._weakCount;
		(*_count)++;
		return *this;
	}

	my_shared_ptr<T>& operator=(my_shared_ptr<T>&& otherPtr) noexcept
	{
		if (this == &otherPtr)
		{
			return *this;
		}
		if (_count)
		{
			if (*_count == 1)
			{
				destruct();
			}
			else
			{
				(*_count)--;
			}
		}

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
		return (_count) ? * _count : 0;
	}

	bool unique() const
	{
		(_count) && (*_count == 1);
	}
	void reset()
	{
		if (*_count == 1)
		{
			destruct();
		}
		else
		{
			(*_count)--;
		}
		_count = nullptr;
		_ptr = nullptr;
		_weakCount = nullptr;
	}
	void swap(my_shared_ptr ptr)
	{
		std::swap(*this, ptr);
	}

private:
	long* _count;
	long* _weakCount;
	T* _ptr;
	Deleter _deleter;

	void destruct()
	{
		std::cout << "Destructor" << std::endl;
		if (_weakCount == nullptr)
		{
			delete _count;
		}
		else if (*_weakCount <= 0)
		{
			delete _count;
			delete _weakCount;
		}
		else
		{
			(*_count)--;
		}
		_count = nullptr;
		_weakCount = nullptr;
		_deleter(_ptr);
	}
};

