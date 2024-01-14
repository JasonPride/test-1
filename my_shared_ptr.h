#pragma once
#include <iostream>

template<class T, class Deleter = std::default_delete<T>>
class my_shared_ptr 
{
public:
	
	my_shared_ptr()
	{
		_ptr = nullptr;
		_count = nullptr;
	}

	my_shared_ptr(T* ptr)
	{
		_ptr = ptr;
		_count = new long(1);
	}

	my_shared_ptr(T* ptr, Deleter deleter)
	{
		_ptr = ptr;
		_count = new long(1);
		_deleter = deleter;
	}

	my_shared_ptr(const my_shared_ptr<T>& otherPtr)
	{
		_count = otherPtr._count;
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
			std::cout << "Destructor" << std::endl;
			delete _count;
			_deleter(_ptr);
		}
		else
		{
			(*_count)--;
		}
	}

	my_shared_ptr<T> operator=(my_shared_ptr<T>& otherPtr)
	{
		if (this == &otherPtr)
		{
			return *this;
		}
		if (_count)
		{
			if (*_count == 1)
			{
				std::cout << "Destructor" << std::endl;
				delete _count;
				_deleter(_ptr);
			}
			else
			{
				(*_count)--;
			}
		}
		_ptr = otherPtr._ptr;
		_count = otherPtr._count;
		(*_count)++;
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

private:
	long* _count;
	T* _ptr;
	Deleter _deleter;
};