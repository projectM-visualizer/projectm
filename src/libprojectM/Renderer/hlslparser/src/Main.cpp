#include "HLSLParser.h"

#include "GLSLGenerator.h"
#include "HLSLGenerator.h"
#include "MSLGenerator.h"

#include <fstream>
#include <sstream>
#include <iostream>

enum Target
{
    Target_VertexShader,
    Target_FragmentShader,
};

enum Language
{
	Language_GLSL,
	Language_HLSL,
	Language_LegacyHLSL,
	Language_Metal,
};

std::string ReadFile( const char* fileName )
{
	std::ifstream ifs( fileName );
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	return buffer.str();
}

void PrintUsage()
{
	std::cerr << "usage: hlslparser [-h] [-fs | -vs] FILENAME ENTRYNAME\n"
		<< "\n"
		<< "Translate HLSL shader to GLSL shader.\n"
		<< "\n"
		<< "positional arguments:\n"
		<< " FILENAME    input file name\n"
		<< " ENTRYNAME   entry point of the shader\n"
		<< "\n"
		<< "optional arguments:\n"
		<< " -h, --help  show this help message and exit\n"
		<< " -fs         generate fragment shader (default)\n"
		<< " -vs         generate vertex shader\n"
		<< " -glsl       generate GLSL (default)\n"
		<< " -hlsl       generate HLSL\n"
		<< " -legacyhlsl generate legacy HLSL\n"
		<< " -metal      generate MSL\n";
}

int main( int argc, char* argv[] )
{
	using namespace M4;

	// Parse arguments
	const char* fileName = NULL;
	const char* entryName = NULL;

	Target target = Target_FragmentShader;
	Language language = Language_GLSL;

	for( int argn = 1; argn < argc; ++argn )
	{
		const char* const arg = argv[ argn ];

		if( String_Equal( arg, "-h" ) || String_Equal( arg, "--help" ) )
		{
			PrintUsage();
			return 0;
		}
		else if( String_Equal( arg, "-fs" ) )
		{
			target = Target_FragmentShader;
		}
		else if( String_Equal( arg, "-vs" ) )
		{
			target = Target_VertexShader;
		}
		else if( String_Equal( arg, "-glsl" ) )
		{
			language = Language_GLSL;
		}
		else if( String_Equal( arg, "-hlsl" ) )
		{
			language = Language_HLSL;
		}
		else if( String_Equal( arg, "-legacyhlsl" ) )
		{
			language = Language_LegacyHLSL;
		}
		else if( String_Equal( arg, "-metal" ) )
		{
			language = Language_Metal;
		}
		else if( fileName == NULL )
		{
			fileName = arg;
		}
		else if( entryName == NULL )
		{
			entryName = arg;
		}
		else
		{
			Log_Error( "Too many arguments\n" );
			PrintUsage();
			return 1;
		}
	}

	if( fileName == NULL || entryName == NULL )
	{
		Log_Error( "Missing arguments\n" );
		PrintUsage();
		return 1;
	}

	// Read input file
	const std::string source = ReadFile( fileName );

	// Parse input file
	Allocator allocator;
	HLSLParser parser( &allocator, fileName, source.data(), source.size() );
	HLSLTree tree( &allocator );
	if( !parser.Parse( &tree ) )
	{
		Log_Error( "Parsing failed, aborting\n" );
		return 1;
	}

	// Generate output
	if (language == Language_GLSL)
	{
		GLSLGenerator generator;
		if (!generator.Generate( &tree, GLSLGenerator::Target(target), GLSLGenerator::Version_140, entryName ))
		{
			Log_Error( "Translation failed, aborting\n" );
			return 1;
		}

		std::cout << generator.GetResult();
	}
	else if (language == Language_HLSL)
	{
		HLSLGenerator generator;
		if (!generator.Generate( &tree, HLSLGenerator::Target(target), entryName, language == Language_LegacyHLSL ))
		{
			Log_Error( "Translation failed, aborting\n" );
			return 1;
		}

		std::cout << generator.GetResult();
	}
	else if (language == Language_Metal)
	{
		MSLGenerator generator;
		if (!generator.Generate( &tree, MSLGenerator::Target(target), entryName ))
		{
			Log_Error( "Translation failed, aborting\n" );
			return 1;
		}

		std::cout << generator.GetResult();
	}

	return 0;
}
