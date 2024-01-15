#pragma once
#include "Widget.h"

class TabWidget : public Widget
{
public:
	TabWidget() : Widget() {}
	TabWidget(my_shared_ptr<Widget>& parent) : Widget(parent) {}
	virtual WidgetType getType();
private:
};