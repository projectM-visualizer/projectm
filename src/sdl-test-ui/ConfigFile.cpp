#include "ConfigFile.h"

using std::string;

ConfigFile::ConfigFile(string filename, string delimiter,
                       string comment, string sentry)
    : myDelimiter(delimiter)
    , myComment(comment)
    , mySentry(sentry)
{


    std::ifstream in(filename.c_str());

    if (!in)
        throw file_not_found(filename);

    in >> (*this);
}


ConfigFile::ConfigFile()
    : myDelimiter(string(1, '='))
    , myComment(string(1, '#'))
{

}


void ConfigFile::remove(const string& key)
{

    myContents.erase(myContents.find(key));
    return;
}


bool ConfigFile::keyExists(const string& key) const
{

    mapci p = myContents.find(key);
    return (p != myContents.end());
}



void ConfigFile::trim(string& s)
{

    static const char whitespace[] = " \n\t\v\r\f";
    s.erase(0, s.find_first_not_of(whitespace));
    s.erase(s.find_last_not_of(whitespace) + 1U);
}


std::ostream& operator<<(std::ostream& os, const ConfigFile& cf)
{

    for (ConfigFile::mapci p = cf.myContents.begin();
         p != cf.myContents.end();
         ++p)
    {
        os << p->first << " " << cf.myDelimiter << " ";
        os << p->second << std::endl;
    }
    return os;
}


std::istream& operator>>(std::istream& is, ConfigFile& cf)
{


    typedef string::size_type pos;
    const string& delim = cf.myDelimiter;
    const string& comm = cf.myComment;
    const string& sentry = cf.mySentry;
    const pos skip = delim.length();

    string nextline = "";

    while (is || nextline.length() > 0)
    {

        string line;
        if (nextline.length() > 0)
        {
            line = nextline;
            nextline = "";
        }
        else
        {
            std::getline(is, line);
        }


        line = line.substr(0, line.find(comm));


        if (sentry != "" && line.find(sentry) != string::npos)
            return is;


        pos delimPos = line.find(delim);
        if (delimPos < string::npos)
        {

            string key = line.substr(0, delimPos);
            line.replace(0, delimPos + skip, "");




            bool terminate = false;
            while (!terminate && is)
            {
                std::getline(is, nextline);
                terminate = true;

                string nlcopy = nextline;
                ConfigFile::trim(nlcopy);
                if (nlcopy == "")
                    continue;

                nextline = nextline.substr(0, nextline.find(comm));
                if (nextline.find(delim) != string::npos)
                    continue;
                if (sentry != "" && nextline.find(sentry) != string::npos)
                    continue;

                nlcopy = nextline;
                ConfigFile::trim(nlcopy);
                if (nlcopy != "")
                    line += "\n";
                line += nextline;
                terminate = false;
            }


            ConfigFile::trim(key);
            ConfigFile::trim(line);
            cf.myContents[key] = line;
        }
    }

    return is;
}
