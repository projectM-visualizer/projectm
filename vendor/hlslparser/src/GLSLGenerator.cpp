//=============================================================================
//
// Render/GLSLGenerator.cpp
//
// Created by Max McGuire (max@unknownworlds.com)
// Copyright (c) 2013, Unknown Worlds Entertainment, Inc.
//
//=============================================================================

#include "GLSLGenerator.h"
#include "HLSLParser.h"
#include "HLSLTree.h"

//#include "Engine/String.h"
//#include "Engine/Log.h"
#include "Engine.h"

#include <stdarg.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string>

namespace M4
{

static const HLSLType kFloatType(HLSLBaseType_Float);
static const HLSLType kUintType(HLSLBaseType_Uint);
static const HLSLType kIntType(HLSLBaseType_Int);
static const HLSLType kBoolType(HLSLBaseType_Bool);

// These are reserved words in GLSL that aren't reserved in HLSL.
const char* GLSLGenerator::s_reservedWord[] =
    {
        "output",
        "input",
        "mod",
        "mix",
        "fract",
        "dFdx",
        "dFdy",
        "filter",
		"main",
    };

static const char* GetTypeName(const HLSLType& type)
{
    switch (type.baseType)
    {
    case HLSLBaseType_Void:         return "void";
    case HLSLBaseType_Float:        return "float";
    case HLSLBaseType_Float2:       return "vec2";
    case HLSLBaseType_Float3:       return "vec3";
    case HLSLBaseType_Float4:       return "vec4";
    case HLSLBaseType_Float2x4:     return "mat4x2";
    case HLSLBaseType_Float2x3:     return "mat3x2";
    case HLSLBaseType_Float2x2:     return "mat2";
    case HLSLBaseType_Float3x4:     return "mat4x3";
    case HLSLBaseType_Float3x3:     return "mat3";
    case HLSLBaseType_Float3x2:     return "mat2x3";
    case HLSLBaseType_Float4x4:     return "mat4";
    case HLSLBaseType_Float4x3:     return "mat3x4";
    case HLSLBaseType_Float4x2:     return "mat2x4";
    case HLSLBaseType_Bool:         return "bool";
	case HLSLBaseType_Bool2:        return "bvec2";
	case HLSLBaseType_Bool3:        return "bvec3";
	case HLSLBaseType_Bool4:        return "bvec4";
    case HLSLBaseType_Int:          return "int";
    case HLSLBaseType_Int2:         return "ivec2";
    case HLSLBaseType_Int3:         return "ivec3";
    case HLSLBaseType_Int4:         return "ivec4";
    case HLSLBaseType_Uint:         return "uint";
    case HLSLBaseType_Uint2:        return "uvec2";
    case HLSLBaseType_Uint3:        return "uvec3";
    case HLSLBaseType_Uint4:        return "uvec4";
    case HLSLBaseType_Texture:      return "texture";
    case HLSLBaseType_Sampler:      return "sampler";
    case HLSLBaseType_Sampler2D:    return "sampler2D";
    case HLSLBaseType_Sampler3D:    return "sampler3D";
    case HLSLBaseType_SamplerCube:  return "samplerCube";
    case HLSLBaseType_Sampler2DMS:  return "sampler2DMS";
    case HLSLBaseType_Sampler2DArray:  return "sampler2DArray";
    case HLSLBaseType_UserDefined:  return type.typeName;
    default:
        ASSERT(0);
        return "?";
    }
}

static bool GetCanImplicitCast(const HLSLType& srcType, const HLSLType& dstType)
{
    return srcType.baseType == dstType.baseType;
}

static int GetFunctionArguments(HLSLFunctionCall* functionCall, HLSLExpression* expression[], int maxArguments)
{
    HLSLExpression* argument = functionCall->argument;
    int numArguments = 0;
    while (argument != NULL)
    {
        if (numArguments < maxArguments)
        {
            expression[numArguments] = argument;
        }
        argument = argument->nextExpression;
        ++numArguments;
    }
    return numArguments;
}

GLSLGenerator::GLSLGenerator() :
    m_writer(/* writeFileNames= */ false)
{
    m_tree                      = NULL;
    m_entryName                 = NULL;
    m_target                    = Target_VertexShader;
#if USE_GLES
    m_version                   = Version_300_ES;
#else
    m_version                   = Version_330;
#endif
	
    m_versionLegacy             = false;
    m_inAttribPrefix            = NULL;
    m_outAttribPrefix           = NULL;
    m_error                     = false;
    m_matrixRowFunction[0]      = 0;
    m_matrixCtorFunction[0]     = 0;
    m_matrixMulFunction[0]      = 0;
    m_clipFunction[0]           = 0;
    m_tex2DlodFunction[0]       = 0;
    m_tex2DbiasFunction[0]      = 0;
    m_tex3DlodFunction[0]       = 0;
    m_texCUBEbiasFunction[0]    = 0;
	m_texCUBElodFunction[ 0 ] 	= 0;
    m_scalarSwizzle2Function[0] = 0;
    m_scalarSwizzle3Function[0] = 0;
    m_scalarSwizzle4Function[0] = 0;
    m_sinCosFunction[0]         = 0;
	m_bvecTernary[ 0 ]			= 0;
    m_modfFunction[0]           = 0;
    m_outputPosition            = false;
    m_outputTargets             = 0;
}

bool GLSLGenerator::Generate(HLSLTree* tree, Target target, Version version, const char* entryName, const Options& options)
{

    m_tree      = tree;
    m_entryName = entryName;
    m_target    = target;
    m_version   = version;
    m_versionLegacy = (version == Version_110 || version == Version_120 || version == Version_100_ES);
    m_options   = options;

    globalVarsAssignments.clear();

    ChooseUniqueName("matrix_row", m_matrixRowFunction, sizeof(m_matrixRowFunction));
    ChooseUniqueName("matrix_ctor", m_matrixCtorFunction, sizeof(m_matrixCtorFunction));
    ChooseUniqueName("matrix_mul", m_matrixMulFunction, sizeof(m_matrixMulFunction));
    ChooseUniqueName("clip", m_clipFunction, sizeof(m_clipFunction));
    ChooseUniqueName("tex2Dlod", m_tex2DlodFunction, sizeof(m_tex2DlodFunction));
    ChooseUniqueName("tex2Dbias", m_tex2DbiasFunction, sizeof(m_tex2DbiasFunction));
    ChooseUniqueName("tex2Dgrad", m_tex2DgradFunction, sizeof(m_tex2DgradFunction));
    ChooseUniqueName("tex3Dlod", m_tex3DlodFunction, sizeof(m_tex3DlodFunction));
    ChooseUniqueName("texCUBEbias", m_texCUBEbiasFunction, sizeof(m_texCUBEbiasFunction));
	ChooseUniqueName( "texCUBElod", m_texCUBElodFunction, sizeof( m_texCUBElodFunction ) );
	ChooseUniqueName( "modf", m_modfFunction, sizeof( m_modfFunction ) );

    for (int i = 0; i < s_numReservedWords; ++i)
    {
        ChooseUniqueName( s_reservedWord[i], m_reservedWord[i], sizeof(m_reservedWord[i]) );
    }

    ChooseUniqueName("m_scalar_swizzle2", m_scalarSwizzle2Function, sizeof(m_scalarSwizzle2Function));
    ChooseUniqueName("m_scalar_swizzle3", m_scalarSwizzle3Function, sizeof(m_scalarSwizzle3Function));
    ChooseUniqueName("m_scalar_swizzle4", m_scalarSwizzle4Function, sizeof(m_scalarSwizzle4Function));

    ChooseUniqueName("sincos", m_sinCosFunction, sizeof(m_sinCosFunction));

	ChooseUniqueName( "bvecTernary", m_bvecTernary, sizeof( m_bvecTernary ) );

    if (target == Target_VertexShader)
    {
        m_inAttribPrefix  = "";
        m_outAttribPrefix = "frag_";
    }
    else
    {
        m_inAttribPrefix  = "frag_";
        m_outAttribPrefix = "rast_";
    }

    m_tree->ReplaceUniformsAssignments();

    HLSLRoot* root = m_tree->GetRoot();
    HLSLStatement* statement = root->statement;

    // Find the entry point function.
    HLSLFunction* entryFunction = FindFunction(root, m_entryName);
    if (entryFunction == NULL)
    {
        Error("Entry point '%s' doesn't exist", m_entryName);
        return false;
    }

    if (m_version == Version_110)
    {
        m_writer.WriteLine(0, "#version 110");
    }
    if (m_version == Version_120)
    {
        m_writer.WriteLine(0, "#version 120");
    }
    else if (m_version == Version_140)
    {
        m_writer.WriteLine(0, "#version 140");

        // Pragmas for NVIDIA.
        m_writer.WriteLine(0, "#pragma optionNV(fastmath on)");
        //m_writer.WriteLine(0, "#pragma optionNV(fastprecision on)");
        m_writer.WriteLine(0, "#pragma optionNV(ifcvt none)");
        m_writer.WriteLine(0, "#pragma optionNV(inline all)");
        m_writer.WriteLine(0, "#pragma optionNV(strict on)");
        m_writer.WriteLine(0, "#pragma optionNV(unroll all)");
    }
    else if (m_version == Version_150)
    {
        m_writer.WriteLine(0, "#version 150");
    }
    else if (m_version == Version_330)
    {
        m_writer.WriteLine(0, "#version 330");
    }
    else if (m_version == Version_100_ES)
    {
        m_writer.WriteLine(0, "#version 100");
        m_writer.WriteLine(0, "precision highp float;");
    }
    else if (m_version == Version_300_ES)
    {  
        m_writer.WriteLine(0, "#version 300 es");
        m_writer.WriteLine(0, "precision highp float;");
        m_writer.WriteLine(0, "precision highp sampler3D;");
    }
    else
    {
        Error("Unrecognized target version");
        return false;
    }

    // Output the special function used to access rows in a matrix.
    m_writer.WriteLine(0, "vec2 %s(mat2 m, int i) { return vec2( m[0][i], m[1][i] ); }", m_matrixRowFunction);
    m_writer.WriteLine(0, "vec3 %s(mat3 m, int i) { return vec3( m[0][i], m[1][i], m[2][i] ); }", m_matrixRowFunction);
    m_writer.WriteLine(0, "vec4 %s(mat4 m, int i) { return vec4( m[0][i], m[1][i], m[2][i], m[3][i] ); }", m_matrixRowFunction);

    // Output the special function used to do matrix cast for OpenGL 2.0
    if (m_versionLegacy)
    {
        m_writer.WriteLine(0, "mat3 %s(mat4 m) { return mat3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]); }", m_matrixCtorFunction);
    }

    // Output the special functions used for matrix multiplication lowering
    // They make sure glsl-optimizer can fold expressions better
    if (m_tree->NeedsFunction("mul") && (m_options.flags & Flag_LowerMatrixMultiplication))
    {
        m_writer.WriteLine(0, "vec2 %s(mat2 m, vec2 v) { return m[0] * v.x + m[1] * v.y; }", m_matrixMulFunction);
        m_writer.WriteLine(0, "vec2 %s(vec2 v, mat2 m) { return vec2(dot(m[0], v), dot(m[1], v)); }", m_matrixMulFunction);
        m_writer.WriteLine(0, "vec3 %s(mat3 m, vec3 v) { return m[0] * v.x + m[1] * v.y + m[2] * v.z; }", m_matrixMulFunction);
        m_writer.WriteLine(0, "vec3 %s(vec3 v, mat3 m) { return vec3(dot(m[0], v), dot(m[1], v), dot(m[2], v)); }", m_matrixMulFunction);
        m_writer.WriteLine(0, "vec4 %s(mat4 m, vec4 v) { return m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w; }", m_matrixMulFunction);
        m_writer.WriteLine(0, "vec4 %s(vec4 v, mat4 m) { return vec4(dot(m[0], v), dot(m[1], v), dot(m[2], v), dot(m[3], v)); }", m_matrixMulFunction);
    }

    // Output the special function used to emulate HLSL clip.
    if (m_tree->NeedsFunction("clip"))
    {
        const char* discard = m_target == Target_FragmentShader ? "discard" : "";
        m_writer.WriteLine(0, "void %s(float x) { if (x < 0.0) %s;  }", m_clipFunction, discard);
        m_writer.WriteLine(0, "void %s(vec2  x) { if (any(lessThan(x, vec2(0.0, 0.0)))) %s;  }", m_clipFunction, discard);
        m_writer.WriteLine(0, "void %s(vec3  x) { if (any(lessThan(x, vec3(0.0, 0.0, 0.0)))) %s;  }", m_clipFunction, discard);
        m_writer.WriteLine(0, "void %s(vec4  x) { if (any(lessThan(x, vec4(0.0, 0.0, 0.0, 0.0)))) %s;  }", m_clipFunction, discard);
    }

    // Output the special function used to emulate tex2Dlod.
    if (m_tree->NeedsFunction("tex2Dlod"))
    {
        const char* function = "textureLod";

        if (m_versionLegacy)
        {
            m_writer.WriteLine(0, "#extension GL_ARB_shader_texture_lod : require");
            function = "texture2DLod";
        }
        else if (m_version == Version_100_ES)
        {
            m_writer.WriteLine(0, "#extension GL_EXT_shader_texture_lod : require");
            function = "texture2DLodEXT";
        }

        m_writer.WriteLine(0, "vec4 %s(sampler2D samp, vec4 texCoord) { return %s(samp, texCoord.xy, texCoord.w);  }", m_tex2DlodFunction, function);
    }

    // Output the special function used to emulate tex2Dgrad.
    if (m_tree->NeedsFunction("tex2Dgrad"))
    {
        const char* function = "textureGrad";

        if (m_versionLegacy)
        {
            m_writer.WriteLine(0, "#extension GL_ARB_shader_texture_lod : require");
            function = "texture2DGradARB";
        }
        else if (m_version == Version_100_ES)
        {
            m_writer.WriteLine(0, "#extension GL_EXT_shader_texture_lod : require");
            function = "texture2DGradEXT";
        }

        m_writer.WriteLine(0, "vec4 %s(sampler2D samp, vec2 texCoord, vec2 dx, vec2 dy) { return %s(samp, texCoord, dx, dy);  }", m_tex2DgradFunction, function);
    }

    // Output the special function used to emulate tex2Dbias.
    if (m_tree->NeedsFunction("tex2Dbias"))
    {
        if (target == Target_FragmentShader)
        {
            m_writer.WriteLine(0, "vec4 %s(sampler2D samp, vec4 texCoord) { return %s(samp, texCoord.xy, texCoord.w);  }", m_tex2DbiasFunction, m_versionLegacy ? "texture2D" : "texture" );
        }
        else
        {
            // Bias value is not supported in vertex shader.
            m_writer.WriteLine(0, "vec4 %s(sampler2D samp, vec4 texCoord) { return texture(samp, texCoord.xy);  }", m_tex2DbiasFunction );
        }
    }

    // Output the special function used to emulate tex2DMSfetch.
    if (m_tree->NeedsFunction("tex2DMSfetch"))
    {
        m_writer.WriteLine(0, "vec4 tex2DMSfetch(sampler2DMS samp, ivec2 texCoord, int sample) {");
        m_writer.WriteLine(1, "return texelFetch(samp, texCoord, sample);");
        m_writer.WriteLine(0, "}");
    }

    // Output the special function used to emulate tex3Dlod.
    if (m_tree->NeedsFunction("tex3Dlod"))
    {
        m_writer.WriteLine(0, "vec4 %s(sampler3D samp, vec4 texCoord) { return %s(samp, texCoord.xyz, texCoord.w);  }", m_tex3DlodFunction, m_versionLegacy ? "texture3D" : "texture" );
    }

    // Output the special function used to emulate texCUBEbias.
    if (m_tree->NeedsFunction("texCUBEbias"))
    {
        if (target == Target_FragmentShader)
        {
            m_writer.WriteLine(0, "vec4 %s(samplerCube samp, vec4 texCoord) { return %s(samp, texCoord.xyz, texCoord.w);  }", m_texCUBEbiasFunction, m_versionLegacy ? "textureCube" : "texture" );
        }
        else
        {
            // Bias value is not supported in vertex shader.
            m_writer.WriteLine(0, "vec4 %s(samplerCube samp, vec4 texCoord) { return texture(samp, texCoord.xyz);  }", m_texCUBEbiasFunction );
        }
    }

	// Output the special function used to emulate texCUBElod
	if (m_tree->NeedsFunction("texCUBElod"))
	{
        const char* function = "textureLod";

        if (m_version == Version_110 || m_version == Version_120)
        {
            m_writer.WriteLine(0, "#extension GL_ARB_shader_texture_lod : require");
            function = "textureCubeLod";
        }
        else if (m_version == Version_100_ES)
        {
            m_writer.WriteLine(0, "#extension GL_EXT_shader_texture_lod : require");
            function = "textureCubeLodEXT";
        }

		m_writer.WriteLine( 0, "vec4 %s(samplerCube samp, vec4 texCoord) { return %s(samp, texCoord.xyz, texCoord.w);  }", m_texCUBElodFunction, function);
	}

    if (m_tree->NeedsFunction("modf"))
    {
        if (m_version == Version_110 || m_version == Version_120 || m_version == Version_100_ES)
        {
            m_writer.WriteLine(0, "float %s(float x, out float ip) { ip = int(x); return x - ip; }", m_modfFunction);
        } else {
            m_writer.WriteLine(0, "float %s(float x, out float ip) { return modf(x, ip); }", m_modfFunction);
        }
    }

    m_writer.WriteLine(0, "vec2  %s(float x) { return  vec2(x, x); }", m_scalarSwizzle2Function);
    m_writer.WriteLine(0, "ivec2 %s(int   x) { return ivec2(x, x); }", m_scalarSwizzle2Function);

    m_writer.WriteLine(0, "vec3  %s(float x) { return  vec3(x, x, x); }", m_scalarSwizzle3Function);
    m_writer.WriteLine(0, "ivec3 %s(int   x) { return ivec3(x, x, x); }", m_scalarSwizzle3Function);

    m_writer.WriteLine(0, "vec4  %s(float x) { return  vec4(x, x, x, x); }", m_scalarSwizzle4Function);
    m_writer.WriteLine(0, "ivec4 %s(int   x) { return ivec4(x, x, x, x); }", m_scalarSwizzle4Function);

    if (!m_versionLegacy)
    {
        m_writer.WriteLine(0, "uvec2 %s(uint  x) { return uvec2(x, x); }", m_scalarSwizzle2Function);
        m_writer.WriteLine(0, "uvec3 %s(uint  x) { return uvec3(x, x, x); }", m_scalarSwizzle3Function);
        m_writer.WriteLine(0, "uvec4 %s(uint  x) { return uvec4(x, x, x, x); }", m_scalarSwizzle4Function);
    }

    if (m_tree->NeedsFunction("sincos"))
    {
        const char* floatTypes[] = { "float", "vec2", "vec3", "vec4" };
        for (int i = 0; i < 4; ++i)
        {
            m_writer.WriteLine(0, "void %s(%s x, out %s s, out %s c) { s = sin(x); c = cos(x); }", m_sinCosFunction,
                floatTypes[i], floatTypes[i], floatTypes[i]);
        }
    }

	// special function to emulate ?: with bool{2,3,4} condition type
	m_writer.WriteLine( 0, "vec2 %s(bvec2 cond, vec2 trueExpr, vec2 falseExpr) { vec2 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; return ret; }", m_bvecTernary );
	m_writer.WriteLine( 0, "vec3 %s(bvec3 cond, vec3 trueExpr, vec3 falseExpr) { vec3 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; ret.z = cond.z ? trueExpr.z : falseExpr.z; return ret; }", m_bvecTernary );
	m_writer.WriteLine( 0, "vec4 %s(bvec4 cond, vec4 trueExpr, vec4 falseExpr) { vec4 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; ret.z = cond.z ? trueExpr.z : falseExpr.z; ret.w = cond.w ? trueExpr.w : falseExpr.w; return ret; }", m_bvecTernary );

    m_tree->EnumerateMatrixCtorsNeeded(matrixCtors);
    for(matrixCtor & ctor : matrixCtors)
    {
        std::string id = GetTypeName(HLSLType(ctor.matrixType));
        id += "_from";
        for(HLSLBaseType argType : ctor.argumentTypes)
        {
            id += "_";
            id += GetTypeName(HLSLType(argType));
        }
        matrixCtorsId[ctor] = id;
    }

    OutputMatrixCtors();

    // Output the extension used for dFdx/dFdy in GLES2
    if (m_version == Version_100_ES && (m_tree->NeedsFunction("ddx") || m_tree->NeedsFunction("ddy")))
    {
        m_writer.WriteLine(0, "#extension GL_OES_standard_derivatives : require");
    }

    OutputAttributes(entryFunction);

    if (m_target == Target_FragmentShader)
    {
        if (!m_outputTargets)
            Error("Fragment shader must output a color");

        if (!m_versionLegacy)
            m_writer.WriteLine(0, "out vec4 rast_FragData[%d];", m_outputTargets);
    }

    OutputStatements(0, statement);
    OutputEntryCaller(entryFunction);

    m_tree = NULL;

    // The GLSL compilers don't check for this, so generate our own error message.
    if (target == Target_VertexShader && !m_outputPosition)
    {
        Error("Vertex shader must output a position");
    }

    return !m_error;

}

const char* GLSLGenerator::GetResult() const
{
    return m_writer.GetResult();
}

void GLSLGenerator::OutputExpressionList(HLSLExpression* expression, HLSLArgument* argument)
{
    int numExpressions = 0;
    while (expression != NULL)
    {
        if (numExpressions > 0)
        {
            m_writer.Write(", ");
        }
        
        HLSLType* expectedType = NULL;
        if (argument != NULL)
        {
            expectedType = &argument->type;
            argument = argument->nextArgument;
        }

        OutputExpression(expression, expectedType);
        expression = expression->nextExpression;
        ++numExpressions;
    }
}

const HLSLType* commonScalarType(const HLSLType& lhs, const HLSLType& rhs)
{
    if (!IsScalarType(lhs) || !IsScalarType(rhs))
        return NULL;

    if (lhs.baseType == HLSLBaseType_Float || rhs.baseType == HLSLBaseType_Float)
        return &kFloatType;

    if (lhs.baseType == HLSLBaseType_Uint || rhs.baseType == HLSLBaseType_Uint)
        return &kUintType;

    if (lhs.baseType == HLSLBaseType_Int || rhs.baseType == HLSLBaseType_Int)
        return &kIntType;

    if (lhs.baseType == HLSLBaseType_Bool || rhs.baseType == HLSLBaseType_Bool)
        return &kBoolType;

    return NULL;
}

void GLSLGenerator::OutputExpression(HLSLExpression* expression, const HLSLType* dstType)
{

    bool cast = dstType != NULL && !GetCanImplicitCast(expression->expressionType, *dstType);
    if (expression->nodeType == HLSLNodeType_CastingExpression)
    {
        // No need to include a cast if the expression is already doing it.
        cast = false;
    }

    if (cast)
    {
        OutputCast(*dstType);
        m_writer.Write("(");
    }

    HLSLBuffer* bufferAccess = (m_options.flags & Flag_EmulateConstantBuffer) ? GetBufferAccessExpression(expression) : 0;

    if (bufferAccess)
    {
        OutputBufferAccessExpression(bufferAccess, expression, expression->expressionType, 0);
    }
    else if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression* identifierExpression = static_cast<HLSLIdentifierExpression*>(expression);
        OutputIdentifier(identifierExpression->name);
    }
    else if (expression->nodeType == HLSLNodeType_ConstructorExpression)
    {
        HLSLConstructorExpression* constructorExpression = static_cast<HLSLConstructorExpression*>(expression);

        bool matrixCtorNeeded = false;
        if (IsMatrixType(constructorExpression->type.baseType))
        {
            matrixCtor ctor = matrixCtorBuilder(constructorExpression->type, constructorExpression->argument);
            if (std::find(matrixCtors.cbegin(), matrixCtors.cend(), ctor) != matrixCtors.cend())
            {
                matrixCtorNeeded = true;
            }
        }

        if (matrixCtorNeeded)
        {
            // Matrix contructors needs to be adapted since GLSL access a matrix as m[c][r] while HLSL is m[r][c]
            matrixCtor ctor = matrixCtorBuilder(constructorExpression->type, constructorExpression->argument);
            m_writer.Write("%s(", matrixCtorsId[ctor].c_str());
            OutputExpressionList(constructorExpression->argument);
            m_writer.Write(")");
        }
        else
        {
            m_writer.Write("%s(", GetTypeName(constructorExpression->type));
            OutputExpressionList(constructorExpression->argument);
            m_writer.Write(")");
        }
    }
    else if (expression->nodeType == HLSLNodeType_CastingExpression)
    {
        HLSLCastingExpression* castingExpression = static_cast<HLSLCastingExpression*>(expression);
        OutputCast(castingExpression->type);
        m_writer.Write("(");
        OutputExpression(castingExpression->expression);
        m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_LiteralExpression)
    {
        HLSLLiteralExpression* literalExpression = static_cast<HLSLLiteralExpression*>(expression);
        switch (literalExpression->type)
        {
        case HLSLBaseType_Float:
            {
                // Don't use printf directly so that we don't use the system locale.
                char buffer[64];
                String_FormatFloat(buffer, sizeof(buffer), literalExpression->fValue);
                m_writer.Write("%s", buffer);
            }
            break;
        case HLSLBaseType_Int:
        case HLSLBaseType_Uint:
            m_writer.Write("%d", literalExpression->iValue);
            break;
        case HLSLBaseType_Bool:
            m_writer.Write("%s", literalExpression->bValue ? "true" : "false");
            break;
        default:
            ASSERT(0);
        }
    }
    else if (expression->nodeType == HLSLNodeType_UnaryExpression)
    {
        HLSLUnaryExpression* unaryExpression = static_cast<HLSLUnaryExpression*>(expression);
        const char* op = "?";
        bool pre = true;
        const HLSLType* dstType2 = NULL;
        switch (unaryExpression->unaryOp)
        {
        case HLSLUnaryOp_Negative:      op = "-";  break;
        case HLSLUnaryOp_Positive:      op = "+";  break;
        case HLSLUnaryOp_Not:           op = "!";  dstType2 = &unaryExpression->expressionType; break;
        case HLSLUnaryOp_PreIncrement:  op = "++"; break;
        case HLSLUnaryOp_PreDecrement:  op = "--"; break;
        case HLSLUnaryOp_PostIncrement: op = "++"; pre = false; break;
        case HLSLUnaryOp_PostDecrement: op = "--"; pre = false; break;
        case HLSLUnaryOp_BitNot:        op = "~";  break;
        }
        m_writer.Write("(");
        if (pre)
        {
            m_writer.Write("%s", op);
            OutputExpression(unaryExpression->expression, dstType2);
        }
        else
        {
            OutputExpression(unaryExpression->expression, dstType2);
            m_writer.Write("%s", op);
        }
        m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_BinaryExpression)
    {
        HLSLBinaryExpression* binaryExpression = static_cast<HLSLBinaryExpression*>(expression);
        const char* op = "?";
        const HLSLType* dstType1 = NULL;
        const HLSLType* dstType2 = NULL;

		//
		bool vectorExpression = IsVectorType( binaryExpression->expression1->expressionType ) || IsVectorType( binaryExpression->expression2->expressionType );
		if( vectorExpression && IsCompareOp( binaryExpression->binaryOp ))
		{
			switch (binaryExpression->binaryOp)
			{
			case HLSLBinaryOp_Less:         m_writer.Write("lessThan(");			break;
			case HLSLBinaryOp_Greater:      m_writer.Write("greaterThan(");			break;
			case HLSLBinaryOp_LessEqual:    m_writer.Write("lessThanEqual(");		break;
			case HLSLBinaryOp_GreaterEqual: m_writer.Write("greaterThanEqual(");	break;
			case HLSLBinaryOp_Equal:        m_writer.Write("equal(");				break;
			case HLSLBinaryOp_NotEqual:     m_writer.Write("notEqual(");			break;
			default:
				ASSERT(0); // is so, check isCompareOp
			}

			if( IsVectorType( binaryExpression->expression1->expressionType ) && IsScalarType( binaryExpression->expression2->expressionType ) )
				dstType2 = &binaryExpression->expression1->expressionType;
			else if( IsScalarType( binaryExpression->expression1->expressionType ) && IsVectorType( binaryExpression->expression2->expressionType ) )
				dstType1 = &binaryExpression->expression2->expressionType;
			// TODO if both expressions are vector but with different dimension handle it here or in parser?

			OutputExpression(binaryExpression->expression1, dstType1);
			m_writer.Write(", ");
			OutputExpression(binaryExpression->expression2, dstType2);
			m_writer.Write(")");
		}
		else
		{
			switch (binaryExpression->binaryOp)
			{
			case HLSLBinaryOp_Add:          op = " + "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Sub:          op = " - "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Mul:          op = " * "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Div:          op = " / "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
            case HLSLBinaryOp_Mod:          op = " % "; dstType1 = dstType2 = &kIntType; break;
			case HLSLBinaryOp_Less:         op = " < "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_Greater:      op = " > "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_LessEqual:    op = " <= "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_GreaterEqual: op = " >= "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_Equal:        op = " == "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_NotEqual:     op = " != "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_Assign:       op = " = ";  dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_AddAssign:    op = " += "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_SubAssign:    op = " -= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_MulAssign:    op = " *= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_DivAssign:    op = " /= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_And:          op = " && "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Or:           op = " || "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_BitAnd:       op = " & "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_BitOr:        op = " | "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			case HLSLBinaryOp_BitXor:       op = " ^ "; dstType1 = dstType2 = commonScalarType(binaryExpression->expression1->expressionType, binaryExpression->expression2->expressionType); break;
			default:
				ASSERT(0);
			}
            if ((m_version == Version_110 || m_version == Version_120 || m_version == Version_100_ES) && binaryExpression->binaryOp == HLSLBinaryOp_Mod) {
                m_writer.Write("(int(mod(");
                OutputExpression(binaryExpression->expression1, dstType1);
                m_writer.Write(",");
                OutputExpression(binaryExpression->expression2, dstType2);
                m_writer.Write(")))");
            } else {
			    m_writer.Write("(");
			    OutputExpression(binaryExpression->expression1, dstType1);
			    m_writer.Write("%s", op);
			    OutputExpression(binaryExpression->expression2, dstType2);
			    m_writer.Write(")");
            }
		}
    }
    else if (expression->nodeType == HLSLNodeType_ConditionalExpression)
    {
        HLSLConditionalExpression* conditionalExpression = static_cast<HLSLConditionalExpression*>(expression);
		if( IsVectorType( conditionalExpression->condition->expressionType ) )
		{
			m_writer.Write( "%s", m_bvecTernary );
			m_writer.Write( "( " );
			OutputExpression( conditionalExpression->condition );
			m_writer.Write( ", " );
			OutputExpression( conditionalExpression->trueExpression, &conditionalExpression->expressionType );
			m_writer.Write( ", " );
			OutputExpression( conditionalExpression->falseExpression, &conditionalExpression->expressionType  );
			m_writer.Write( " )" );
		}
		else
		{
			m_writer.Write( "((" );
			OutputExpression( conditionalExpression->condition, &kBoolType );
			m_writer.Write( ")?(" );
			OutputExpression( conditionalExpression->trueExpression, dstType );
			m_writer.Write( "):(" );
			OutputExpression( conditionalExpression->falseExpression, dstType );
			m_writer.Write( "))" );
		}
    }
    else if (expression->nodeType == HLSLNodeType_MemberAccess)
    {

        HLSLMemberAccess* memberAccess = static_cast<HLSLMemberAccess*>(expression);

        if (memberAccess->object->expressionType.baseType == HLSLBaseType_Float ||
            memberAccess->object->expressionType.baseType == HLSLBaseType_Int   ||
            memberAccess->object->expressionType.baseType == HLSLBaseType_Uint)
        {
            // Handle swizzling on scalar values.
            size_t swizzleLength = strlen(memberAccess->field);
            if (swizzleLength == 2)
            {
                m_writer.Write("%s", m_scalarSwizzle2Function);
            }
            else if (swizzleLength == 3)
            {
                m_writer.Write("%s", m_scalarSwizzle3Function);
            }
            else if (swizzleLength == 4)
            {
                m_writer.Write("%s", m_scalarSwizzle4Function);
            }
            m_writer.Write("(");
            OutputExpression(memberAccess->object);
            m_writer.Write(")");
        }
        else
        {
            m_writer.Write("(");
            OutputExpression(memberAccess->object);
            m_writer.Write(")");

            if( IsMatrixType(memberAccess->object->expressionType.baseType))
            {
                // Handle HLSL matrix "swizzling".
                // TODO: Properly handle multiple element selection such as _m00_m12
                const char* n = memberAccess->field;
                while (n[0] != 0)
                {
                    if ( n[0] != '_' )
                    {
                        ASSERT(0);
                        break;
                    }
                    ++n;
                    char base = '1';
                    if (n[0] == 'm')
                    {
                        base = '0';
                        ++n;
                    }
                    if (isdigit(n[0]) && isdigit(n[1]) )
                    {
                        m_writer.Write("[%d][%d]", n[1] - base, n[0] - base);
                        n += 2;
                    }
                    else
                    {
                        ASSERT(0);
                        break;
                    }
                }
            }
            else
            {
                m_writer.Write(".%s", memberAccess->field);
            }

        }

    }
    else if (expression->nodeType == HLSLNodeType_ArrayAccess)
    {
        HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(expression);

        if (!arrayAccess->array->expressionType.array &&
            IsMatrixType(arrayAccess->array->expressionType.baseType) )
        {
            // GLSL access a matrix as m[c][r] while HLSL is m[r][c], so use our
            // special row access function to convert.
            m_writer.Write("%s(", m_matrixRowFunction);
            OutputExpression(arrayAccess->array);
            m_writer.Write(",");
            OutputExpression(arrayAccess->index);
            m_writer.Write(")");
        }
        else
        {
            OutputExpression(arrayAccess->array);
            m_writer.Write("[");
            OutputExpression(arrayAccess->index);
            m_writer.Write("]");
        }

    }
    else if (expression->nodeType == HLSLNodeType_FunctionCall)
    {
        HLSLFunctionCall* functionCall = static_cast<HLSLFunctionCall*>(expression);

        // Handle intrinsic funtions that are different between HLSL and GLSL.
        bool handled = false;
        const char* functionName = functionCall->function->name;

        if (String_Equal(functionName, "mul"))
        {
            HLSLExpression* argument[2];
            if (GetFunctionArguments(functionCall, argument, 2) != 2)
            {
                Error("mul expects 2 arguments");
                return;
            }

            const HLSLType& type0 = functionCall->function->argument->type;
            const HLSLType& type1 = functionCall->function->argument->nextArgument->type;

            if (IsVectorType(type0.baseType) && IsVectorType(type1.baseType))
            {
                m_writer.Write("dot((");
                OutputExpression(argument[0], &type0);
                m_writer.Write("),(");
                OutputExpression(argument[1], &type1);
                m_writer.Write("))");
            }
            else
            {
                m_writer.Write("((");
                OutputExpression(argument[0], &type0);
                m_writer.Write(")*(");
                OutputExpression(argument[1], &type1);
                m_writer.Write("))");
            }

            handled = true;
        }
        else if (String_Equal(functionName, "saturate"))
        {
            HLSLExpression* argument[1];
            if (GetFunctionArguments(functionCall, argument, 1) != 1)
            {
                Error("saturate expects 1 argument");
                return;
            }
            m_writer.Write("clamp(");
            OutputExpression(argument[0]);
            HLSLBaseType baseType = argument[0]->expressionType.baseType;
            switch (baseType) {
            case HLSLBaseType_Float:
            case HLSLBaseType_Float2:
            case HLSLBaseType_Float3:
            case HLSLBaseType_Float4:
                m_writer.Write(", 0.0, 1.0)");
                break;

            case HLSLBaseType_Int:
            case HLSLBaseType_Int2:
            case HLSLBaseType_Int3:
            case HLSLBaseType_Int4:
            case HLSLBaseType_Uint:
            case HLSLBaseType_Uint2:
            case HLSLBaseType_Uint3:
            case HLSLBaseType_Uint4:
                m_writer.Write(", 0, 1)");
                break;

            default:
                Error("saturate unhandled type: %s", GetTypeName(argument[0]->expressionType));
                break;
            }
            handled = true;
        }
        else if (String_Equal(functionName, "rsqrt"))
        {
            HLSLExpression* argument[1];
            if (GetFunctionArguments(functionCall, argument, 1) != 1)
            {
                Error("rsqrt expects 1 argument");
                return;
            }
            m_writer.Write("inversesqrt(");
            OutputExpression(argument[0]);
            m_writer.Write(")");
            handled = true;
        }

        if (!handled)
        {
            OutputIdentifier(functionName);
            m_writer.Write("(");
            OutputExpressionList(functionCall->argument, functionCall->function->argument);
            m_writer.Write(")");
        }
    }
    else
    {
        m_writer.Write("<unknown expression>");
    }

