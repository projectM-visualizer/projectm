//
// Created by matthew on 1/7/19.
//
// This is a place to collect/run non-graphical unit tests
// CONSIDER using some framework like googletest etc, but for now didn't want new dependencies
//

#ifndef PROJECTM_LUA_TESTRUNNER_H
#define PROJECTM_LUA_TESTRUNNER_H


#include <string>
#include <vector>


class Test
{
public:
    const std::string getName() { return name; };
#ifdef NDEBUG
    virtual bool test() {return true;}
#else
    virtual bool test() = 0;
#endif
    virtual ~Test() = default;
protected:
    explicit Test(std::string name_) : name(name_) {};
    const std::string name;

    bool verify(const char *test, bool success);
};


class TestRunner
{
public:
    static bool run();
private:
    static std::vector<Test *> tests;
};


#endif //PROJECTM_LUA_TESTRUNNER_H
