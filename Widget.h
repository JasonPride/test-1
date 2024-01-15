#pragma once
#include <vector>
#include "my_shared_ptr.h"
class Widget
{
public:
	enum WidgetType
	{
		TabWidget,
		CalendarWidget
	};

	Widget()
	{
	}

	Widget(my_shared_ptr<Widget>& parent)
	{
		/*my_weak_ptr<Widget> temp(parent)*/
		_parent = my_weak_ptr<Widget>(parent);
		//parent->addChild(my_shared_ptr<Widget>(this));
	}
	int getChildrenCount();
	my_shared_ptr<Widget> getParent();

	virtual WidgetType getType() = 0;
private:
	std::vector<my_shared_ptr<Widget>> _children;
	my_weak_ptr<Widget> _parent;
	void addChild(my_shared_ptr<Widget>& child);
	void addChild(Widget* child);
};