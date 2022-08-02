#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>

#include "tests_manager.h"

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
#include "../tests/test12.h"
#include "../tests/test13.h"
#include "../tests/test14.h"
#include "../tests/test15.h"
#include "../tests/test16.h"
#include "../tests/test17.h"
#include "../tests/test18.h"

extern unsigned int RAY_IT_ECHO_LEVEL;


void FillListOfTests(std::vector<std::shared_ptr<Test>>& list_of_tests) {
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
    list_of_tests.push_back(std::make_shared<Test12>());
    list_of_tests.push_back(std::make_shared<Test13>());
    list_of_tests.push_back(std::make_shared<Test14>());
    list_of_tests.push_back(std::make_shared<Test15>());
    list_of_tests.push_back(std::make_shared<Test16>());
    list_of_tests.push_back(std::make_shared<Test17>());
    list_of_tests.push_back(std::make_shared<Test18>());
}

int RunAllTests() {

    RAY_IT_ECHO_LEVEL = 0;

    int number_of_errors = 0;

    std::vector<std::shared_ptr<Test>> list_of_tests;

    FillListOfTests(list_of_tests);

    for(auto test:list_of_tests){
        try{
            if(!test->Run()) {
                std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" OK"<<std::endl;
            } else {
                std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" Failed!"<<std::endl;
                number_of_errors++;
            }
        } catch (std::exception& e) {
            std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" Failed!   "<<e.what()<<std::endl;
        }
    }

    return number_of_errors;
}


int RunTest(const int i) {

    RAY_IT_ECHO_LEVEL = 0;

    int number_of_errors = 0;

    std::vector<std::shared_ptr<Test>> list_of_tests;

    FillListOfTests(list_of_tests);

    bool test_has_been_run = false;

    for(auto test:list_of_tests){
        if(test->mNumber == i) {
            test_has_been_run = true;
            try{
                if(!test->Run()) {
                    std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" OK"<<std::endl;
                } else {
                    std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" Failed!"<<std::endl;
                    number_of_errors++;
                }
            } catch (std::exception& e) {
                std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<test->mNumber<<" Failed!   "<<e.what()<<std::endl;
            }
        }
    }

    if( ! test_has_been_run) {
        std::cout<<"  Test "<<std::setw(3)<<std::setfill('0')<<i<<" could not be found!   "<<std::endl;
        return 1;
    }

    return number_of_errors;
}