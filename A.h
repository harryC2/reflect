#pragma once
#include <iostream>
#include "ClassFactory.h"
#include <string>

using namespace yazi::reflect;

class  A  final: public RelfectObject
{
public:
    string m_strName;
    int a{0};
public:
    A(/* args */):m_strName("abc"){};
    ~A(){};

void show()
{
    std::cout<<"A"<<std::endl;
}
void showAndArg(const string & strName)
{
    std::cout<<"A"<<strName<<std::endl;
}
int add(int a,int b){
    return a+b;
}
};
REGISTER_CLASS(A)
REGISTER_CLASS_FIELD(A, a)
REGISTER_CLASS_FIELD(A, m_strName)
REGISTER_CLASS_FUNC(A, show)
REGISTER_CLASS_FUNC_ARGS(A, showAndArg, const string &)
REGISTER_CLASS_FUNC_ARGS_RET(A, add, int, int, int)
