#include<iostream>
#include <string>
#include"A.h"
#include"ClassFactory.h"
using namespace yazi::reflect;

int main()
{
   auto object = ClassFactory::getInstance()->get_classPtr("A");
   string strTest;
   object->get("A","m_strName",strTest);
   object->set("A","a",1);
   int c = 0;

  object->get("A","a",c);
  object->call("A","showAndArg",string("hello world"));
    return 0;

}