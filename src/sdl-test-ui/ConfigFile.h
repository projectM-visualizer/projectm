









































#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using std::string;

class ConfigFile
{

protected:
    string myDelimiter;
    string myComment;
    string mySentry;
    std::map<string, string> myContents;

    typedef std::map<string, string>::iterator mapi;
    typedef std::map<string, string>::const_iterator mapci;


public:
    ConfigFile(string filename,
               string delimiter = "=",
               string comment = "#",
               string sentry = "EndConfigFile");
    ConfigFile();


    template<class T>
    T read(const string& key) const;
    template<class T>
    T read(const string& key, const T& value) const;
    template<class T>
    bool readInto(T& var, const string& key) const;
    template<class T>
    bool readInto(T& var, const string& key, const T& value) const;


    template<class T>
    void add(string key, const T& value);
    void remove(const string& key);


    bool keyExists(const string& key) const;


    string getDelimiter() const
    {
        return myDelimiter;
    }
    string getComment() const
    {
        return myComment;
    }
    string getSentry() const
    {
        return mySentry;
    }
    string setDelimiter(const string& s)
    {
        string old = myDelimiter;
        myDelimiter = s;
        return old;
    }
    string setComment(const string& s)
    {
        string old = myComment;
        myComment = s;
        return old;
    }


    friend std::ostream& operator<<(std::ostream& os, const ConfigFile& cf);
    friend std::istream& operator>>(std::istream& is, ConfigFile& cf);

protected:
    template<class T>
    static string T_as_string(const T& t);
    template<class T>
    static T string_as_T(const string& s);
    static void trim(string& s);



public:
    struct file_not_found {
        string filename;
        file_not_found(const string& filename_ = string())
            : filename(filename_)
        {
        }
    };
    struct key_not_found {
        string key;
        key_not_found(const string& key_ = string())
            : key(key_)
        {
        }
    };
};



template<class T>
string ConfigFile::T_as_string(const T& t)
{


    std::ostringstream ost;
    ost << t;
    return ost.str();
}



template<class T>
T ConfigFile::string_as_T(const string& s)
{


    T t;
    std::istringstream ist(s);
    ist >> t;
    return t;
}



template<>
inline string ConfigFile::string_as_T<string>(const string& s)
{


    return s;
}



template<>
inline bool ConfigFile::string_as_T<bool>(const string& s)
{



    bool b = true;
    string sup = s;
    for (string::iterator p = sup.begin(); p != sup.end(); ++p)
        *p = toupper(*p);
    if (sup == string("FALSE") || sup == string("F") ||
        sup == string("NO") || sup == string("N") ||
        sup == string("0") || sup == string("NONE"))
        b = false;
    return b;
}


template<class T>
T ConfigFile::read(const string& key) const
{

    mapci p = myContents.find(key);
    if (p == myContents.end())
        throw key_not_found(key);
    return string_as_T<T>(p->second);
}


template<class T>
T ConfigFile::read(const string& key, const T& value) const
{


    mapci p = myContents.find(key);
    if (p == myContents.end())
        return value;
    return string_as_T<T>(p->second);
}


template<class T>
bool ConfigFile::readInto(T& var, const string& key) const
{



    mapci p = myContents.find(key);
    bool found = (p != myContents.end());
    if (found)
        var = string_as_T<T>(p->second);
    return found;
}


template<class T>
bool ConfigFile::readInto(T& var, const string& key, const T& value) const
{



    mapci p = myContents.find(key);
    bool found = (p != myContents.end());
    if (found)
        var = string_as_T<T>(p->second);
    else
        var = value;
    return found;
}


template<class T>
void ConfigFile::add(string key, const T& value)
{

    string v = T_as_string(value);
    trim(key);
    trim(v);
    myContents[key] = v;
    return;
}

