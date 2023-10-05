/*
Input class

- ���� : ������ Ű���� �Է� ó��
*/

#ifndef  _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
	/* member variable */
private:
	bool m_keys[256];

	/* constructro & destructor */
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	/* interface */
public:
	void Initialize();					// �� key ���� �ʱ�ȭ

	void KeyDown(unsigned int);			// key�� ������ ��
	void KeyUp(unsigned int);			// key�� �������� ��

	bool IsKeyDown(unsigned int);		// key ���� ��ȯ
};

#endif