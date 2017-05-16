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

	deadzoneX = 0.2;
	deadzoneY = 0.2;
	triggerDeadzone = 0.2;

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

	keys[6].name = "LEFTARROW";
	keys[6].vKey = 0x25;
	keys[6].keyState = 0;

	keys[7].name = "RIGHTARROW";
	keys[7].vKey = 0x27;
	keys[7].keyState = 0;

	keys[8].name = "SPACEBAR";
	keys[8].vKey = 0x20;
	keys[8].keyState = 0;
}

InputManager * InputManager::Instance()
{
	return instance;
}

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

int InputManager::GetPort() 
{
	return cId + 1;
}

XINPUT_GAMEPAD* InputManager::GetState() 
{
	return &state.Gamepad;
}

bool InputManager::CheckConnection()
{
	int controllerId = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
			controllerId = i;
	}

	cId = controllerId;

	return controllerId != -1;
}

bool InputManager::Refresh()
{
	if (cId == -1)
		CheckConnection();

	if (cId != -1)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(cId, &state) != ERROR_SUCCESS)
		{
			cId = -1;
			return false;
		}

		float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);

		leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));

		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);

		float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);

		rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));

		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);

		leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)state.Gamepad.bRightTrigger / 255;

		return true;
	}
	return false;
}

bool InputManager::GetButtonDown(WORD button)
{
	Refresh();
	return (state.Gamepad.wButtons & button) != 0;
}

StickDirections InputManager::GetStickXDirection(Sticks stick) {
	float xDirection = 0;
	
	switch (stick)
	{
		case LEFTSTICK:
			xDirection = leftStickX;
			break;
		case RIGHTSTICK:
			xDirection = rightStickX;
			break;
		default:
			return StickDirections::STICKNONE;
			break;
	}

	if (fabsf(xDirection) > deadzoneX) 
	{
		if (xDirection < 0) 
		{
			return StickDirections::STICKLEFT;
		}
		else if (xDirection > 0)
		{
			return StickDirections::STICKRIGHT;
		}
	}
	return StickDirections::STICKNONE;
}

StickDirections InputManager::GetStickYDirection(Sticks stick) {
	float yDirection = 0;

	switch (stick)
	{
		case LEFTSTICK:
			yDirection = leftStickY;
			break;
		case RIGHTSTICK:
			yDirection = rightStickY;
			break;
		default:
			return StickDirections::STICKNONE;
			break;
	}

	if (fabsf(yDirection) > deadzoneY) 
	{
		if (yDirection < 0) 
		{
			return StickDirections::STICKLEFT;
		}
		else if (yDirection > 0)
		{
			return StickDirections::STICKRIGHT;
		}
	}
	return StickDirections::STICKNONE;
}

bool InputManager::GetTriggerDown(Sticks trigger) {
	switch (trigger) {
		case LEFTTRIGGER:
			if (leftTrigger > triggerDeadzone) {
				return true;
			}
			break;
		case RIGHTTRIGGER:
			if (rightTrigger > triggerDeadzone) {
				return true;
			}
			break;
		default:
			break;
		}
	return false;
}
