#pragma once
#include <Windows.h>
#include <vector>
#include <string>

#define NUM_KEYS	8

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
	RIGHTARROW
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

class InputManager
{
	static InputManager* instance;
	KeyInfo keys[NUM_KEYS];

public:
	InputManager();
	~InputManager();
	void InitKeys();
	static InputManager* Instance();
	//bool isUpPressed();
	//bool isDownPressed();
	//bool isLeftPressed();
	//bool isRightPressed();
	//bool isForwardPressed();
	//bool isBackwardPressed();

	bool GetKeyDown(int vKey);
	bool GetKeyHolding(int vKey);
	bool GetKeyUp(int vKey);
};