    if (cast)
    {
        if (IsVectorType(dstType->baseType) || IsMatrixType(dstType->baseType))
        {
            CompleteConstructorArguments(expression, dstType->baseType);
        }

        m_writer.Write(")");
    }

}

void GLSLGenerator::OutputIdentifier(const char* name)
{

    // Remap intrinstic functions.
    if (String_Equal(name, "tex2D"))
    {
        name = m_versionLegacy ? "texture2D" : "texture";
    }
    else if (String_Equal(name, "tex2Dproj"))
    {
        name = m_versionLegacy ? "texture2DProj" : "textureProj";
    }
    else if (String_Equal(name, "texCUBE"))
    {
        name = m_versionLegacy ? "textureCube" : "texture";
    }
    else if (String_Equal(name, "tex3D"))
    {
        name = m_versionLegacy ? "texture3D" : "texture";
    }
    else if (String_Equal(name, "clip"))
    {
        name = m_clipFunction;
    }
    else if (String_Equal(name, "tex2Dlod"))
    {
        name = m_tex2DlodFunction;
    }
    else if (String_Equal(name, "tex2Dbias"))
    {
        name = m_tex2DbiasFunction;
    }
    else if (String_Equal(name, "tex2Dgrad"))
    {
        name = m_tex2DgradFunction;
    }
    else if (String_Equal(name, "tex2DArray"))
    {
        name = "texture";
    }
    else if (String_Equal(name, "texCUBEbias"))
    {
        name = m_texCUBEbiasFunction;
    }
	else if( String_Equal( name, "texCUBElod" ) )
	{
		name = m_texCUBElodFunction;
	}
    else if (String_Equal(name, "atan2"))
    {
        name = "atan";
    }
    else if (String_Equal(name, "sincos"))
    {
        name = m_sinCosFunction;
    }
    else if (String_Equal(name, "fmod"))
    {
        // mod is not the same as fmod if the parameter is negative!
        // The equivalent of fmod(x, y) is x - y * floor(x/y)
        // We use the mod version for performance.
        name = "mod";
    }
    else if (String_Equal(name, "lerp"))
    {
        name = "mix";
    }
    else if (String_Equal(name, "frac"))
    {
        name = "fract";
    }
    else if (String_Equal(name, "ddx"))
    {
        name = "dFdx";
    }
    else if (String_Equal(name, "ddy"))
    {
        name = "dFdy";
    }
    else if (String_Equal(name, "modf"))
    {
        name = m_modfFunction;
    }
    else 
    {
        // The identifier could be a GLSL reserved word (if it's not also a HLSL reserved word).
        name = GetSafeIdentifierName(name);
    }
    m_writer.Write("%s", name);

}

