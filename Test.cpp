#include <iostream>
#include "my_unique_ptr.h"
#include "my_shared_ptr.h"

using namespace std;

struct Pair
{
	int x;
	int y;
	Pair()
	{
		x = 0;
		y = 0;
	}
	Pair(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

void testUnique()
{
	cout << "TESTING UNIQUE" << endl;
	my_unique_ptr<int> pMyUnique(new int(30));
	my_unique_ptr<int> pMyUnique2 = (new int(20));
	const int ARR_LENGTH = 15;
	my_unique_ptr<int, void(*)(int*)> pMyUniqueArr(new int[ARR_LENGTH], [](int* arr)
		{
			delete[] arr;
		});
	cout << *pMyUnique2 << endl;
	*pMyUnique2 = 24;
	cout << *pMyUnique2 << endl;

	my_unique_ptr<Pair> pPair(new Pair(4, 6));
	cout << "x: " << pPair->x << "; y = " << pPair->y << endl;

	cout << "Unique array: " << endl;
	for (int i = 0; i < ARR_LENGTH; i++)
	{
		pMyUniqueArr[i] = i;
		cout << pMyUniqueArr[i] << " ";
	}
	cout << endl;
	
	cout << "Before swapping" << endl;
	cout << "1: " << *pMyUnique << ", 2: " << *pMyUnique2 << endl;
	pMyUnique.swap(pMyUnique2);
	cout << "After swapping" << endl;
	cout << "1: " << *pMyUnique << ", 2: " << *pMyUnique2 << endl;
	cout << "END UNIQUE" << endl;
}

void testShared()
{
	cout << "TESTING SHARED" << endl;
	shared_ptr<int> p_shared = make_shared<int>(123);
	
	my_shared_ptr<int> pShared1(new int(10));
	my_shared_ptr<int> pShared2(pShared1);
	my_shared_ptr<int> pShared3(new int(20));
	const int ARR_LENGTH = 15;
	my_shared_ptr<int, void(*)(int*)> pSharedArr(new int[ARR_LENGTH], [](int* ptr)
		{
			delete[]ptr;
		});

	cout << "Ref count: 1 - " + pShared1.use_count() 
		<< ", 2 - " << pShared2.use_count() 
		<< ", 3 - " << pShared3.use_count() << endl;

	pShared1 = pShared3;

	cout << "Ref count: 1 - " + pShared1.use_count()
		<< ", 2 - " << pShared2.use_count()
		<< ", 3 - " << pShared3.use_count() << endl;


	pShared2 = pShared1;

	for (int i = 0; i < ARR_LENGTH; i++)
	{
		pSharedArr[i] = i;
		cout << pSharedArr[i] << " ";
	}
	cout << endl;
	cout << "END SHARED" << endl;
}

int main()
{
	testUnique();
	cout << endl;
	testShared();
	cout << endl;
}
