#pragma once

template <class T, class Deleter = std::default_delete<T>>
class my_unique_ptr
{
public:
	my_unique_ptr(T* ptr = nullptr)
	{
		_ptr = ptr;
	}

	my_unique_ptr(T* ptr, Deleter deleter)
	{
		_ptr = ptr;
		_deleter = deleter;
	}

	my_unique_ptr(my_unique_ptr&& otherPtr) noexcept
	{
		_ptr = otherPtr._ptr;
		otherPtr._ptr = nullptr;
	}

	~my_unique_ptr()
	{
		if (_ptr == nullptr)
		{
			return;
		}
		_deleter(_ptr);
	}

	my_unique_ptr(const my_unique_ptr<T>&) = delete;

	T* get()
	{
		return _ptr;
	}

	Deleter getDeleter()
	{
		return _deleter;
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

	void swap(my_unique_ptr& otherPtr)
	{
		if (this == &otherPtr)
		{
			return;
		}
		my_unique_ptr<T, Deleter> temp = move(otherPtr);
		otherPtr = move(*this);
		*this = move(temp);
	}

	my_unique_ptr& operator=(const my_unique_ptr<T>&) = delete;

	my_unique_ptr& operator=(my_unique_ptr&& otherPtr) noexcept
	{
		if (this != &otherPtr)
		{
			delete _ptr;
			_ptr = otherPtr._ptr;
			otherPtr._ptr = nullptr;
			_deleter = otherPtr._deleter;
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
private:
	T* _ptr;
	Deleter _deleter;
};