void GLSLGenerator::OutputArguments(HLSLArgument* argument)
{
    int numArgs = 0;
    while (argument != NULL)
    {
        if (numArgs > 0)
        {
            m_writer.Write(", ");
        }

        switch (argument->modifier)
        {
        case HLSLArgumentModifier_In:
            m_writer.Write("in ");
            break;
        case HLSLArgumentModifier_Out:
            m_writer.Write("out ");
            break;
        case HLSLArgumentModifier_Inout:
            m_writer.Write("inout ");
            break;
        default:
            break;
        }

        OutputDeclaration(argument->type, argument->name);
        argument = argument->nextArgument;
        ++numArgs;
    }
}

void GLSLGenerator::OutputStatements(int indent, HLSLStatement* statement, const HLSLType* returnType)
{

    while (statement != NULL)
    {
        if (statement->hidden)
        {
            statement = statement->nextStatement;
            continue;
        }

        if (statement->nodeType == HLSLNodeType_Declaration)
        {
            HLSLDeclaration* declaration = static_cast<HLSLDeclaration*>(statement);

            // GLSL doesn't seem have texture uniforms, so just ignore them.
            if (declaration->type.baseType != HLSLBaseType_Texture)
            {
                bool skipAssignment = true;
                if (indent != 0)
                {
                    skipAssignment = false;
                }

                m_writer.BeginLine(indent, declaration->fileName, declaration->line);
                if (indent == 0 && (declaration->type.flags & HLSLTypeFlag_Uniform))
                {
                    // At the top level, we need the "uniform" keyword.
                    m_writer.Write("uniform ");
                    skipAssignment = false;
                }
                OutputDeclaration(declaration, skipAssignment);
                m_writer.EndLine(";");
            }
        }
        else if (statement->nodeType == HLSLNodeType_Struct)
        {
            HLSLStruct* structure = static_cast<HLSLStruct*>(statement);
            m_writer.WriteLine(indent, "struct %s {", structure->name);
            HLSLStructField* field = structure->field;
            while (field != NULL)
            {
                m_writer.BeginLine(indent + 1, field->fileName, field->line);
                OutputDeclaration(field->type, field->name);
                m_writer.Write(";");
                m_writer.EndLine();
                field = field->nextField;
            }
            m_writer.WriteLine(indent, "};");
        }
        else if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer* buffer = static_cast<HLSLBuffer*>(statement);
            OutputBuffer(indent, buffer);
        }
        else if (statement->nodeType == HLSLNodeType_Function)
        {
            HLSLFunction* function = static_cast<HLSLFunction*>(statement);

            // Use an alternate name for the function which is supposed to be entry point
            // so that we can supply our own function which will be the actual entry point.
            const char* functionName   = GetSafeIdentifierName(function->name);
            const char* returnTypeName = GetTypeName(function->returnType);

            m_writer.BeginLine(indent, function->fileName, function->line);
            m_writer.Write("%s %s(", returnTypeName, functionName);

            OutputArguments(function->argument);

            if (function->forward)
            {
                m_writer.WriteLine(indent, ");");
            }
            else
            {
                m_writer.Write(") {");
                m_writer.EndLine();

                OutputStatements(indent + 1, function->statement, &function->returnType);
                m_writer.WriteLine(indent, "}");
            }
        }
        else if (statement->nodeType == HLSLNodeType_ExpressionStatement)
        {
            HLSLExpressionStatement* expressionStatement = static_cast<HLSLExpressionStatement*>(statement);
            m_writer.BeginLine(indent, statement->fileName, statement->line);
            OutputExpression(expressionStatement->expression);
            m_writer.EndLine(";");
        }
        else if (statement->nodeType == HLSLNodeType_ReturnStatement)
        {
            HLSLReturnStatement* returnStatement = static_cast<HLSLReturnStatement*>(statement);
            if (returnStatement->expression != NULL)
            {
                m_writer.BeginLine(indent, returnStatement->fileName, returnStatement->line);
                m_writer.Write("return ");
                OutputExpression(returnStatement->expression, returnType);
                m_writer.EndLine(";");
            }
            else
            {
                m_writer.WriteLineTagged(indent, returnStatement->fileName, returnStatement->line, "return;");
            }
        }
        else if (statement->nodeType == HLSLNodeType_DiscardStatement)
        {
            HLSLDiscardStatement* discardStatement = static_cast<HLSLDiscardStatement*>(statement);
            if (m_target == Target_FragmentShader)
            {
                m_writer.WriteLineTagged(indent, discardStatement->fileName, discardStatement->line, "discard;");
            }
        }
        else if (statement->nodeType == HLSLNodeType_BreakStatement)
        {
            HLSLBreakStatement* breakStatement = static_cast<HLSLBreakStatement*>(statement);
            m_writer.WriteLineTagged(indent, breakStatement->fileName, breakStatement->line, "break;");
        }
        else if (statement->nodeType == HLSLNodeType_ContinueStatement)
        {
            HLSLContinueStatement* continueStatement = static_cast<HLSLContinueStatement*>(statement);
            m_writer.WriteLineTagged(indent, continueStatement->fileName, continueStatement->line, "continue;");
        }
        else if (statement->nodeType == HLSLNodeType_IfStatement)
        {
            HLSLIfStatement* ifStatement = static_cast<HLSLIfStatement*>(statement);
            m_writer.BeginLine(indent, ifStatement->fileName, ifStatement->line);
            m_writer.Write("if (");
            OutputExpression(ifStatement->condition, &kBoolType);
            m_writer.Write(") {");
            m_writer.EndLine();
            OutputStatements(indent + 1, ifStatement->statement, returnType);
            m_writer.WriteLine(indent, "}");
            if (ifStatement->elseStatement != NULL)
            {
                m_writer.WriteLine(indent, "else {");
                OutputStatements(indent + 1, ifStatement->elseStatement, returnType);
                m_writer.WriteLine(indent, "}");
            }
        }
        else if (statement->nodeType == HLSLNodeType_ForStatement)
        {
            HLSLForStatement* forStatement = static_cast<HLSLForStatement*>(statement);
            m_writer.BeginLine(indent, forStatement->fileName, forStatement->line);
            m_writer.Write("for (");
            if (forStatement->initialization != NULL)
            {
                OutputDeclaration(forStatement->initialization, false);
            }
            else
            {
                OutputExpression(forStatement->initializationWithoutType);
            }
            m_writer.Write("; ");
            OutputExpression(forStatement->condition, &kBoolType);
            m_writer.Write("; ");
            OutputExpression(forStatement->increment);
            m_writer.Write(") {");
            m_writer.EndLine();
            OutputStatements(indent + 1, forStatement->statement, returnType);
            m_writer.WriteLine(indent, "}");
        }
        else if (statement->nodeType == HLSLNodeType_WhileStatement)
        {
            HLSLWhileStatement* whileStatement = static_cast<HLSLWhileStatement*>(statement);
            m_writer.BeginLine(indent, whileStatement->fileName, whileStatement->line);
            m_writer.Write("while (");
            OutputExpression(whileStatement->condition, &kBoolType);
            m_writer.Write(") {");
            m_writer.EndLine();
            OutputStatements(indent + 1, whileStatement->statement, returnType);
            m_writer.WriteLine(indent, "}");
        }
        else if (statement->nodeType == HLSLNodeType_BlockStatement)
        {
            HLSLBlockStatement* blockStatement = static_cast<HLSLBlockStatement*>(statement);
            m_writer.WriteLine(indent, "{");
            OutputStatements(indent + 1, blockStatement->statement, returnType);
            m_writer.WriteLine(indent, "}");
        }
        else
        {
            // Unhanded statement type.
            ASSERT(0);
        }

        statement = statement->nextStatement;

    }

}

