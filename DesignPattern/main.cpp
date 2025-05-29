//
// Created by orange on 4/2/25.
//
#include "Singleton.h"
int main(){
    {
        using namespace std;
        cout<<"Singleton"<<endl;
        Singleton::getInstance()->printData();
        Singleton::getInstance()->modifyData(123);
        Singleton::getInstance()->printData();
    }
    {
        using namespace std;
        cout<<"Singleton"<<endl;
        SingletonWith2LockEfficient::getInstance()->printData();
        SingletonWith2LockEfficient::getInstance()->modifyData(123);
        SingletonWith2LockEfficient::getInstance()->printData();
    }

}