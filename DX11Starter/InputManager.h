#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <Xinput.h>

#define NUM_KEYS	9

enum KeyPressed
{
	// Remember to update total number of keys if add add new
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	LEFTARROW,
	RIGHTARROW,
	SPACEBAR
};

enum KeyStates
{
	RELEASED = 0,
	PRESSED,
	HELD
};

struct KeyInfo
{
	std::string name;
	int vKey;
	SHORT keyState;
};

enum Sticks
{
	LEFTSTICK = 0,
	RIGHTSTICK,
	LEFTTRIGGER,
	RIGHTTRIGGER
};

enum StickDirections
{
	STICKUP = 0,
	STICKDOWN,
	STICKLEFT,
	STICKRIGHT,
	STICKNONE
};

class InputManager
{
	static InputManager* instance;
	KeyInfo keys[NUM_KEYS];

public:
	InputManager();
	~InputManager();
	void InitKeys();
	static InputManager* Instance();

	bool GetKeyDown(int vKey);
	bool GetKeyHolding(int vKey);
	bool GetKeyUp(int vKey);
	bool GetButtonDown(WORD);

	StickDirections GetStickXDirection(Sticks stick);
	StickDirections GetStickYDirection(Sticks stick);

	bool GetTriggerDown(Sticks trigger);

private:
	//Controller code based off https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/
	int cId;
	XINPUT_STATE state;

	float deadzoneX;
	float deadzoneY;
	float triggerDeadzone;

	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;

	int GetPort();
	XINPUT_GAMEPAD *GetState();
	bool CheckConnection();
	bool Refresh();
};

