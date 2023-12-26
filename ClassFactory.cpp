#include "ClassFactory.h"

using namespace yazi::reflect;

std::unique_ptr<RelfectObject> ClassFactory::get_classPtr(const string &className)
{
    auto it = m_classMap.find(className);
    if (it == m_classMap.end())
    {
        return nullptr;
    }
    return std::unique_ptr<RelfectObject>(it->second());
}

void ClassFactory::register_field(const string &className, const string &fileName, size_t offset)
{
    auto it = m_classMap.find(className);
    if (it == m_classMap.end())
        throw std::runtime_error("Can not find class");
    if (m_classFieldMap.find(className) == m_classFieldMap.end())
    {
        m_classFieldMap[className] = std::map<std::string, size_t>();
    }
    m_classFieldMap[className][fileName] = offset;
}
void ClassFactory::register_function(const string &className, const string &funcName, size_t ptr)
{
    auto it = m_classMap.find(className);
    if (it == m_classMap.end())
        throw std::runtime_error("Can not find class");
    if (m_classFunctionMap.find(className) == m_classFunctionMap.end())
    {
        m_classFunctionMap[className] = std::map<std::string, size_t>();
    }
    m_classFunctionMap[className][funcName] = ptr;
}