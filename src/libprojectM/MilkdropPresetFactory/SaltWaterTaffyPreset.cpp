//
// Created by matthew on 12/22/18.
//

#include "SaltWaterTaffyPreset.hpp"
#include "Parser.hpp"
#include "fatal.h"
#include <cstring>


int convert(std::istream & fs, std::ostream & os);
int parse_object(const char *prefix_init, const char *prefix_eqns, std::istream & fs, std::ostream & os);
int parse_equations(const char *prefix, std::istream & fs, std::ostream & os);
int parse_shader(const char *prefix, std::istream & fs, std::ostream & os);
int parse_object_line(const std::string &line, std::string &type, std::string &name, std::string &comment);
int parse_assignment_line(const std::string &line, std::string &name, std::string &value, std::string &comment);



SaltWaterTaffyPreset::SaltWaterTaffyPreset(const std::string & absoluteFilePath, const std::string & presetName, PresetOutputs & presetOutputs):
    MilkdropPreset(absoluteFilePath, presetName, presetOutputs)
{

}


int SaltWaterTaffyPreset::readIn(std::istream & fs)
{
    // I could actually write a new parser, and that would probably give better error messages
    // but for a start just translate to .milk format in memory
    std::ostringstream os;
    int ret = convert( fs, os );
    if (ret != PROJECTM_SUCCESS)
        return ret;
    std::string converted = os.str();
std::cerr << converted << std::endl;
    std::istringstream is(converted);
    return MilkdropPreset::readIn(is);
}


bool starts_with(const std::string & a, const std::string & b)
{
    return a.compare(0, b.length(), b) == 0;
}
bool ends_with(const std::string & a, const std::string & b)
{
    return a.compare(a.length()-b.length(), a.length(), b) == 0;
}
void trim(std::string & s)
{
    size_t start = s.find_first_not_of(' ');
    if (start == std::string::npos)
    {
        s.clear();
        return;
    }
    size_t end = s.find_last_not_of(' ');
    s.erase(end+1);
    s.erase(0,start);
}
bool getline(std::istream & fs, std::string & line)
{
    line.clear();
    if (!fs || fs.eof())
        return false;

    while (true)
    {
        char c = fs.get();
        if (c == EOF)
            return !line.empty();
        if (c == '\n')
            return true;
        line.append(1,c);
    }
}


// try to keep lines # matching between source and target

int convert(std::istream & fs, std::ostream & os)
{
    // read looking for top level objects: preset, wave, shape
    std::string line;
    int wavecount=0, shapecount=0;
    char prefix_init_buffer[30];
    char prefix_eqns_buffer[30];

    while (getline(fs,line))
    {
        trim(line);

        if (starts_with(line, "//") || line.empty())
        {
            os << line << std::endl;
        }
        else if (starts_with(line, "preset ") || starts_with(line, "wave ") || starts_with(line, "shape "))
        {
            std::string type, name, comment;
            int ret = parse_object_line(line, type, name, comment);
            if (ret != PROJECTM_SUCCESS)
                return ret;

            if (type == "preset")
            {
                os << "[" << (name.empty()?"preset0":name) << "]" << std::endl;;
                parse_object("", "", fs, os);
            }
            else if (type == "wave")
            {
                os << std::endl;
                sprintf(prefix_init_buffer, "wavecode_%d_", wavecount);
                sprintf(prefix_eqns_buffer, "wave_%d_", wavecount);
                wavecount++;
                parse_object(prefix_init_buffer, prefix_eqns_buffer, fs, os);
            }
            else // type == "shape"
            {
                os << std::endl;
                sprintf(prefix_init_buffer, "shapecode_%d_", shapecount);
                sprintf(prefix_eqns_buffer, "shape_%d_", shapecount);
                shapecount++;
                parse_object(prefix_init_buffer,prefix_eqns_buffer, fs, os);
            }
        }
        else
        {
            // we have a non-blank line that doesn't seem to start an object...
            if (PARSE_DEBUG) printf("parse taffy: expected start of object 'preset', 'shape', or 'wave': %s\n", line.c_str());
            return PROJECTM_PARSE_ERROR;
        }
    }
    return PROJECTM_SUCCESS;
}


// not including the shader program there are really only two line types
//
// {preset|wave|shape} 'name' {
// key=value


int parse_object_line(const std::string & line, std::string & type, std::string & name, std::string & comment)
{
    type.empty(); name.empty(); comment.empty();
    size_t start = 0;
    size_t end = line.length();

    size_t c = line.find("//");
    if (c != std::string::npos)
    {
        comment = line.substr(c);
        end = c;
    }
    if (line[end-1] != '{')
    {
        if (PARSE_DEBUG) printf("parse taffy: expected line to end with '{': %s\n", line.c_str());
        return PROJECTM_PARSE_ERROR;
    }
    start = line.find(' ');
    type = line.substr(0, start);

    start = line.find_first_not_of(" ", start);
    end =  line.find_last_not_of(' ', end-2)+1;
    if (end-start > 3 && line[start]=='\'' && line[end-1]=='\'')
    {
        start++;
        end--;
    }
    if (end > start)
        name = line.substr(start,end-start);
    return PROJECTM_SUCCESS;
}


