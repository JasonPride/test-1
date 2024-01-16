#pragma once
#include "Widget.h"

class TabWidget : public Widget
{
public:
	TabWidget() : Widget() {}
	TabWidget(std::string name) : Widget(name) {}
	virtual WidgetType getType();
private:
};