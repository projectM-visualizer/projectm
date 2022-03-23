//
// Created by matthew on 1/7/19.
//

#include <iostream>
#include <MilkdropPresetFactory/Parser.hpp>
#include <TestRunner.hpp>
#include <MilkdropPresetFactory/Param.hpp>

std::vector<Test *> TestRunner::tests;


bool Test::verify(const char *test, bool success)
{
    if (!success)
        std::cout << "failed " << test << std::endl;
    return success;
}


bool TestRunner::run()
{
    if (tests.empty())
    {
        // We still call register/run tests in NDEBUG (useful for performance testing)
        //   but tests may choose to comment out body to save space
        tests.push_back(Param::test());
        tests.push_back(Parser::test());
        tests.push_back(Expr::test());
        tests.push_back(Pcm::MakeTest());
    }

    int count = 0;
    bool successful = true;
    for (auto it=tests.begin() ; it < tests.end() ; it++ )
    {
        if (nullptr == (*it))
            continue;
        count++;
        std::cout << "TestRunner: " << (*it)->getName() << " started" << std::endl;
        std::cout.flush();
        bool result = (*it)->test();
        successful &= result;
        if (result)
            std::cout << "TestRunner: " << (*it)->getName() << " passed" << std::endl;
        else
            std::cout << "TestRunner: " << (*it)->getName() << " FAILED" << std::endl;
    }
    if (0 == count)
        std::cout << "TestRunner: no tests found to run" << std::endl;
    return successful;
}
