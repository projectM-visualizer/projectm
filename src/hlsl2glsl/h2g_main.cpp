#include <projectM.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>

std::string warp_prefix("warp_");
std::string comp_prefix("comp_");


#define starts_with(s,f)  boost::algorithm::starts_with(s,f)
#define trim(s) boost::algorithm::trim(s)



std::string hlsl2glsl(const std::string &glsl)
{
    return glsl;
}




int main(int argc, char *argv[])
{
    std::istream *pinput = &std::cin;
    std::ostream *poutput = &std::cout;

    if (argc >= 2)
    {
        pinput = new std::ifstream(argv[1]);
    }

    if (argc >= 3)
    {
        poutput = new std::ofstream(argv[2]);
    }

    std::istream &input = *pinput;
    std::ostream &output = *poutput;

    std::string line;
    std::string warp;
    std::string comp;
    while (std::getline(input, line))
    {
        if (starts_with(line,warp_prefix) || starts_with(line,comp_prefix))
        {
            size_t eq = line.find('=');
            if (eq == std::string::npos)
                continue;
            std::string rhs = line.substr(eq+1);
            trim(rhs);
            if (rhs.length()>0 && rhs[0] == '`')
                rhs = rhs.substr(1);
            if (starts_with(line,warp_prefix))
                warp += rhs + "\n";
            else
                comp += rhs + "\n";
        }
        else
            output << line << std::endl;
    }

    std::string warp_glsl = hlsl2glsl(warp);
    std::string comp_glsl = hlsl2glsl(comp);

    size_t line_count = 0;
    std::stringstream ss(warp_glsl);
    output << warp_prefix << "lang=glsl" << std::endl;
    while(std::getline(ss,line,'\n'))
    {
        output << warp_prefix << ++line_count << "=`" << line << std::endl;
    }
    line_count = 0;
    ss = std::stringstream(comp_glsl);
    output << comp_prefix << "lang=glsl" << std::endl;
    while(std::getline(ss,line,'\n'))
    {
        output << comp_prefix << ++line_count << "=`" << line << std::endl;
    }
    output.flush();

    if (1==1)
    {
        std::cerr << "\n - WARP HLSL - \n\n";
        std::cerr << warp;
        std::cerr << "\n - WARP GLSL - \n\n";
        std::cerr << warp_glsl;
        std::cerr << "\n - COMP HLSL - \n\n";
        std::cerr << comp;
        std::cerr << "\n - COMP GLSL - \n\n";
        std::cerr << comp_glsl;
        std::cerr.flush();
    }

    exit(0);
}