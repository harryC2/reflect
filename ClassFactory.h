#pragma once
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <exception>
#include <cstring>
#include <stdexcept>

using namespace std;

namespace yazi
{
    namespace reflect
    {

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#define INPORT_API __declspec(dllinport)
#elif __linux__
#define EXPORT_API
#define INPORT_API
#endif

        class EXPORT_API RelfectObject;
        class EXPORT_API ClassFactory
        {
            friend RelfectObject;

        private:
            ClassFactory(){};
            ~ClassFactory(){};
            ClassFactory(const ClassFactory &other) = delete;
            ClassFactory &operator=(const ClassFactory &other) = delete;

        public:
            static ClassFactory *getInstance()
            {

                static ClassFactory instance;
                return &instance;
            }

        public:
            template <class T>
            void register_class(const string &className, std::function<T *(void)> &&f);
            std::unique_ptr<RelfectObject> get_classPtr(const string &className);

            void register_field(const string &className, const string &fileName, size_t offset);
            void register_function(const string &className, const string &funcName, size_t ptr);

        private:
            std::map<string, std::function<RelfectObject *(void)>> m_classMap; // 类名与类对象映射
            std::map<string, std::map<string, size_t>> m_classFieldMap;        // 类名 对应成员地址地址
            std::map<string, std::map<string, size_t>> m_classFunctionMap;     // 类名 对应方法名与方法地址
        };
        template <class T>
        void ClassFactory::register_class(const string &className, std::function<T *(void)> &&f)
        {
            m_classMap[className] = std::forward<std::function<T *(void)>>(f);
        };

        class EXPORT_API RelfectObject
        {
        public:
            virtual ~RelfectObject(){};

            void call(const std::string &className, const std::string &funcName);

            template <class Ret>
            Ret call(const std::string &className, const std::string &funcName);

            template <class... Args>
            void call(const std::string &className, const std::string &funcName, Args &&...args);

            template <class Ret, class... Args>
            Ret call(const std::string &className, const std::string &funcName, Args &&...args);

            template <class T>
            void set(const std::string &className, const std::string &fieldName, const T &src);
            template <class T>
            void get(const std::string &className, const std::string &fieldName, T &src);
        };

        template <class T>
        void RelfectObject::set(const std::string &className, const std::string &fieldName, const T &src)
        {
            auto it = ClassFactory::getInstance()->m_classMap.find(className);
            if (it == ClassFactory::getInstance()->m_classMap.end())
            {
                throw std::runtime_error("Can not find class");
            }
            auto it2 = ClassFactory::getInstance()->m_classFieldMap[className].find(fieldName);
            if (it2 == ClassFactory::getInstance()->m_classFieldMap[className].end())
                throw std::runtime_error("Can not find field");
            size_t pField = (size_t)this + it2->second; // 这是这个具体的成员的地址， 需要给他设置值 src;
            *((T *)pField) = src;                       // 先将变量类型转换成T*; 改变这个地址变量指向的值
        };
        template <class T>
        void RelfectObject::get(const std::string &className, const std::string &fieldName, T &src)
        {
            auto it = ClassFactory::getInstance()->m_classMap.find(className);
            if (it == ClassFactory::getInstance()->m_classMap.end())
            {
                throw std::runtime_error("Can not find class");
            }
            auto it2 = ClassFactory::getInstance()->m_classFieldMap[className].find(fieldName);
            if (it2 == ClassFactory::getInstance()->m_classFieldMap[className].end())
                throw std::runtime_error("Can not find field");
            size_t pField = (size_t)this + it2->second; // 这是这个具体的成员的地址， 需要给他设置值 src;
            src = *((T *)pField);
        };

        // void RelfectObject::call(const std::string &className, const std::string &funcName)
        // {
        //     auto it =  ClassFactory::getInstance()->m_classMap.find(className);
        //     if(it == ClassFactory::getInstance()->m_classMap.end())
        //     {
        //         throw std::runtime_error("Can not find class");
        //     }
        //     auto it2 = ClassFactory::getInstance()->m_classFunctionMap[className].find(funcName);
        //     if (it2 == ClassFactory::getInstance()->m_classFunctionMap[className].end())
        //         throw std::runtime_error("Can not find field");
        //     // decltype 编译期自动推导类型，this 是RelfectObject 的子类 指针
        //     std::function<void(decltype(this))> func = *(std::function<void(decltype(this))> *)(it2->second);
        //     func(this);
        // }
        template <class Ret>
        Ret RelfectObject::call(const std::string &className, const std::string &funcName)
        {
            auto it = ClassFactory::getInstance()->m_classMap.find(className);
            if (it == ClassFactory::getInstance()->m_classMap.end())
            {
                throw std::runtime_error("Can not find class");
            }
            auto it2 = ClassFactory::getInstance()->m_classFunctionMap[className].find(funcName);
            if (it2 == ClassFactory::getInstance()->m_classFunctionMap[className].end())
                throw std::runtime_error("Can not find field");
            auto func = *(std::function<Ret(decltype(this))> *)(it2->second);
            return func(this);
        }