void GLSLGenerator::OutputBuffer(int indent, HLSLBuffer* buffer)
{
    // Empty uniform blocks cause compilation errors on NVIDIA, so don't emit them.
    if (buffer->field == NULL)
        return;

    if (m_options.flags & Flag_EmulateConstantBuffer)
    {
        unsigned int size = 0;
        LayoutBuffer(buffer, size);

        unsigned int uniformSize = (size + 3) / 4;

        m_writer.WriteLineTagged(indent, buffer->fileName, buffer->line, "uniform vec4 %s%s[%d];", m_options.constantBufferPrefix, buffer->name, uniformSize);
    }
    else
    {
        m_writer.WriteLineTagged(indent, buffer->fileName, buffer->line, "layout (std140) uniform %s%s {", m_options.constantBufferPrefix, buffer->name);
        HLSLDeclaration* field = buffer->field;
        while (field != NULL)
        {
            m_writer.BeginLine(indent + 1, field->fileName, field->line);
            OutputDeclaration(field->type, field->name);
            m_writer.Write(";");
            m_writer.EndLine();
            field = (HLSLDeclaration*)field->nextStatement;
        }
        m_writer.WriteLine(indent, "};");
    }
}

inline void alignForWrite(unsigned int& offset, unsigned int size)
{
    ASSERT(size <= 4);

    if (offset / 4 != (offset + size - 1) / 4)
        offset = (offset + 3) & ~3;
}

