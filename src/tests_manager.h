#ifndef __Ray_ittests_manager
#define __Ray_ittests_manager

#include <iostream>
#include <vector>
#include <memory>

#include "../tests/test.h"
#include "../tests/test1.h"
#include "../tests/test2.h"
#include "../tests/test3.h"
#include "../tests/test4.h"
#include "../tests/test5.h"
#include "../tests/test6.h"
#include "../tests/test7.h"
#include "../tests/test8.h"
#include "../tests/test9.h"
#include "../tests/test10.h"
#include "../tests/test11.h"

int RunTests() {

    int number_of_errors = 0;

    std::vector<std::shared_ptr<Test>> list_of_tests;

    list_of_tests.push_back(std::make_shared<Test1>());
    list_of_tests.push_back(std::make_shared<Test2>());
    list_of_tests.push_back(std::make_shared<Test3>());
    list_of_tests.push_back(std::make_shared<Test4>());
    list_of_tests.push_back(std::make_shared<Test5>());
    list_of_tests.push_back(std::make_shared<Test6>());
    list_of_tests.push_back(std::make_shared<Test7>());
    list_of_tests.push_back(std::make_shared<Test8>());
    list_of_tests.push_back(std::make_shared<Test9>());
    list_of_tests.push_back(std::make_shared<Test10>());
    list_of_tests.push_back(std::make_shared<Test11>());

    for(auto test:list_of_tests){
        try{
            if(!test->Run()) {
                std::cout<<"  Test "<<test->mNumber<<" OK"<<std::endl;
            } else {
                std::cout<<"  Test "<<test->mNumber<<" Failed!"<<std::endl;
                number_of_errors++;
            }
        } catch (std::exception& e) {
            std::cout<<"  Test "<<test->mNumber<<" Failed!   "<<e.what()<<std::endl;
        }
    }

    return number_of_errors;
}

#endif