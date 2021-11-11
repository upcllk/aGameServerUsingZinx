#pragma once
#include <vector>
#include <string>

/*
	这里的实现没有做可用名字全部用完的判断
	我们默认可用姓名空间无限大
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