void GLSLGenerator::LayoutBuffer(HLSLBuffer* buffer, unsigned int& offset)
{
    for (HLSLDeclaration* field = buffer->field; field; field = (HLSLDeclaration*)field->nextStatement)
    {
        LayoutBuffer(field->type, offset);
    }
}

void GLSLGenerator::LayoutBuffer(const HLSLType& type, unsigned int& offset)
{
    LayoutBufferAlign(type, offset);

    if (type.array)
    {
        int arraySize = 0;
        m_tree->GetExpressionValue(type.arraySize, arraySize);

        unsigned int elementSize = 0;
        LayoutBufferElement(type, elementSize);

        unsigned int alignedElementSize = (elementSize + 3) & ~3;

        offset += alignedElementSize * arraySize;
    }
    else
    {
        LayoutBufferElement(type, offset);
    }
}

void GLSLGenerator::LayoutBufferElement(const HLSLType& type, unsigned int& offset)
{
    if (type.baseType == HLSLBaseType_Float)
    {
        offset += 1;
    }
    else if (type.baseType == HLSLBaseType_Float2)
    {
        offset += 2;
    }
    else if (type.baseType == HLSLBaseType_Float3)
    {
        offset += 3;
    }
    else if (type.baseType == HLSLBaseType_Float4)
    {
        offset += 4;
    }
    else if (type.baseType == HLSLBaseType_Float4x4)
    {
        offset += 16;
    }
    else if (type.baseType == HLSLBaseType_UserDefined)
    {
        HLSLStruct * st = m_tree->FindGlobalStruct(type.typeName);

        if (st)
        {
            for (HLSLStructField* field = st->field; field; field = field->nextField)
            {
                LayoutBuffer(field->type, offset);
            }
        }
        else
        {
            Error("Unknown type %s", type.typeName);
        }
    }
    else
    {
        Error("Constant buffer layout is not supported for %s", GetTypeName(type));
    }
}

