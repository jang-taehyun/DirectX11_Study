/*
Input class

- 역할 : 유저의 키보드 입력 처리
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
	void Initialize();					// 각 key 상태 초기화

	void KeyDown(unsigned int);			// key가 눌렸을 때
	void KeyUp(unsigned int);			// key가 떼어졌을 때

	bool IsKeyDown(unsigned int);		// key 상태 반환
};

#endif