#include "InputClass.h"

InputClass::InputClass() :
	m_keys()
{
}

InputClass::InputClass(const InputClass& ref) :
	m_keys()
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	/* 각 key들의 상태를 뗴어짐 상태로 초기화 */
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int input)
{
	return m_keys[input];
}