void GLSLGenerator::LayoutBufferAlign(const HLSLType& type, unsigned int& offset)
{
    if (type.array)
    {
        alignForWrite(offset, 4);
    }
    else if (type.baseType == HLSLBaseType_Float)
    {
        alignForWrite(offset, 1);
    }
    else if (type.baseType == HLSLBaseType_Float2)
    {
        alignForWrite(offset, 2);
    }
    else if (type.baseType == HLSLBaseType_Float3)
    {
        alignForWrite(offset, 3);
    }
    else if (type.baseType == HLSLBaseType_Float4)
    {
        alignForWrite(offset, 4);
    }
    else if (type.baseType == HLSLBaseType_Float4x4)
    {
        alignForWrite(offset, 4);
    }
    else if (type.baseType == HLSLBaseType_UserDefined)
    {
        alignForWrite(offset, 4);
    }
    else
    {
        Error("Constant buffer layout is not supported for %s", GetTypeName(type));
    }
}

HLSLBuffer* GLSLGenerator::GetBufferAccessExpression(HLSLExpression* expression)
{
    if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression* identifierExpression = static_cast<HLSLIdentifierExpression*>(expression);

        if (identifierExpression->global)
        {
            HLSLDeclaration * declaration = m_tree->FindGlobalDeclaration(identifierExpression->name);

            if (declaration && declaration->buffer)
                return declaration->buffer;
        }
    }
    else if (expression->nodeType == HLSLNodeType_MemberAccess)
    {
        HLSLMemberAccess* memberAccess = static_cast<HLSLMemberAccess*>(expression);

        if (memberAccess->object->expressionType.baseType == HLSLBaseType_UserDefined)
            return GetBufferAccessExpression(memberAccess->object);
    }
    else if (expression->nodeType == HLSLNodeType_ArrayAccess)
    {
        HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(expression);

        if (arrayAccess->array->expressionType.array)
            return GetBufferAccessExpression(arrayAccess->array);
    }

    return 0;
}

void GLSLGenerator::OutputBufferAccessExpression(HLSLBuffer* buffer, HLSLExpression* expression, const HLSLType& type, unsigned int postOffset)
{
    if (type.array)
    {
        Error("Constant buffer access is not supported for arrays (use indexing instead)");
    }
    else if (type.baseType == HLSLBaseType_Float)
    {
        m_writer.Write("%s%s[", m_options.constantBufferPrefix, buffer->name);
        unsigned int index = OutputBufferAccessIndex(expression, postOffset);
        m_writer.Write("%d].%c", index / 4, "xyzw"[index % 4]);
    }
    else if (type.baseType == HLSLBaseType_Float2)
    {
        m_writer.Write("%s%s[", m_options.constantBufferPrefix, buffer->name);
        unsigned int index = OutputBufferAccessIndex(expression, postOffset);
        m_writer.Write("%d].%s", index / 4, index % 4 == 0 ? "xy" : index % 4 == 1 ? "yz" : "zw");
    }
    else if (type.baseType == HLSLBaseType_Float3)
    {
        m_writer.Write("%s%s[", m_options.constantBufferPrefix, buffer->name);
        unsigned int index = OutputBufferAccessIndex(expression, postOffset);
        m_writer.Write("%d].%s", index / 4, index % 4 == 0 ? "xyz" : "yzw");
    }
    else if (type.baseType == HLSLBaseType_Float4)
    {
        m_writer.Write("%s%s[", m_options.constantBufferPrefix, buffer->name);
        unsigned int index = OutputBufferAccessIndex(expression, postOffset);
        ASSERT(index % 4 == 0);
        m_writer.Write("%d]", index / 4);
    }
    else if (type.baseType == HLSLBaseType_Float4x4)
    {
        m_writer.Write("mat4(");
        for (int i = 0; i < 4; ++i)
        {
            m_writer.Write("%s%s[", m_options.constantBufferPrefix, buffer->name);
            unsigned int index = OutputBufferAccessIndex(expression, postOffset + i * 4);
            ASSERT(index % 4 == 0);
            m_writer.Write("%d]%c", index / 4, i == 3 ? ')' : ',');
        }
    }
    else if (type.baseType == HLSLBaseType_UserDefined)
    {
        HLSLStruct * st = m_tree->FindGlobalStruct(type.typeName);

        if (st)
        {
            m_writer.Write("%s(", st->name);

            unsigned int offset = postOffset;

            for (HLSLStructField* field = st->field; field; field = field->nextField)
            {
                OutputBufferAccessExpression(buffer, expression, field->type, offset);

                if (field->nextField)
                    m_writer.Write(",");

                LayoutBuffer(field->type, offset);
            }

            m_writer.Write(")");
        }
        else
        {
            Error("Unknown type %s", type.typeName);
        }
    }
    else
    {
        Error("Constant buffer layout is not supported for %s", GetTypeName(type));
    }
}

unsigned int GLSLGenerator::OutputBufferAccessIndex(HLSLExpression* expression, unsigned int postOffset)
{
    if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression* identifierExpression = static_cast<HLSLIdentifierExpression*>(expression);
        ASSERT(identifierExpression->global);

        HLSLDeclaration * declaration = m_tree->FindGlobalDeclaration(identifierExpression->name);
        ASSERT(declaration);

        HLSLBuffer * buffer = declaration->buffer;
        ASSERT(buffer);

        unsigned int offset = 0;

        for (HLSLDeclaration* field = buffer->field; field; field = (HLSLDeclaration*)field->nextStatement)
        {
            if (field == declaration)
            {
                LayoutBufferAlign(field->type, offset);
                break;
            }

            LayoutBuffer(field->type, offset);
        }

        return offset + postOffset;
    }
    else if (expression->nodeType == HLSLNodeType_MemberAccess)
    {
        HLSLMemberAccess* memberAccess = static_cast<HLSLMemberAccess*>(expression);

        const HLSLType& type = memberAccess->object->expressionType;
        ASSERT(type.baseType == HLSLBaseType_UserDefined);

        HLSLStruct * st = m_tree->FindGlobalStruct(type.typeName);

        if (st)
        {
            unsigned int offset = 0;

            for (HLSLStructField* field = st->field; field; field = field->nextField)
            {
                if (field->name == memberAccess->field)
                {
                    LayoutBufferAlign(field->type, offset);
                    break;
                }

                LayoutBuffer(field->type, offset);
            }

            return offset + OutputBufferAccessIndex(memberAccess->object, postOffset);
        }
        else
        {
            Error("Unknown type %s", type.typeName);
        }
    }
    else if (expression->nodeType == HLSLNodeType_ArrayAccess)
    {
        HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(expression);

        const HLSLType& type = arrayAccess->array->expressionType;
        ASSERT(type.array);

        unsigned int elementSize = 0;
        LayoutBufferElement(type, elementSize);

        unsigned int alignedElementSize = (elementSize + 3) & ~3;

        int arrayIndex = 0;
        if (m_tree->GetExpressionValue(arrayAccess->index, arrayIndex))
        {
            unsigned int offset = arrayIndex * alignedElementSize;

            return offset + OutputBufferAccessIndex(arrayAccess->array, postOffset);
        }
        else
        {
            m_writer.Write("%d*(", alignedElementSize / 4);
            OutputExpression(arrayAccess->index);
            m_writer.Write(")+");

            return OutputBufferAccessIndex(arrayAccess->array, postOffset);
        }
    }
    else
    {
        ASSERT(!"IsBufferAccessExpression should have returned false");
    }

    return 0;
}

