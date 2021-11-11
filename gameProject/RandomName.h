#pragma once
#include <vector>
#include <string>

/*
	�����ʵ��û������������ȫ��������ж�
	����Ĭ�Ͽ��������ռ����޴�
*/

class FirstName {
public:
	std::string m_first;
	std::vector<std::string> m_last_list;
};

class RandomName
{
private:
	std::vector<FirstName*> m_pool;
public:
	RandomName();
	~RandomName();
	std::string GetName();
	void ReleaseName(std::string _name);
	void LoadFile();
};

