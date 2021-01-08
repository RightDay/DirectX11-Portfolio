#pragma once

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(GameActor& actor) = 0;
}; 

class InputHandler
{
public:
	Command* handleInput();

	bool isPressed(char key);

private:
	Command* button1;
	Command* button2;
	Command* button3;
	Command* button4;
};

class JumpCommand : public Command 
{
	virtual void execute(GameActor& actor) { actor.Jump(); }
};