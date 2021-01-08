#include "stdafx.h"
#include "GameActor.h"
#include "Command.h"

Command* InputHandler::handleInput()
{
	if (isPressed('W')) return button1;
	if (isPressed('A')) return button2;
	if (isPressed('S')) return button3;
	if (isPressed('D')) return button4;
}

bool InputHandler::isPressed(char key)
{
	if (Keyboard::Get()->Press(key)) return true;

	return false;
}