HLSLFunction* GLSLGenerator::FindFunction(HLSLRoot* root, const char* name)
{
    HLSLStatement* statement = root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Function)
        {
            HLSLFunction* function = static_cast<HLSLFunction*>(statement);
            if (String_Equal(function->name, name))
            {
                return function;
            }
        }
        statement = statement->nextStatement;
    }
    return NULL;
}

HLSLStruct* GLSLGenerator::FindStruct(HLSLRoot* root, const char* name)
{
    HLSLStatement* statement = root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Struct)
        {
            HLSLStruct* structDeclaration = static_cast<HLSLStruct*>(statement);
            if (String_Equal(structDeclaration->name, name))
            {
                return structDeclaration;
            }
        }
        statement = statement->nextStatement;
    }
    return NULL;
}


const char* GLSLGenerator::GetAttribQualifier(AttributeModifier modifier)
{
    if (m_versionLegacy)
    {
        if (m_target == Target_VertexShader)
            return (modifier == AttributeModifier_In) ? "attribute" : "varying";
        else
            return (modifier == AttributeModifier_In) ? "varying" : "out";
    }
    else
    {
        return (modifier == AttributeModifier_In) ? "in" : "out";
    }
}

void GLSLGenerator::OutputAttribute(const HLSLType& type, const char* semantic, AttributeModifier modifier)
{
    const char* qualifier = GetAttribQualifier(modifier);
    const char* prefix = (modifier == AttributeModifier_In) ? m_inAttribPrefix : m_outAttribPrefix;

    HLSLRoot* root = m_tree->GetRoot();
    if (type.baseType == HLSLBaseType_UserDefined)
    {
        // If the argument is a struct with semantics specified, we need to
        // grab them.
        HLSLStruct* structDeclaration = FindStruct(root, type.typeName);
        ASSERT(structDeclaration != NULL);
        HLSLStructField* field = structDeclaration->field;
        while (field != NULL)
        {
            if (field->semantic != NULL && GetBuiltInSemantic(field->semantic, modifier) == NULL)
            {
                m_writer.Write( "%s ", qualifier );
				char attribName[ 64 ];
				String_Printf( attribName, 64, "%s%s", prefix, field->semantic );
				OutputDeclaration( field->type, attribName );
				m_writer.EndLine(";");
            }
            field = field->nextField;
        }
    }
    else if (semantic != NULL && GetBuiltInSemantic(semantic, modifier) == NULL)
    {
		m_writer.Write( "%s ", qualifier );
		char attribName[ 64 ];
		String_Printf( attribName, 64, "%s%s", prefix, semantic );
		OutputDeclaration( type, attribName );
		m_writer.EndLine(";");
    }
}

void GLSLGenerator::OutputAttributes(HLSLFunction* entryFunction)
{
    // Write out the input/output attributes to the shader.
    HLSLArgument* argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (argument->modifier == HLSLArgumentModifier_None || argument->modifier == HLSLArgumentModifier_In)
            OutputAttribute(argument->type, argument->semantic, AttributeModifier_In);
        if (argument->modifier == HLSLArgumentModifier_Out)
            OutputAttribute(argument->type, argument->semantic, AttributeModifier_Out);

        argument = argument->nextArgument;
    }

    // Write out the output attributes from the shader.
    OutputAttribute(entryFunction->returnType, entryFunction->semantic, AttributeModifier_Out);
}

void GLSLGenerator::OutputSetOutAttribute(const char* semantic, const char* resultName)
{
    int outputIndex = -1;
    const char* builtInSemantic = GetBuiltInSemantic(semantic, AttributeModifier_Out, &outputIndex);
    if (builtInSemantic != NULL)
    {
        if (String_Equal(builtInSemantic, "gl_Position"))
        {
            if (m_options.flags & Flag_FlipPositionOutput)
            {
                // Mirror the y-coordinate when we're outputing from
                // the vertex shader so that we match the D3D texture
                // coordinate origin convention in render-to-texture
                // operations.
                // We also need to convert the normalized device
                // coordinates from the D3D convention of 0 to 1 to the
                // OpenGL convention of -1 to 1.
                m_writer.WriteLine(1, "vec4 temp = %s;", resultName);
                m_writer.WriteLine(1, "%s = temp * vec4(1,-1,2,1) - vec4(0,0,temp.w,0);", builtInSemantic);
            }
            else
            {
                m_writer.WriteLine(1, "%s = %s;", builtInSemantic, resultName);
            }

            m_outputPosition = true;
        }
        else if (String_Equal(builtInSemantic, "gl_FragDepth"))
        {
            // If the value goes outside of the 0 to 1 range, the
            // fragment will be rejected unlike in D3D, so clamp it.
            m_writer.WriteLine(1, "%s = clamp(float(%s), 0.0, 1.0);", builtInSemantic, resultName);
        }
        else if (outputIndex >= 0)
        {
            m_writer.WriteLine(1, "%s[%d] = %s;", builtInSemantic, outputIndex, resultName);
        }
        else
        {
            m_writer.WriteLine(1, "%s = %s;", builtInSemantic, resultName);
        }
    }
    else if (m_target == Target_FragmentShader)
    {
        Error("Output attribute %s does not map to any built-ins", semantic);
    }
    else
    {
        m_writer.WriteLine(1, "%s%s = %s;", m_outAttribPrefix, semantic, resultName);
    }
}

void GLSLGenerator::OutputEntryCaller(HLSLFunction* entryFunction)
{
    HLSLRoot* root = m_tree->GetRoot();

    m_writer.WriteLine(0, "void main() {");

    // Create local variables for each of the parameters we'll need to pass
    // into the entry point function.
    HLSLArgument* argument = entryFunction->argument;
    while (argument != NULL)
    {
        m_writer.BeginLine(1);
        OutputDeclaration(argument->type, argument->name);
        m_writer.EndLine(";");

        if (argument->modifier != HLSLArgumentModifier_Out)
        {
            // Set the value for the local variable.
            if (argument->type.baseType == HLSLBaseType_UserDefined)
            {
                HLSLStruct* structDeclaration = FindStruct(root, argument->type.typeName);
                ASSERT(structDeclaration != NULL);
                HLSLStructField* field = structDeclaration->field;
                while (field != NULL)
                {
                    if (field->semantic != NULL)
                    {
                        const char* builtInSemantic = GetBuiltInSemantic(field->semantic, AttributeModifier_In);
                        if (builtInSemantic)
                        {
                            m_writer.WriteLine(1, "%s.%s = %s;", GetSafeIdentifierName(argument->name), GetSafeIdentifierName(field->name), builtInSemantic);
                        }
                        else
                        {
                            m_writer.WriteLine(1, "%s.%s = %s%s;", GetSafeIdentifierName(argument->name), GetSafeIdentifierName(field->name), m_inAttribPrefix, field->semantic);
                        }
                    }
                    field = field->nextField;
                }
            }
            else if (argument->semantic != NULL)
            {
                const char* builtInSemantic = GetBuiltInSemantic(argument->semantic, AttributeModifier_In);
                if (builtInSemantic)
                {
                    m_writer.WriteLine(1, "%s = %s;", GetSafeIdentifierName(argument->name), builtInSemantic);
                }
                else
                {
                    m_writer.WriteLine(1, "%s = %s%s;", GetSafeIdentifierName(argument->name), m_inAttribPrefix, argument->semantic);
                }
            }
        }

        argument = argument->nextArgument;
    }


    // Initialize global variables
    for(HLSLDeclaration *declaration : globalVarsAssignments)
    {
        m_writer.BeginLine(1, declaration->fileName, declaration->line);
        OutputDeclarationBody( declaration->type, GetSafeIdentifierName( declaration->name ) );

        OutputDeclarationAssignment(declaration);
        m_writer.EndLine(";");
    }


    const char* resultName = "result";

    // Call the original entry function.
    m_writer.BeginLine(1);
    if (entryFunction->returnType.baseType != HLSLBaseType_Void)
        m_writer.Write("%s %s = ", GetTypeName(entryFunction->returnType), resultName);
    m_writer.Write("%s(", m_entryName);

    int numArgs = 0;
    argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (numArgs > 0)
        {
            m_writer.Write(", ");
        }

        m_writer.Write("%s", GetSafeIdentifierName(argument->name));

        argument = argument->nextArgument;
        ++numArgs;
    }
    m_writer.EndLine(");");

    // Copy values from the result into the out attributes as necessary.
    argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (argument->modifier == HLSLArgumentModifier_Out && argument->semantic)
            OutputSetOutAttribute(argument->semantic, GetSafeIdentifierName(argument->name));

        argument = argument->nextArgument;
    }

    if (entryFunction->returnType.baseType == HLSLBaseType_UserDefined)
    {
        HLSLStruct* structDeclaration = FindStruct(root, entryFunction->returnType.typeName);
        ASSERT(structDeclaration != NULL);
        HLSLStructField* field = structDeclaration->field;
        while (field != NULL)
        {
            char fieldResultName[1024];
            String_Printf( fieldResultName, sizeof(fieldResultName), "%s.%s", resultName, field->name );
            OutputSetOutAttribute( field->semantic, fieldResultName );
            field = field->nextField;
        }
    }
    else if (entryFunction->semantic != NULL)
    {
        OutputSetOutAttribute(entryFunction->semantic, resultName);
    }

    m_writer.WriteLine(0, "}");
}

