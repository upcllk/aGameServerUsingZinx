#include "RandomName.h"
#include <random>
#include <fstream>

static std::default_random_engine random_engine(time(NULL));

RandomName::RandomName()
{
}

RandomName::~RandomName()
{
    while (!m_pool.empty()) {
        if (*(m_pool.begin()) != nullptr) {
            delete *(m_pool.begin());
        }
        m_pool.erase(m_pool.begin());
    }
}

std::string RandomName::GetName()
{
    int index_first = random_engine() % m_pool.size();
    std::string first = m_pool[index_first]->m_first;
    auto& last_list = m_pool[index_first]->m_last_list;
    int index_last = random_engine() % last_list.size();
    std::string last = last_list[index_last];

    last_list.erase(last_list.begin() + index_last);
    // 本姓的所有名都取完了，把姓删掉
    if (last_list.size() == 0) {
        delete m_pool[index_first];
        m_pool.erase(m_pool.begin() + index_first);
    }

    return first + " " + last;
}

void RandomName::ReleaseName(std::string _name)
{
    int space_pos = _name.find(' ');
    std::string first = _name.substr(0, space_pos);
    std::string last = _name.substr(space_pos + 1);

    bool found = false;

    for (auto first_name : m_pool) {
        if (first_name->m_first == first) {
            found = true;
            first_name->m_last_list.push_back(last);
            break;
        }
    }
    if (found == false) {
        auto first_name = new FirstName();
        first_name->m_first = first;
        first_name->m_last_list.push_back(last);
        m_pool.push_back(first_name);
    }
}

void RandomName::LoadFile()
{
    std::ifstream firstFile("random_first.txt");
    std::ifstream lastFile("random_last.txt");

    std::vector<std::string> tmp;
    std::string last_name;
    while (getline(lastFile, last_name)) {
        tmp.push_back(last_name);
    }
    
    std::string first_name;
    while (getline(firstFile, first_name)) {
        auto first_name_obj = new FirstName();
        first_name_obj->m_first = first_name;
        first_name_obj->m_last_list = tmp;
        m_pool.push_back(first_name_obj);
    }

}
