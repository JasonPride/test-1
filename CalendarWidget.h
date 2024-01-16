#pragma once
#include "Widget.h"

class CalendarWidget : public Widget
{
public:
	CalendarWidget() : Widget() {}
	CalendarWidget(std::string name) : Widget(name) {}
	virtual WidgetType getType();
private:
};