int parse_assignment_line(const std::string &line, std::string &name, std::string &value, std::string &comment)
{
    name.empty(); value.empty(); comment.empty();

    size_t c = line.find("//");
    if (c != std::string::npos)
    {
        comment = line.substr(c);
    }
    size_t eq = line.find('=');
    if (eq == std::string::npos || eq > c)
    {
        name = line.substr(0,c);
        trim(name);
        if (name.empty())
            return PROJECTM_SUCCESS;
        if (PARSE_DEBUG) printf("parse taffy: expected line to containe '=': %s\n", line.c_str());
        return PROJECTM_PARSE_ERROR;
    }
    name = line.substr(0,eq);
    trim(name);
    value = line.substr(eq+1);
    trim(value);
    return PROJECTM_SUCCESS;
}



// parse init conditions, looking for function blocks, until '}
int parse_object(const char *prefix_init, const char *prefix_eqn, std::istream & fs, std::ostream & os)
{
    std::string line;
    std::string name, value, comment;
    char prefix_buffer[30];

    while (getline(fs,line))
    {
        trim(line);
        if (line.empty())
        {
            os << std::endl;
            continue;
        }
        else if (line == "}")
        {
            os << std::endl;
            return PROJECTM_SUCCESS;
        }

        int ret = parse_assignment_line(line, name, value, comment);
        if (ret != PROJECTM_SUCCESS)
            return ret;

        if (value == "{")
        {
            if (name != "per_frame_init" && name != "per_frame" &&  name != "per_point" && name != "per_pixel")
            {
                if (PARSE_DEBUG) printf("parse taffy: block name should be one of frame_init, per_frame, per_point, per_frame, per_pixel: %s\n", line.c_str());
                return PROJECTM_PARSE_ERROR;
            }

            if (!strlen(prefix_init) && (name == "per_frame" || name == "per_frame_init" || name == "per_pixel"))
                sprintf(prefix_buffer, "%s%s_", prefix_eqn, name.c_str());
            else
                sprintf(prefix_buffer, "%s%s", prefix_eqn, name.c_str());

            os << std::endl;
            ret = parse_equations(prefix_buffer, fs, os);
            if (ret != PROJECTM_SUCCESS)
                return ret;
        }
        else if (value == "\"\"\"")
        {
            std::string shader="comp";
            std::string lang="hlsl";
            if (name != "comp_hlsl" && name != "comp_glsl_330" && name != "warp_hlsl" && name != "warp_glsl_330")
            {
                if (PARSE_DEBUG) printf("parse taffy: shader name should be one of comp_hlsl, comp_glsl_330, warp, or warp_glsl_330: %s\n", line.c_str());
                return PROJECTM_PARSE_ERROR;
            }
            if (name == "comp_glsl_330" || name == "warp_glsl_330")
                lang = "glsl_330";
            if (starts_with(name, "warp"))
                shader = "warp";
            os << shader << "_lang=" << lang << name << std::endl;
            sprintf(prefix_buffer, "%s_", shader.c_str());
            ret = parse_shader(prefix_buffer, fs, os);
            if (ret != PROJECTM_SUCCESS)
                return ret;
        }
        else
        {
            os << prefix_init << line << std::endl;
        }
    }
    if (PARSE_DEBUG) printf("parse taffy: found EOF expected '}'\n");
    return PROJECTM_PARSE_ERROR;
}


int parse_equations(const char *prefix, std::istream & fs, std::ostream & os)
{
    std::string line;
    int line_no=0;

    bool continue_line = false;
    std::string newlines;

    while (getline(fs,line))
    {
        trim(line);
        if (starts_with(line, "//") || line.empty())
        {
            os << std::endl;
            continue;
        }
        else if (line == "}")
        {
            os << std::endl;
            return PROJECTM_SUCCESS;
        }
        else if (continue_line)
        {
            os << " " << line;
            // collect a new line to keep things lined up
            newlines.append("\n");
        }
        else
        {
            ++line_no;
            os << prefix << line_no << (line_no <= 9 ? " = ":"= ") << line;
            newlines = "";
        }
        if (ends_with(line, ";"))
        {
            os << std::endl;
            os << newlines;
            newlines = "";
            continue_line = false;
        }
        else
        {
            continue_line = true;
        }
    }
    if (PARSE_DEBUG) printf("parse taffy: found EOF expected '}'\n");
    return PROJECTM_PARSE_ERROR;
}


int parse_shader(const char *prefix, std::istream & fs, std::ostream & os)
{
    int count=0;
    std::string line;
    while (getline(fs,line))
    {
        std::string trimmed = line;
        trim(trimmed);
        if (trimmed.empty())
        {
            os << std::endl;
            continue;
        }
        else if (trimmed == "\"\"\"")
        {
            os << std::endl;
            return PROJECTM_SUCCESS;
        }
        else
        {
            os << prefix << (++count) << "=`" << line << std::endl;
        }
    }
    if (PARSE_DEBUG) printf("parse taffy: found EOF expected \"\"\"\n");
    return PROJECTM_PARSE_ERROR;
}
