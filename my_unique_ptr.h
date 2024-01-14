#pragma once
#include <iostream>

template <class T>
class my_unique_ptr
{
public:
	my_unique_ptr(T* ptr = nullptr)
	{
		std::cout << "Constructor" << std::endl;
		_ptr = ptr;
	}
	my_unique_ptr(my_unique_ptr&& otherPtr) noexcept
	{
		std::cout << "Move constructor" << std::endl;
		_ptr = otherPtr._ptr;
		otherPtr._ptr = nullptr;
	}
	~my_unique_ptr()
	{
		std::cout << "Destructor" << std::endl;
		if (_ptr == nullptr)
		{
			return;
		}
		delete _ptr;
	}
	my_unique_ptr(const my_unique_ptr<T>&) = delete;
	my_unique_ptr& operator=(const my_unique_ptr<T>&) = delete;
	my_unique_ptr& operator=(my_unique_ptr&& otherPtr) noexcept
	{
		if (this != &otherPtr)
		{
			delete _ptr;
			_ptr = otherPtr._ptr;
			otherPtr._ptr = nullptr;
		}
		return *this;
	}
	T& operator*()
	{
		return *_ptr;
	}
	T* get()
	{
		return _ptr;
	}
	T* release()
	{
		T* tempPtr = _ptr;
		_ptr = nullptr;
		return tempPtr;
	}
	void reset(T* ptr = nullptr)
	{
		delete _ptr;
		_ptr = ptr;
	}
	T* operator->()
	{
		return _ptr;
	}
private:
	T* _ptr;
};