void GLSLGenerator::OutputDeclaration(HLSLDeclaration* declaration, const bool skipAssignment)
{
	OutputDeclarationType( declaration->type );

	HLSLDeclaration* lastDecl = nullptr;
	while( declaration )
	{
		if( lastDecl )
			m_writer.Write( ", " );

		OutputDeclarationBody( declaration->type, GetSafeIdentifierName( declaration->name ) );

		if( declaration->assignment != NULL )
		{
            if (!skipAssignment)
            {
                OutputDeclarationAssignment(declaration);
            }
            else
            {
                globalVarsAssignments.push_back(declaration);
            }
        }

		lastDecl = declaration;
		declaration = declaration->nextDeclaration;
	}
}

void GLSLGenerator::OutputDeclarationAssignment(HLSLDeclaration* declaration)
{
   m_writer.Write( " = " );
   if( declaration->type.array )
   {
       m_writer.Write( "%s[]( ", GetTypeName( declaration->type ) );
       OutputExpressionList( declaration->assignment );
       m_writer.Write( " )" );
   }
   else
   {
       bool matrixCtorNeeded = false;
       if (IsMatrixType(declaration->type.baseType))
       {
           matrixCtor ctor = matrixCtorBuilder(declaration->type, declaration->assignment);
           if (std::find(matrixCtors.cbegin(), matrixCtors.cend(), ctor) != matrixCtors.cend())
           {
               matrixCtorNeeded = true;
           }
       }

       if (matrixCtorNeeded)
       {
           // Matrix contructors needs to be adapted since GLSL access a matrix as m[c][r] while HLSL is m[r][c]
           matrixCtor ctor = matrixCtorBuilder(declaration->type, declaration->assignment);
           m_writer.Write("%s(", matrixCtorsId[ctor].c_str());
           OutputExpressionList(declaration->assignment);
           m_writer.Write(")");
       }
       else
       {
           m_writer.Write( "%s( ", GetTypeName( declaration->type ) );
           OutputExpressionList( declaration->assignment );
           m_writer.Write( " )" );
       }
   }
}

void GLSLGenerator::OutputDeclaration(const HLSLType& type, const char* name)
{
	OutputDeclarationType( type );
	OutputDeclarationBody( type, name );
}

void GLSLGenerator::OutputDeclarationType( const HLSLType& type )
{
	m_writer.Write( "%s ", GetTypeName( type ) );
}

void GLSLGenerator::OutputDeclarationBody( const HLSLType& type, const char* name )
{
	if( !type.array )
	{
		m_writer.Write( "%s", GetSafeIdentifierName( name ) );
	}
	else
	{
		m_writer.Write( "%s[", GetSafeIdentifierName( name ) );
		if( type.arraySize != NULL )
		{
			OutputExpression( type.arraySize );
		}
		m_writer.Write( "]" );
	}
}

void GLSLGenerator::OutputCast(const HLSLType& type)
{
    if ((m_version == Version_110 || m_version == Version_120) && type.baseType == HLSLBaseType_Float3x3)
        m_writer.Write("%s", m_matrixCtorFunction);
    else
        OutputDeclaration(type, "");
}

void GLSLGenerator::Error(const char* format, ...)
{
    // It's not always convenient to stop executing when an error occurs,
    // so just track once we've hit an error and stop reporting them until
    // we successfully bail out of execution.
    if (m_error)
    {
        return;
    }
    m_error = true;

    va_list arg;
    va_start(arg, format);
    Log_ErrorArgList(format, arg);
    va_end(arg);
} 

const char* GLSLGenerator::GetSafeIdentifierName(const char* name) const
{
    for (int i = 0; i < s_numReservedWords; ++i)
    {
        if (String_Equal(s_reservedWord[i], name))
        {
            return m_reservedWord[i];
        }
    }
    return name;
}

bool GLSLGenerator::ChooseUniqueName(const char* base, char* dst, int dstLength) const
{
    for (int i = 0; i < 1024; ++i)
    {
        String_Printf(dst, dstLength, "%s%d", base, i);
        if (!m_tree->GetContainsString(dst))
        {
            return true;
        }
    }
    return false;
}

const char* GLSLGenerator::GetBuiltInSemantic(const char* semantic, AttributeModifier modifier, int* outputIndex)
{
    if (outputIndex)
        *outputIndex = -1;

    if (m_target == Target_VertexShader && modifier == AttributeModifier_Out && String_Equal(semantic, "POSITION"))
        return "gl_Position";

    if (m_target == Target_VertexShader && modifier == AttributeModifier_Out && String_Equal(semantic, "SV_Position"))
        return "gl_Position";

    if (m_target == Target_VertexShader && modifier == AttributeModifier_Out && String_Equal(semantic, "PSIZE"))
        return "gl_PointSize";

    if (m_target == Target_VertexShader && modifier == AttributeModifier_In && String_Equal(semantic, "SV_InstanceID"))
        return "gl_InstanceID";

    if (m_target == Target_FragmentShader && modifier == AttributeModifier_Out && String_Equal(semantic, "SV_Depth"))
        return "gl_FragDepth";

    if (m_target == Target_FragmentShader && modifier == AttributeModifier_In && String_Equal(semantic, "SV_Position"))
        return "gl_FragCoord";

    if (m_target == Target_FragmentShader && modifier == AttributeModifier_Out)
    {
        int index = -1;

        if (strncmp(semantic, "COLOR", 5) == 0)
            index = atoi(semantic + 5);
        else if (strncmp(semantic, "SV_Target", 9) == 0)
            index = atoi(semantic + 9);

        if (index >= 0)
        {
            if (m_outputTargets <= index)
                m_outputTargets = index + 1;

            if (outputIndex)
                *outputIndex = index;

            return m_versionLegacy ? "gl_FragData" : "rast_FragData";
        }
    }

    return NULL;
}

void GLSLGenerator::CompleteConstructorArguments(HLSLExpression* expression, HLSLBaseType dstType)
{
    int nbComponentsProvided = 0;
    int nbComponentsNeeded = 0;

    const BaseTypeDescription& dstTypeDesc = baseTypeDescriptions[dstType];
    nbComponentsNeeded = dstTypeDesc.numComponents * dstTypeDesc.height;

    const BaseTypeDescription& srcTypeDesc = baseTypeDescriptions[expression->expressionType.baseType];
    nbComponentsProvided = srcTypeDesc.numComponents * srcTypeDesc.height;
    if (IsMatrixType(expression->expressionType.baseType) ||
        IsVectorType(expression->expressionType.baseType) )
    {
        for(int i = nbComponentsProvided; i < nbComponentsNeeded; i++)
        {
            m_writer.Write(", 0");
        }
    }
}


void GLSLGenerator::OutputMatrixCtors() {
    for(matrixCtor & ctor : matrixCtors)
    {
        m_writer.Write("%s %s(",
                            GetTypeName(HLSLType(ctor.matrixType)),
                            matrixCtorsId[ctor].c_str());
        int argNum = 0;
        for(HLSLBaseType argType : ctor.argumentTypes)
        {
            if (argNum == 0)
            {
                m_writer.Write("%s %c", GetTypeName(HLSLType(argType)), 'a' + argNum);
            }
            else
            {
                m_writer.Write(", %s %c", GetTypeName(HLSLType(argType)), 'a' + argNum);
            }
            argNum++;
        }
        m_writer.Write( ") { return %s(", GetTypeName(HLSLType(ctor.matrixType)));

        const BaseTypeDescription& ctorTypeDesc = baseTypeDescriptions[ctor.matrixType];
        std::vector<std::string> args(ctorTypeDesc.numComponents * ctorTypeDesc.height, "0");
        int argNumIn = 0;
        int argNumOut = 0;
        for(HLSLBaseType argType : ctor.argumentTypes)
        {
            std::string arg;
            arg += 'a' + argNumIn;

            if (IsScalarType(argType))
            {
                int index = (argNumOut % ctorTypeDesc.height) * (ctorTypeDesc.numComponents) +
                            (argNumOut / ctorTypeDesc.height);
                args[index] = arg;
                argNumOut++;
            }
            else if (IsVectorType(argType))
            {
                const BaseTypeDescription& argTypeDesc = baseTypeDescriptions[argType];
                for(int dim = 0; dim < argTypeDesc.numComponents; dim++)
                {
                    std::string argVect = arg + ".";
                    argVect += "xyzw"[dim];
                    int index = (argNumOut % ctorTypeDesc.height) * (ctorTypeDesc.numComponents) +
                                (argNumOut / ctorTypeDesc.height);
                    args[index] = argVect;
                    argNumOut++;
                }
            }

            argNumIn++;
        }

        bool first = true;
        for(std::string & arg : args)
        {
            if (!first)
            {
                m_writer.Write(",%s", arg.c_str());
            }
            else
            {
                m_writer.Write("%s", arg.c_str());
            }
            first = false;
        }

        m_writer.Write("); }");
        m_writer.EndLine();
    }
}


}
