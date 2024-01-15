#include "Widget.h"

void Widget::addChild(my_shared_ptr<Widget>& child)
{
	_children.push_back(child);
}

int Widget::getChildrenCount()
{
	return _children.size();
}

my_shared_ptr<Widget> Widget::getParent()
{
	if (_parent.use_count() == 0)
	{
		return my_shared_ptr<Widget>();
	}
	return _parent.lock();
}