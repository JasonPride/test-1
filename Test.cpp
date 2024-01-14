#include <iostream>
#include "my_unique_ptr.h"

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
	cout << "Testing Unique" << endl;
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
}

void testShared()
{
	shared_ptr<int> p_shared = make_shared<int>(123);
	shared_ptr<int>* p_shared3 = new shared_ptr<int>(new int[10], [](int* ptr) {
		delete[]ptr;
		});

	shared_ptr<int> p_shared2 = p_shared;
	int count1 = p_shared.use_count();
	int count2 = p_shared.use_count();
	cout << "Count of refs 1" << count1 << endl;
	cout << "Count of refs 2" << count2 << endl;
	p_shared2 = nullptr;
	count1 = p_shared.use_count();
	cout << "Count of refs 1" << count1 << endl;
}

int main()
{
	testUnique();
}
