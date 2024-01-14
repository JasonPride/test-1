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

int main()
{
	unique_ptr<int[]>p_unique = make_unique<int[]>(12);

	my_unique_ptr<int> pMyUnique(new int(30));
	my_unique_ptr<int> pMyUnique2 = move(pMyUnique);
	cout << *pMyUnique2 << endl;
	*pMyUnique2 = 24;
	cout << *pMyUnique2 << endl;

	pMyUnique = move(pMyUnique2);

	my_unique_ptr<Pair> pPair(new Pair(4, 6));
	cout << "x: " << pPair->x << "; y = " << pPair->y << endl;
}
