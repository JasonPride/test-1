#pragma once
#include <vector>
#include "my_shared_ptr.h"

using namespace smart_ptrs;
class Widget
{
public:
	enum WidgetType
	{
		SimpleWidget,
		TabWidget,
		CalendarWidget
	};

	static void addChildToParent(my_shared_ptr<Widget>& parent, my_shared_ptr<Widget>& child);

	Widget()
	{
		_parent.reset();
	}

	Widget(std::string name)
	{
		_parent.reset();
		_name = name;
	}
	int getChildrenCount();
	my_shared_ptr<Widget> getParent();
	std::string getName()
	{
		return _name;
	}

	virtual WidgetType getType() {
		return SimpleWidget;
	}
private:
	std::vector<my_shared_ptr<Widget>> _children;
	std::string _name;
	my_weak_ptr<Widget> _parent;
	void addChild(my_shared_ptr<Widget>& child);
};