#pragma once
#include <Windows.h>


enum KeyPressed
{
	NONE,
	UP			= VK_SPACE,
	DOWN		=	'X',
	LEFT		=	'A',
	RIGHT		=	'D',
	FORWARD		=	'W',
	BACKWARD	=	'S'
};

class InputManager
{
	static InputManager* instance;
public:
	InputManager();
	~InputManager();
	static InputManager* Instance();
	KeyPressed GetKeyPressed();
	bool isUpPressed();
	bool isDownPressed();
	bool isLeftPressed();
	bool isRightPressed();
	bool isForwardPressed();
	bool isBackwardPressed();
};

