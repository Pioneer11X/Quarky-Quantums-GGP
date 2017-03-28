#include "InputManager.h"

InputManager* InputManager::instance;

InputManager::InputManager()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = NULL;
	}

	instance = this;

	InitKeys();
}


InputManager::~InputManager()
{
}

void InputManager::InitKeys()
{
	keys[0].name = "UP";
	keys[0].vKey = 'W';
	keys[0].keyState = 0;

	keys[1].name = "DOWN";
	keys[1].vKey = 'S';
	keys[1].keyState = 0;

	keys[2].name = "LEFT";
	keys[2].vKey = 'A';
	keys[2].keyState = 0;

	keys[3].name = "RIGHT";
	keys[3].vKey = 'D';
	keys[3].keyState = 0;

	keys[4].name = "FORWARD";
	keys[4].vKey = 'Z';
	keys[4].keyState = 0;

	keys[5].name = "BACKWARD";
	keys[5].vKey = 'X';
	keys[5].keyState = 0;
}

InputManager * InputManager::Instance()
{
	return instance;
}

//bool InputManager::isUpPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::UP) & 0x8000)
//		return true;
//	return false;
//}
//
//bool InputManager::isDownPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::DOWN) & 0x8000)
//		return true;
//	return false;
//}
//
//bool InputManager::isLeftPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::LEFT) & 0x8000)
//		return true;
//	return false;
//}
//
//bool InputManager::isRightPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::RIGHT) & 0x8000)
//		return true;
//	return false;
//}
//
//bool InputManager::isForwardPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::FORWARD) & 0x8000)
//		return true;
//	return false;
//}
//
//bool InputManager::isBackwardPressed()
//{
//	if (GetAsyncKeyState(KeyPressed::BACKWARD) & 0x8000)
//		return true;
//	return false;
//}

bool InputManager::GetKeyDown(int keyNumber)
{
	if ((GetAsyncKeyState(keys[keyNumber].vKey) & 0x8000) && (keys[keyNumber].keyState != 1))
	{
		keys[keyNumber].keyState = KeyStates::PRESSED;
		return true;
	}

	if (!(GetAsyncKeyState(keys[keyNumber].vKey) & 0x8000))
	{
		keys[keyNumber].keyState = KeyStates::RELEASED;
	}

	return false;
}

bool InputManager::GetKeyHolding(int keyNumber)
{
	if ((GetAsyncKeyState(keys[keyNumber].vKey) & 0x8000))
	{
		keys[keyNumber].keyState = KeyStates::HELD;
		return true;
	}

	keys[keyNumber].keyState = KeyStates::RELEASED;
	return false;
}

bool InputManager::GetKeyUp(int keyNumber)
{
	if (GetKeyHolding(keyNumber))
	{
		if (keys[keyNumber].keyState != KeyStates::RELEASED)
		{
			for (int i = 0; i < 32678; i++)
			{
				GetKeyHolding(keyNumber);

				if (keys[keyNumber].keyState == KeyStates::RELEASED)
					return true;
			}
		}

	}

	return false;
}
