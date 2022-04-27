#ifndef __Ray_ittests_manager
#define __Ray_ittests_manager

#include "../tests/test.h"

void FillListOfTests(std::vector<std::shared_ptr<Test>>& list_of_tests);
int RunAllTests();
int RunTest(const int i);

#endif