#ifndef RayTracer_tests_manager
#define RayTracer_tests_manager

#include <iostream>
#include <vector>

#include "../tests/test.h"
#include "../tests/test1.h"
#include "../tests/test2.h"
#include "../tests/test3.h"
#include "../tests/test4.h"
#include "../tests/test5.h"
#include "../tests/test6.h"
#include "../tests/test7.h"

void RunTests() {

    std::vector<std::shared_ptr<Test>> list_of_tests;
    
    list_of_tests.push_back(std::make_shared<Test1>());
    list_of_tests.push_back(std::make_shared<Test2>());
    list_of_tests.push_back(std::make_shared<Test3>());
    list_of_tests.push_back(std::make_shared<Test4>());
    list_of_tests.push_back(std::make_shared<Test5>());
    list_of_tests.push_back(std::make_shared<Test6>());
    list_of_tests.push_back(std::make_shared<Test7>());

    for(auto test:list_of_tests){
        try{
            if(test->Run()) {
                std::cout<<"  Test "<<test->mNumber<<" OK"<<std::endl;
            } else {
                std::cout<<"  Test "<<test->mNumber<<" Failed!"<<std::endl;
            }
        } catch (std::exception& e) {
            std::cout<<"  Test "<<test->mNumber<<" Failed!   "<<e.what()<<std::endl;            
        }
    }    
}

#endif