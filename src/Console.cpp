#include "Fizz3d.h"
#include "Console.h"

Console::Console(void)
	: _text(""), _visible(false)
{
}

Console::~Console(void)
{
}

std::string Console::getConsoleText() const
{
	return _text;
}

void Console::addCharacter(char character)
{
	_text.push_back(character);
}

void Console::subtractCharacter()
{
	if(_text.length() > 0)
		_text = _text.substr(0, _text.length() - 1);
}

bool Console::isVisible() const
{
	return _visible;
}

void Console::setVisible(bool visible)
{
	_visible = visible;
	if(!_visible)
		clear();
}

void Console::clear()
{
	_text.clear();
}