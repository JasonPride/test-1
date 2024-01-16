#define DEBUG

#include <iostream>
#include "my_unique_ptr.h"
#include "my_shared_ptr.h"
#include "TabWidget.h"

using namespace std;
using namespace smart_ptrs;

#ifdef DEBUG
bool test()
{
	bool allObjDestroyed = DEBUG_OBJ_CONSTRUCTOR == DEBUG_OBJ_DESTRUCTOR;
	bool allStrongRefsDestroyed = DEBUG_STRONG_REF_COUNT_CONSTRUCTOR == DEBUG_STRONG_REF_COUNT_DESTRUCTOR;
	bool allWeakRefsDestroyed = DEBUG_WEAK_REF_COUNT_CONSTRUCTOR == DEBUG_WEAK_REF_COUNT_DESTRUCTOR;
	cout << "All objects destroyed: " << allObjDestroyed << endl;
	cout << "All strong refs object destroyed: " << allStrongRefsDestroyed << endl;
	cout << "All weak refs object destroyed: " << allWeakRefsDestroyed << endl;

	DEBUG_OBJ_CONSTRUCTOR = DEBUG_OBJ_DESTRUCTOR 
		= DEBUG_STRONG_REF_COUNT_CONSTRUCTOR = DEBUG_STRONG_REF_COUNT_DESTRUCTOR 
		= DEBUG_WEAK_REF_COUNT_CONSTRUCTOR = DEBUG_WEAK_REF_COUNT_DESTRUCTOR = 0;
	return allObjDestroyed && allStrongRefsDestroyed && allWeakRefsDestroyed;
}
#endif // DEBUG


struct Pair
{
	int x;
	int y;
	int* pX;
	int* pY;
	Pair()
	{
		x = 0;
		y = 0;
		pX = pY = nullptr;
	}
	Pair(int x, int y)
	{
		this->x = x;
		this->y = y;
		pX = pY = nullptr;
	}
	Pair(const Pair& other)
	{
		x = other.x;
		y = other.y;
		pX = other.pX;
		pY = other.pY;
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

void testWeak()
{
	cout << "TESTING WEAK" << endl;
	shared_ptr<int> sharedInt = make_shared<int>(32);
	shared_ptr<int> sharedInt2 = make_shared<int>(30);
	weak_ptr<int> weakInt(sharedInt);
	cout << "Expired: " << weakInt.expired() << endl;
	shared_ptr<int> sharedWeak = weakInt.lock();
	shared_ptr<int> sharedWeak2 = weakInt.lock();
	sharedInt = sharedInt2;
	cout << "Expired: " << weakInt.expired() << endl;

	weakInt.reset();
	sharedWeak.reset();

	weak_ptr<int> weakInt2(sharedWeak2);
	cout << "Expired: " << weakInt2.expired() << endl;
	sharedWeak2.reset();
	cout << "Expired: " << weakInt2.expired() << endl;
	cout << "Expired: " << weakInt.expired() << endl;

	cout << "-------------------------------------------" << endl;

	my_shared_ptr<int> mySharedInt(new int(32));
	my_shared_ptr<int> mySharedInt2(new int(30));
	my_weak_ptr<int> myWeakInt(mySharedInt);
	cout << "Expired: " << myWeakInt.isExpired() << endl;
	my_shared_ptr<int> mySharedWeak = myWeakInt.lock();
	my_shared_ptr<int> mySharedWeak2 = myWeakInt.lock();
	mySharedInt = mySharedInt2;
	cout << "Expired: " << myWeakInt.isExpired() << endl;

	myWeakInt.reset();
	mySharedWeak.reset();

	my_weak_ptr<int> myWeakInt2(mySharedWeak2);
	cout << "Expired: " << myWeakInt2.isExpired() << endl;
	mySharedWeak2.reset();
	cout << "Expired: " << myWeakInt2.isExpired() << endl;
	cout << "Expired: " << myWeakInt.isExpired() << endl;

	cout << "END WEAK" << endl;
}

void testWidget()
{
	cout << "TESTING WIDGET" << endl;

	my_shared_ptr<Widget> parent(new TabWidget("Parent"));
	my_shared_ptr<Widget> testingWidget(new TabWidget("First child"));

	Widget::addChildToParent(parent, testingWidget);

	my_shared_ptr<Widget> parent1 = parent->getParent();
	my_shared_ptr<Widget> parent2 = testingWidget->getParent();

	cout << "END WIDGET" << endl;
}

int main()
{
	testUnique();
	cout << endl;
	cout << endl;

	testShared();
#ifdef DEBUG
	test();
#endif // DEBUG
	cout << endl;
	cout << endl;

	testWeak();
#ifdef DEBUG
	test();
#endif // DEBUG
	cout << endl;
	cout << endl;

	testWidget();
#ifdef DEBUG
	test();
#endif // DEBUG
}