        template <class... Args>
        void RelfectObject::call(const std::string &className, const std::string &funcName, Args &&...args)
        {
            auto it = ClassFactory::getInstance()->m_classMap.find(className);
            if (it == ClassFactory::getInstance()->m_classMap.end())
            {
                throw std::runtime_error("Can not find class");
            }
            auto it2 = ClassFactory::getInstance()->m_classFunctionMap[className].find(funcName);
            if (it2 == ClassFactory::getInstance()->m_classFunctionMap[className].end())
                throw std::runtime_error("Can not find field");
            auto func = *(std::function<void(decltype(this), Args && ...)> *)(it2->second);
            func(this, std::forward<Args>(args)...);
        }

        template <class Ret, class... Args>
        Ret RelfectObject::call(const std::string &className, const std::string &funcName, Args &&...args)
        {
            auto it = ClassFactory::getInstance()->m_classMap.find(className);
            if (it == ClassFactory::getInstance()->m_classMap.end())
            {
                throw std::runtime_error("Can not find class");
            }
            auto it2 = ClassFactory::getInstance()->m_classFunctionMap[className].find(funcName);
            if (it2 == ClassFactory::getInstance()->m_classFunctionMap[className].end())
                throw std::runtime_error("Can not find field");
            std::function<Ret(decltype(this), Args && ...)> func = *(std::function<Ret(decltype(this), Args && ...)> *)(it2->second());
            return func(this, std::forward<Args>(args)...);
        }

        class ClassRegister
        {

        public:
            ClassRegister(const string &className, std::function<RelfectObject *(void)> &&f)
            {
                ClassFactory::getInstance()->register_class(className, std::forward<std::function<RelfectObject *(void)>>(f));
            }
        };

        class EXPORT_API ClassFieldRegister
        {
        public:
            ClassFieldRegister(const std::string &className, const std::string &fieldName, size_t offset)
            {
                ClassFactory::getInstance()->register_field(className, fieldName, offset);
            }
        };

        class EXPORT_API ClassFunctionRegister
        {
        public:
            ClassFunctionRegister(const std::string &className, const std::string &funcName, size_t ptr)
            {
                ClassFactory::getInstance()->register_function(className, funcName, ptr);
            }
        };

        // ## 宏里面 两个#代表字符串链接  # 一个井号代表转成一个字符串了
#define REGISTER_CLASS(className) \
    ClassRegister classregister##className(#className, []() { return new className(); });

// 成员地址是同一个可以不用new，这里计算的是偏移地址，函数地址减去首地址
#define REGISTER_CLASS_FIELD(className, fieldName)                              \
    className tmp##className##fieldName;                                        \
    yazi::reflect::ClassFieldRegister ClassFieldRegister##className##fieldName( \
        #className, #fieldName,                                                 \
        ((size_t) & tmp##className##fieldName.fieldName - (size_t) & tmp##className##fieldName));

// 方法地址是同一个可以不用new， 公用同一个方法地址
#define REGISTER_CLASS_FUNC(className, funcName)                                          \
    std::function<void(className *)> tmpFunc##className##funcName = &className::funcName; \
    yazi::reflect::ClassFunctionRegister ClassFunctionRegister##className##funcName(#className, #funcName, (size_t) & tmpFunc##className##funcName);

#define REGISTER_CLASS_FUNC_RET(className, funcName, ret)                                \
    std::function<ret(className *)> tmpFunc##className##funcName = &className::funcName; \
    yazi::reflect::ClassFunctionRegister ClassFunctionRegister##className##funcName(#className, #funcName, (size_t) & tmpFunc##className##funcName);

#define REGISTER_CLASS_FUNC_ARGS(className, funcName, ...)                                             \
    std::function<void(className *, __VA_ARGS__)> tmpFunc##className##funcName = &className::funcName; \
    yazi::reflect::ClassFunctionRegister ClassFunctionRegister##className##funcName(#className, #funcName, (size_t) & tmpFunc##className##funcName);

#define REGISTER_CLASS_FUNC_ARGS_RET(className, funcName, ret, ...)                                   \
    std::function<ret(className *, __VA_ARGS__)> tmpFunc##className##funcName = &className::funcName; \
    yazi::reflect::ClassFunctionRegister ClassFunctionRegister##className##funcName(#className, #funcName, (size_t) & tmpFunc##className##funcName);

    }
}