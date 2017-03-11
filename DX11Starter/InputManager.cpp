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
}


InputManager::~InputManager()
{
}

InputManager * InputManager::Instance()
{
	return instance;
}

KeyPressed InputManager::GetKeyPressed()
{
	if (GetAsyncKeyState(KeyPressed::UP) & 0x8000)
		return KeyPressed::UP;
	if (GetAsyncKeyState(KeyPressed::DOWN) & 0x8000)
		return KeyPressed::DOWN;
	if (GetAsyncKeyState(KeyPressed::RIGHT) & 0x8000)
		return KeyPressed::RIGHT;
	if (GetAsyncKeyState(KeyPressed::LEFT) & 0x8000)
		return KeyPressed::LEFT;

	return NONE;
}

bool InputManager::isUpPressed()
{
	if (GetAsyncKeyState(KeyPressed::UP) & 0x8000)
		return true;
	return false;
}

bool InputManager::isDownPressed()
{
	if (GetAsyncKeyState(KeyPressed::DOWN) & 0x8000)
		return true;
	return false;
}

bool InputManager::isLeftPressed()
{
	if (GetAsyncKeyState(KeyPressed::LEFT) & 0x8000)
		return true;
	return false;
}

bool InputManager::isRightPressed()
{
	if (GetAsyncKeyState(KeyPressed::RIGHT) & 0x8000)
		return true;
	return false;
}

bool InputManager::isForwardPressed()
{
	if (GetAsyncKeyState(KeyPressed::FORWARD) & 0x8000)
		return true;
	return false;
}

bool InputManager::isBackwardPressed()
{
	if (GetAsyncKeyState(KeyPressed::BACKWARD) & 0x8000)
		return true;
	return false;
}
