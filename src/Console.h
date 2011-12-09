
#ifndef CONSOLE_H
#define CONSOLE_H

#include "Fizz3d.h"


class Console
{
public:
	Console(void);
	~Console(void);

	std::string getConsoleText() const;
	std::string getCommand() const;
	std::vector<std::string> getArguments() const;

	void addCharacter(char character);
	void subtractCharacter();
	void clear();

	bool isVisible() const;
	void setVisible(bool visible);

private:
	std::string _text;
	bool _visible;
};


#endif
