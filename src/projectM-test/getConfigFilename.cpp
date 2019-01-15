//
// File:   getConfigFilename.cpp
//
// Author: fatray
//
// Created on 05 December 2007, 23:39
//
// FIXME: portability


// I hacked include<string> on to silence my compiler, is it valid?
#include <string>
#include <cstring>
#include <cstdlib>
#include "getConfigFilename.h"
#include <fcntl.h>	
#include <sys/stat.h>
#include <cstdio>

// get the full pathname of a configfile
std::string getConfigFilename()
{
    FILE *in;

    if ((in = fopen(CONFIG_FILE, "r")))
    {
        printf("Successfully opened default config file\n");
        fclose(in);
        return std::string(CONFIG_FILE);
    }

    printf("Did not find config.inp in current directory.\n");
    exit(1);
}

