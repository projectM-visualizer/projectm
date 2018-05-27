//=============================================================================
//
// Render/MSLGenerator.cpp
//
// Created by Max McGuire (max@unknownworlds.com)
// Copyright (c) 2013, Unknown Worlds Entertainment, Inc.
//
//=============================================================================

//#include "Engine/String.h"
//#include "Engine/Log.h"
#include "Engine.h"

#include "MSLGenerator.h"
#include "HLSLParser.h"
#include "HLSLTree.h"

#include <string.h>

// MSL limitations:
// - Some type conversions and constructors don't work exactly the same way. For example, casts to smaller size vectors are not alloweed in C++. @@ Add more details...
// - Swizzles on scalar types, whether or not it expands them. a_float.x, a_float.xxxx both cause compile errors.
// - Using ints as floats without the trailing .0 makes the compiler sad.
// Unsupported by this generator:
// - Matrix [] access is implemented as a function call, so result cannot be passed as out/inout argument.
// - Matrix [] access is not supported in all l-value expressions. Only simple assignments.
// - No support for boolean vectors and logical operators involving vectors. This is not just in metal.
// - No support for non-float texture types

namespace M4
{
    
static void ParseSemantic(const char* semantic, unsigned int* outputLength, unsigned int* outputIndex)
{
    const char* semanticIndex = semantic;

    while (*semanticIndex && !isdigit(*semanticIndex))
    {
        semanticIndex++;
    }

    *outputLength = semanticIndex - semantic;
    *outputIndex = atoi(semanticIndex);
}

// Parse register name and advance next register index.
static int ParseRegister(const char* registerName, int& nextRegister)
{
    if (!registerName)
    {
        return nextRegister++;
    }

    while (*registerName && !isdigit(*registerName))
    {
        registerName++;
    }

    if (!*registerName)
    {
        return nextRegister++;
    }

    int result = atoi(registerName);

    if (nextRegister <= result)
    {
        nextRegister = result + 1;
    }

    return result;
}

inline bool IsHalf(HLSLBaseType type)
{
    return type >= HLSLBaseType_Half && type <= HLSLBaseType_Half4x2;
}
    
inline bool IsFloat(HLSLBaseType type)
{
    return type >= HLSLBaseType_Float && type <= HLSLBaseType_Float4x2;
}


MSLGenerator::MSLGenerator()
{
    m_tree                          = NULL;
    m_entryName                     = NULL;
    m_target                        = Target_VertexShader;
    m_error = false;

    m_firstClassArgument = NULL;
    m_lastClassArgument  = NULL;
    
    m_currentFunction = NULL;
}

// Copied from GLSLGenerator
void MSLGenerator::Error(const char* format, ...)
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

inline void MSLGenerator::AddClassArgument(ClassArgument* arg)
{
    if (m_firstClassArgument == NULL)
    {
        m_firstClassArgument = arg;
    }
    else
    {
        m_lastClassArgument->nextArg = arg;
    }
    m_lastClassArgument = arg;
}
    
    
void MSLGenerator::Prepass(HLSLTree* tree, Target target, HLSLFunction* entryFunction)
{
    // Hide unused arguments. @@ It would be good to do this in the other generators too.
    HideUnusedArguments(entryFunction);

    HLSLRoot* root = tree->GetRoot();
    HLSLStatement* statement = root->statement;
    ASSERT(m_firstClassArgument == NULL);

    HLSLType samplerType(HLSLBaseType_Sampler);

    int nextTextureRegister = 0;
    int nextBufferRegister = 0;

    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Declaration)
        {
            HLSLDeclaration* declaration = (HLSLDeclaration*)statement;
            
            if (!declaration->hidden && IsSamplerType(declaration->type))
            {
                int textureRegister = ParseRegister(declaration->registerName, nextTextureRegister);

                const char * textureName = m_tree->AddStringFormat("%s_texture", declaration->name);
                const char * textureRegisterName = m_tree->AddStringFormat("texture(%d)", textureRegister);
                AddClassArgument(new ClassArgument(textureName, declaration->type, textureRegisterName));

                if (declaration->type.baseType != HLSLBaseType_Sampler2DMS)
                {
                    const char * samplerName = m_tree->AddStringFormat("%s_sampler", declaration->name);
                    const char * samplerRegisterName = m_tree->AddStringFormat("sampler(%d)", textureRegister);
                    AddClassArgument(new ClassArgument(samplerName, samplerType, samplerRegisterName));
                }
            }
        }
        else if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer * buffer = (HLSLBuffer *)statement;

            if (!buffer->hidden)
            {
                HLSLType type(HLSLBaseType_UserDefined);
                type.addressSpace = HLSLAddressSpace_Constant;
                type.typeName = m_tree->AddStringFormat("Uniforms_%s", buffer->name);

                int bufferRegister = ParseRegister(buffer->registerName, nextBufferRegister) + m_options.bufferRegisterOffset;

                const char * bufferRegisterName = m_tree->AddStringFormat("buffer(%d)", bufferRegister);

                AddClassArgument(new ClassArgument(buffer->name, type, bufferRegisterName));
            }
        }
        
        statement = statement->nextStatement;
    }
    
    // @@ IC: instance_id parameter must be a function argument. If we find it inside a struct we must move it to the function arguments
    // and patch all the references to it!
    
    // Translate semantics.
    HLSLArgument* argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (argument->hidden)
        {
            argument = argument->nextArgument;
            continue;
        }

        if (argument->modifier == HLSLArgumentModifier_Out)
        {
            // Translate output arguments semantics.
            if (argument->type.baseType == HLSLBaseType_UserDefined)
            {
                // Our vertex input is a struct and its fields need to be tagged when we generate that
                HLSLStruct* structure = tree->FindGlobalStruct(argument->type.typeName);
                if (structure == NULL)
                {
                    Error("Vertex shader output struct '%s' not found in shader\n", argument->type.typeName);
                }

                HLSLStructField* field = structure->field;
                while (field != NULL)
                {
                    if (!field->hidden)
                    {
                        field->sv_semantic = TranslateOutputSemantic(field->semantic);
                    }
                    field = field->nextField;
                }
            }
            else
            {
                argument->sv_semantic = TranslateOutputSemantic(argument->semantic);
            }
        }
        else
        {
            // Translate input arguments semantics.
            if (argument->type.baseType == HLSLBaseType_UserDefined)
            {
                // Our vertex input is a struct and its fields need to be tagged when we generate that
                HLSLStruct* structure = tree->FindGlobalStruct(argument->type.typeName);
                if (structure == NULL)
                {
                    Error("Vertex shader input struct '%s' not found in shader\n", argument->type.typeName);
                }

                HLSLStructField* field = structure->field;
                while (field != NULL)
                {
                    // Hide vertex shader output position from fragment shader. @@ This is messing up the runtime compiler...
                    /*if (target == Target_FragmentShader && is_semantic(field->semantic, "POSITION"))
                    {
                        field->hidden = true;
                    }*/

                    if (!field->hidden)
                    {
                        field->sv_semantic = TranslateInputSemantic(field->semantic);

                        // Force type to uint.
                        if (field->sv_semantic && strcmp(field->sv_semantic, "sample_id") == 0) {
                            field->type.baseType = HLSLBaseType_Uint;
                            field->type.flags |= HLSLTypeFlag_NoPromote;
                        }
                        
                        /*if (target == Target_VertexShader && is_semantic(field->semantic, "COLOR"))
                        {
                            field->type.flags |= HLSLTypeFlag_Swizzle_BGRA;
                        }*/
                    }
                    field = field->nextField;
                }
            }
            else
            {
                argument->sv_semantic = TranslateInputSemantic(argument->semantic);
                
                // Force type to uint.
                if (argument->sv_semantic && strcmp(argument->sv_semantic, "sample_id") == 0) {
                    argument->type.baseType = HLSLBaseType_Uint;
                    argument->type.flags |= HLSLTypeFlag_NoPromote;
                }
            }
        }

        argument = argument->nextArgument;
    }

    // Translate return value semantic.
    if (entryFunction->returnType.baseType != HLSLBaseType_Void)
    {
        if (entryFunction->returnType.baseType == HLSLBaseType_UserDefined)
        {
            // Our vertex input is a struct and its fields need to be tagged when we generate that
            HLSLStruct* structure = tree->FindGlobalStruct(entryFunction->returnType.typeName);
            if (structure == NULL)
            {
                Error("Vertex shader output struct '%s' not found in shader\n", entryFunction->returnType.typeName);
            }

            HLSLStructField* field = structure->field;
            while (field != NULL)
            {
                if (!field->hidden)
                {
                    field->sv_semantic = TranslateOutputSemantic(field->semantic);
                }
                field = field->nextField;
            }
        }
        else
        {
            entryFunction->sv_semantic = TranslateOutputSemantic(entryFunction->semantic);
            
            //Error("MSL only supports COLOR semantic in return \n", entryFunction->returnType.typeName);
        }
    }
}

void MSLGenerator::CleanPrepass()
{
    ClassArgument* currentArg = m_firstClassArgument;
    while (currentArg != NULL)
    {
        ClassArgument* nextArg = currentArg->nextArg;
        delete currentArg;
        currentArg = nextArg;
    }
    delete currentArg;
    m_firstClassArgument = NULL;
    m_lastClassArgument = NULL;
}
    
void MSLGenerator::PrependDeclarations()
{
    // Any special function stubs we need go here
    // That includes special constructors to emulate HLSL not being strict
    
    
    m_writer.WriteLine(0, "#include <metal_stdlib>");
    m_writer.WriteLine(0, "using namespace metal;");
    m_writer.WriteLine(0, "");
    
    if (m_tree->NeedsFunction("mad"))
    {
        if (m_options.usePreciseFma)
        {
            m_writer.WriteLine(0, "#define mad precise::fma");
        }
        else
        {
            m_writer.WriteLine(0, "inline float mad(float a, float b, float c) {");
            m_writer.WriteLine(1, "return a * b + c;");
            m_writer.WriteLine(0, "}");
            m_writer.WriteLine(0, "inline float2 mad(float2 a, float2 b, float2 c) {");
            m_writer.WriteLine(1, "return a * b + c;");
            m_writer.WriteLine(0, "}");
            m_writer.WriteLine(0, "inline float3 mad(float3 a, float3 b, float3 c) {");
            m_writer.WriteLine(1, "return a * b + c;");
            m_writer.WriteLine(0, "}");
            m_writer.WriteLine(0, "inline float4 mad(float4 a, float4 b, float4 c) {");
            m_writer.WriteLine(1, "return a * b + c;");
            m_writer.WriteLine(0, "}");
            
            if (!m_options.treatHalfAsFloat)
            {
                m_writer.WriteLine(0, "inline half mad(half a, half b, half c) {");
                m_writer.WriteLine(1, "return a * b + c;");
                m_writer.WriteLine(0, "}");
                m_writer.WriteLine(0, "inline half2 mad(half2 a, half2 b, half2 c) {");
                m_writer.WriteLine(1, "return a * b + c;");
                m_writer.WriteLine(0, "}");
                m_writer.WriteLine(0, "inline half3 mad(half3 a, half3 b, half3 c) {");
                m_writer.WriteLine(1, "return a * b + c;");
                m_writer.WriteLine(0, "}");
                m_writer.WriteLine(0, "inline half4 mad(half4 a, half4 b, half4 c) {");
                m_writer.WriteLine(1, "return a * b + c;");
                m_writer.WriteLine(0, "}");
            }
        }
    }

    // @@ These should not be needed anymore.
    /*if (m_tree->NeedsFunction("max"))
    {
        m_writer.WriteLine(0, "inline float max(int a, float b) {");
        m_writer.WriteLine(1, "return max((float)a, b);");
        m_writer.WriteLine(0, "}");
        m_writer.WriteLine(0, "inline float max(float a, int b) {");
        m_writer.WriteLine(1, "return max(a, (float)b);");
        m_writer.WriteLine(0, "}");
    }
    if (m_tree->NeedsFunction("min"))
    {
        m_writer.WriteLine(0, "inline float min(int a, float b) {");
        m_writer.WriteLine(1, "return min((float)a, b);");
        m_writer.WriteLine(0, "}");
        m_writer.WriteLine(0, "inline float min(float a, int b) {");
        m_writer.WriteLine(1, "return min(a, (float)b);");
        m_writer.WriteLine(0, "}");
    }*/

    if (m_tree->NeedsFunction("lerp"))
    {
        //m_writer.WriteLine(0, "template<typename T, typename X> inline T mix(T a, T b, X x) {");
        //m_writer.WriteLine(1, "return mix(a, b, (float)x);");
        //m_writer.WriteLine(0, "}");
        m_writer.WriteLine(0, "#define lerp mix");
    }

    if (m_tree->NeedsFunction("mul"))
    {
        const char* am = (m_options.flags & Flag_PackMatrixRowMajor) ? "m * a" : "a * m";
        const char* ma = (m_options.flags & Flag_PackMatrixRowMajor) ? "a * m" : "m * a";

        m_writer.WriteLine(0, "inline float2 mul(float2 a, float2x2 m) { return %s; }", am);
        m_writer.WriteLine(0, "inline float3 mul(float3 a, float3x3 m) { return %s; }", am);
        m_writer.WriteLine(0, "inline float4 mul(float4 a, float4x4 m) { return %s; }", am);

        m_writer.WriteLine(0, "inline float2 mul(float2x2 m, float2 a) { return %s; }", ma);
        m_writer.WriteLine(0, "inline float3 mul(float3x3 m, float3 a) { return %s; }", ma);
        m_writer.WriteLine(0, "inline float4 mul(float4x4 m, float4 a) { return %s; }", ma);

        // TODO: Support PackMatrixRowMajor for float3x4/float4x3
        m_writer.WriteLine(0, "inline float3 mul(float4 a, float3x4 m) { return a * m; }");
        m_writer.WriteLine(0, "inline float2 mul(float4 a, float2x4 m) { return a * m; }");
        
        if (!m_options.treatHalfAsFloat)
        {
            m_writer.WriteLine(0, "inline half2 mul(half2 a, half2x2 m) { return %s; }", am);
            m_writer.WriteLine(0, "inline half3 mul(half3 a, half3x3 m) { return %s; }", am);
            m_writer.WriteLine(0, "inline half4 mul(half4 a, half4x4 m) { return %s; }", am);
        
            m_writer.WriteLine(0, "inline half2 mul(half2x2 m, half2 a) { return %s; }", ma);
            m_writer.WriteLine(0, "inline half3 mul(half3x3 m, half3 a) { return %s; }", ma);
            m_writer.WriteLine(0, "inline half4 mul(half4x4 m, half4 a) { return %s; }", ma);
        
            // TODO: Support PackMatrixRowMajor for half3x4/half4x3
            m_writer.WriteLine(0, "inline half3 mul(half4 a, half3x4 m) { return a * m; }");
            m_writer.WriteLine(0, "inline half2 mul(half4 a, half2x4 m) { return a * m; }");
        }

    }

    // @@ How do we know if these will be needed? We could write these after parsing the whole file and prepend them.
    m_writer.WriteLine(0, "inline float4 column(float4x4 m, int i) {");
    m_writer.WriteLine(1, "return float4(m[0][i], m[1][i], m[2][i], m[3][i]);");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float3 column(float3x4 m, int i) {");
    m_writer.WriteLine(1, "return float3(m[0][i], m[1][i], m[2][i]);");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float2 column(float2x4 m, int i) {");
    m_writer.WriteLine(1, "return float2(m[0][i], m[1][i]);");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float4 set_column(thread float4x4& m, int i, float4 v) {");
    m_writer.WriteLine(1, "    m[0][i] = v.x; m[1][i] = v.y; m[2][i] = v.z; m[3][i] = v.w; return v;");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float3 set_column(thread float3x4& m, int i, float3 v) {");
    m_writer.WriteLine(1, "    m[0][i] = v.x; m[1][i] = v.y; m[2][i] = v.z; return v;");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float2 set_column(thread float2x4& m, int i, float2 v) {");
    m_writer.WriteLine(1, "    m[0][i] = v.x; m[1][i] = v.y; return v;");
    m_writer.WriteLine(0, "}");

    m_writer.WriteLine(0, "inline float3x3 matrix_ctor(float4x4 m) {");
    m_writer.WriteLine(1, "    return float3x3(m[0].xyz, m[1].xyz, m[2].xyz);");
    m_writer.WriteLine(0, "}");

    
    if (m_tree->NeedsFunction("clip"))
    {
        m_writer.WriteLine(0, "inline void clip(float x) {");
        m_writer.WriteLine(1, "if (x < 0.0) discard_fragment();");
        m_writer.WriteLine(0, "}");
    }
    if (m_tree->NeedsFunction("rcp"))
    {
        m_writer.WriteLine(0, "inline float rcp(float x) {");
        m_writer.WriteLine(1, "return 1.0f / x;");
        m_writer.WriteLine(0, "}");
        
        m_writer.WriteLine(0, "inline half rcp(half x) {");
        m_writer.WriteLine(1, "return 1.0h / x;");
        m_writer.WriteLine(0, "}");
    }

    if (m_tree->NeedsFunction("ddx")) m_writer.WriteLine(0, "#define ddx dfdx");
    if (m_tree->NeedsFunction("ddy")) m_writer.WriteLine(0, "#define ddy dfdy");
    if (m_tree->NeedsFunction("frac")) m_writer.WriteLine(0, "#define frac fract");
    
    //m_writer.WriteLine(0, "#define mad fma");     // @@ This doesn't seem to work.
    
    const char * samplerType = "float";
    /*if (m_options.halfTextureSamplers)
    {
        samplerType = "half";
    }*/
    const char * intType = "int";
    const char * uintType = "uint";
    if (m_options.use16BitIntegers) {
        intType = "short";
        uintType = "ushort";
    }
    
    if (m_tree->NeedsFunction("tex2D") ||
        m_tree->NeedsFunction("tex2Dlod") ||
        m_tree->NeedsFunction("tex2Dgrad") ||
        m_tree->NeedsFunction("tex2Dbias") ||
        m_tree->NeedsFunction("tex2Dfetch"))
    {
        m_writer.WriteLine(0, "struct Texture2DSampler {");
        m_writer.WriteLine(1, "Texture2DSampler(thread const texture2d<float>& t, thread const sampler& s) : t(t), s(s) {};");
        m_writer.WriteLine(1, "const thread texture2d<float>& t;");
        m_writer.WriteLine(1, "const thread sampler& s;");
        m_writer.WriteLine(0, "};");

        if (!m_options.treatHalfAsFloat) {
            m_writer.WriteLine(0, "struct Texture2DHalfSampler {");
            m_writer.WriteLine(1, "Texture2DHalfSampler(thread const texture2d<half>& t, thread const sampler& s) : t(t), s(s) {};");
            m_writer.WriteLine(1, "const thread texture2d<half>& t;");
            m_writer.WriteLine(1, "const thread sampler& s;");
            m_writer.WriteLine(0, "};");
        }
    }
    
    if (m_tree->NeedsFunction("tex2D"))
    {
        m_writer.WriteLine(0, "inline float4 tex2D(Texture2DSampler ts, float2 texCoord) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord);");
        m_writer.WriteLine(0, "}");
        
        if (!m_options.treatHalfAsFloat)
        {
            m_writer.WriteLine(0, "inline half4 tex2D(Texture2DHalfSampler ts, float2 texCoord) {");
            m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord);");
            m_writer.WriteLine(0, "}");
        }
    }
    if (m_tree->NeedsFunction("tex2Dlod"))
    {
        m_writer.WriteLine(0, "inline float4 tex2Dlod(Texture2DSampler ts, float4 texCoordMip) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordMip.xy, level(texCoordMip.w));");
        m_writer.WriteLine(0, "}");
        
        if (!m_options.treatHalfAsFloat)
        {
            m_writer.WriteLine(0, "inline half4 tex2Dlod(Texture2DHalfSampler ts, float4 texCoordMip) {");
            m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordMip.xy, level(texCoordMip.w));");
            m_writer.WriteLine(0, "}");
        }
    }
    if (m_tree->NeedsFunction("tex2Dgrad"))
    {
        m_writer.WriteLine(0, "inline float4 tex2Dgrad(Texture2DSampler ts, float2 texCoord, float2 gradx, float2 grady) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord.xy, gradient2d(gradx, grady));");
        m_writer.WriteLine(0, "}");
    }
    if (m_tree->NeedsFunction("tex2Dbias"))
    {
        m_writer.WriteLine(0, "inline float4 tex2Dbias(Texture2DSampler ts, float4 texCoordBias) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordBias.xy, bias(texCoordBias.w));");
        m_writer.WriteLine(0, "}");
        
        if (!m_options.treatHalfAsFloat)
        {
            m_writer.WriteLine(0, "inline half4 tex2Dbias(Texture2DHalfSampler ts, float4 texCoordBias) {");
            m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordBias.xy, bias(texCoordBias.w));");
            m_writer.WriteLine(0, "}");
        }
    }
    if (m_tree->NeedsFunction("tex2Dfetch"))
    {
        // @@ not used? not tested?
        m_writer.WriteLine(0, "inline float4 tex2Dfetch(Texture2DSampler ts, %s2 texCoord) {", intType);
        m_writer.WriteLine(1, "return ts.t.read((uint2)texCoord);");
        m_writer.WriteLine(0, "}");
    }
    
    if (m_tree->NeedsFunction("tex3D") ||
        m_tree->NeedsFunction("tex3Dlod"))
    {
        m_writer.WriteLine(0, "struct Texture3DSampler {");
        m_writer.WriteLine(1, "Texture3DSampler(thread const texture3d<float>& t, thread const sampler& s) : t(t), s(s) {};");
        m_writer.WriteLine(1, "const thread texture3d<float>& t;");
        m_writer.WriteLine(1, "const thread sampler& s;");
        m_writer.WriteLine(0, "};");
    }

    if (m_tree->NeedsFunction("tex3D"))
    {
        m_writer.WriteLine(0, "inline float4 tex3D(Texture3DSampler ts, float3 texCoord) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord);");
        m_writer.WriteLine(0, "}");
    }
    if (m_tree->NeedsFunction("tex3Dlod"))
    {
        m_writer.WriteLine(0, "inline float4 tex3Dlod(Texture3DSampler ts, float4 texCoordMip) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordMip.xyz, level(texCoordMip.w));");
        m_writer.WriteLine(0, "}");
    }
    
    if (m_tree->NeedsFunction("texCUBE") ||
        m_tree->NeedsFunction("texCUBElod") ||
        m_tree->NeedsFunction("texCUBEbias"))
    {
        m_writer.WriteLine(0, "struct TextureCubeSampler {");
        m_writer.WriteLine(1, "TextureCubeSampler(thread const texturecube<float>& t, thread const sampler& s) : t(t), s(s) {};");
        m_writer.WriteLine(1, "const thread texturecube<float>& t;");
        m_writer.WriteLine(1, "const thread sampler& s;");
        m_writer.WriteLine(0, "};");
    }

    if (m_tree->NeedsFunction("texCUBE"))
    {
        m_writer.WriteLine(0, "inline float4 texCUBE(TextureCubeSampler ts, float3 texCoord) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord);");
        m_writer.WriteLine(0, "}");
    }

    if (m_tree->NeedsFunction("texCUBElod"))
    {
        m_writer.WriteLine(0, "inline float4 texCUBElod(TextureCubeSampler ts, float4 texCoordMip) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordMip.xyz, level(texCoordMip.w));");
        m_writer.WriteLine(0, "}");
    }

    if (m_tree->NeedsFunction("texCUBEbias"))
    {
        m_writer.WriteLine(0, "inline float4 texCUBEbias(TextureCubeSampler ts, float4 texCoordBias) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoordBias.xyz, bias(texCoordBias.w));");
        m_writer.WriteLine(0, "}");
    }
    
    if (m_tree->NeedsFunction("tex2Dcmp"))
    {
        m_writer.WriteLine(0, "struct Texture2DShadowSampler {");
        m_writer.WriteLine(1, "Texture2DShadowSampler(thread const depth2d<float>& t, thread const sampler& s) : t(t), s(s) {};");
        m_writer.WriteLine(1, "const thread depth2d<float>& t;");
        m_writer.WriteLine(1, "const thread sampler& s;");
        m_writer.WriteLine(0, "};");
        
        m_writer.WriteLine(0, "inline float4 tex2Dcmp(Texture2DShadowSampler ts, float4 texCoordCompare) {");
        if (m_options.flags & Flag_ConstShadowSampler)
        {
            // iOS Metal requires that the sampler in sample_compare is a compile-time constant
            m_writer.WriteLine(1, "constexpr sampler shadow_constant_sampler(mip_filter::none, min_filter::linear, mag_filter::linear, address::clamp_to_edge, compare_func::less);");
            m_writer.WriteLine(1, "return ts.t.sample_compare(shadow_constant_sampler, texCoordCompare.xy, texCoordCompare.z);");
        }
        else
        {
            m_writer.WriteLine(1, "return ts.t.sample_compare(ts.s, texCoordCompare.xy, texCoordCompare.z);");
        }
        m_writer.WriteLine(0, "}");
    }
    
    if (m_tree->NeedsFunction("tex2DMSfetch"))
    {
        m_writer.WriteLine(0, "inline float4 tex2DMSfetch(texture2d_ms<float> t, %s2 texCoord, %s sample) {", intType, intType);
        m_writer.WriteLine(1, "return t.read((uint2)texCoord, (uint)sample);");
        m_writer.WriteLine(0, "}");
    }

    if (m_tree->NeedsFunction("tex2DArray"))
    {
        m_writer.WriteLine(0, "struct Texture2DArraySampler {");
        m_writer.WriteLine(1, "const thread texture2d_array<float>& t;");
        m_writer.WriteLine(1, "const thread sampler& s;");
        m_writer.WriteLine(1, "Texture2DArraySampler(thread const texture2d_array<float>& t, thread const sampler& s) : t(t), s(s) {};");
        m_writer.WriteLine(0, "};");

        m_writer.WriteLine(0, "inline float4 tex2DArray(Texture2DArraySampler ts, float3 texCoord) {");
        m_writer.WriteLine(1, "return ts.t.sample(ts.s, texCoord.xy, texCoord.z + 0.5);"); // 0.5 offset needed on nvidia gpus
        m_writer.WriteLine(0, "}");
    }
}
    
bool MSLGenerator::Generate(HLSLTree* tree, Target target, const char* entryName, const Options& options)
{
    m_firstClassArgument = NULL;
    m_lastClassArgument = NULL;

    m_tree      = tree;
    m_target    = target;
    ASSERT(m_target == Target_VertexShader || m_target == Target_FragmentShader);
    m_entryName = entryName;
    m_options = options;

    m_writer.Reset();

    // Find entry point function
    HLSLFunction* entryFunction = tree->FindFunction(entryName);
    if (entryFunction == NULL)
    {
        Error("Entry point '%s' doesn't exist\n", entryName);
        return false;
    }
    
    Prepass(tree, target, entryFunction);
    
    PrependDeclarations();
    
    // In MSL, uniforms are parameters for the entry point, not globals:
    // to limit code rewriting, we wrap the entire original shader into a class.
    // Uniforms are then passed to the constructor and copied to member variables.
    const char* shaderClassName = (target == MSLGenerator::Target_VertexShader) ? "Vertex_Shader" : "Pixel_Shader";
    m_writer.WriteLine(0, "struct %s {", shaderClassName);
    
    HLSLRoot* root = m_tree->GetRoot();
    OutputStatements(1, root->statement);

    // Generate constructor
    m_writer.WriteLine(0, "");
    m_writer.BeginLine(1);

    m_writer.Write("%s(", shaderClassName);
    const ClassArgument* currentArg = m_firstClassArgument;
    while (currentArg != NULL)
    {
        if (currentArg->type.addressSpace == HLSLAddressSpace_Constant) m_writer.Write("constant ");
        else m_writer.Write("thread ");

        m_writer.Write("%s & %s", GetTypeName(currentArg->type), currentArg->name);

        currentArg = currentArg->nextArg;
        if (currentArg) m_writer.Write(", ");
    }
    m_writer.Write(")");

    currentArg = m_firstClassArgument;
    if (currentArg) m_writer.Write(" : ");
    while (currentArg != NULL)
    {
        m_writer.Write("%s(%s)", currentArg->name, currentArg->name);
        currentArg = currentArg->nextArg;
        if (currentArg) m_writer.Write(", ");
    }
    m_writer.EndLine(" {}");

    m_writer.WriteLine(0, "};"); // Class
    

    // Generate real entry point, the one called by Metal
    m_writer.WriteLine(0, "");

    // If function return value has a non-color output semantic, declare a temporary struct for the output.
    bool wrapReturnType = false;
    if (entryFunction->sv_semantic != NULL && strcmp(entryFunction->sv_semantic, "color(0)") != 0)
    {
        wrapReturnType = true;
        
        m_writer.WriteLine(0, "struct %s_output { %s tmp [[%s]]; };", entryName, GetTypeName(entryFunction->returnType), entryFunction->sv_semantic);
        
        m_writer.WriteLine(0, "");
    }
    
    
    m_writer.BeginLine(0);

    // @@ Add/Translate function attributes.
    // entryFunction->attributes
    
    if (m_target == Target_VertexShader)
    {
        m_writer.Write("vertex ");
    }
    else
    {
        m_writer.Write("fragment ");
    }

    // Return type.
    if (wrapReturnType)
    {
        m_writer.Write("%s_output", entryName);
    }
    else
    {
        if (entryFunction->returnType.baseType == HLSLBaseType_UserDefined)
        {
            m_writer.Write("%s::", shaderClassName);
        }
        m_writer.Write("%s", GetTypeName(entryFunction->returnType));
    }

    m_writer.Write(" %s(", entryName);

    int argumentCount = 0;
    HLSLArgument* argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (!argument->hidden)
        {
            if (argument->type.baseType == HLSLBaseType_UserDefined)
            {
                m_writer.Write("%s::", shaderClassName);
            }
            m_writer.Write("%s %s", GetTypeName(argument->type), argument->name);

            // @@ IC: We are assuming that the first argument is the 'stage_in'.
            if (argument->type.baseType == HLSLBaseType_UserDefined && argument == entryFunction->argument)
            {
                m_writer.Write(" [[stage_in]]");
            }
            else if (argument->sv_semantic)
            {
                m_writer.Write(" [[%s]]", argument->sv_semantic);
            }
            argumentCount++;
        }
        argument = argument->nextArgument;
        if (argument && !argument->hidden)
        {
            m_writer.Write(", ");
        }
    }

    currentArg = m_firstClassArgument;
    if (argumentCount && currentArg != NULL)
    {
        m_writer.Write(", ");
    }
    while (currentArg != NULL)
    {
        //if (currentArg->type.addressSpace == HLSLAddressSpace_Constant) m_writer.Write("constant ");
        //else m_writer.Write("thread ");

        if (currentArg->type.baseType == HLSLBaseType_UserDefined)
        {
            m_writer.Write("constant %s::%s & %s [[%s]]", shaderClassName, currentArg->type.typeName, currentArg->name, currentArg->registerName);
        }
        else
        {
            m_writer.Write("%s %s [[%s]]", GetTypeName(currentArg->type), currentArg->name, currentArg->registerName);
        }

        currentArg = currentArg->nextArg;
        if (currentArg)
        {
            m_writer.Write(", ");
        }
    }
    m_writer.EndLine(") {");

    // Create the helper class instance and call the entry point from the original shader
    m_writer.BeginLine(1);
    m_writer.Write("%s %s", shaderClassName, entryName);

    currentArg = m_firstClassArgument;
    if (currentArg)
    {
        m_writer.Write("(");

        while (currentArg != NULL)
        {
            m_writer.Write("%s", currentArg->name);
            currentArg = currentArg->nextArg;
            if (currentArg)
            {
                m_writer.Write(", ");
            }
        }

        m_writer.Write(")");
    }
    m_writer.EndLine(";");

    m_writer.BeginLine(1);
    
    if (wrapReturnType)
    {
        m_writer.Write("%s_output output; output.tmp = %s.%s(", entryName, entryName, entryName);
    }
    else
    {
        m_writer.Write("return %s.%s(", entryName, entryName);
    }

    argument = entryFunction->argument;
    while (argument != NULL)
    {
        if (!argument->hidden)
        {
            m_writer.Write("%s", argument->name);
        }
        argument = argument->nextArgument;
        if (argument && !argument->hidden)
        {
            m_writer.Write(", ");
        }
    }

    m_writer.EndLine(");");
    
    if (wrapReturnType)
    {
        m_writer.WriteLine(1, "return output;");
    }

    m_writer.WriteLine(0, "}");

    CleanPrepass();
    m_tree = NULL;
    
    // Any final check goes here, but shouldn't be needed as the Metal compiler is solid
    
    return !m_error;
}

const char* MSLGenerator::GetResult() const
{
    return m_writer.GetResult();
}

void MSLGenerator::OutputStatements(int indent, HLSLStatement* statement)
{
    // Main generator loop: called recursively
    while (statement != NULL)
    {
        if (statement->hidden) 
        {
            statement = statement->nextStatement;
            continue;
        }

        OutputAttributes(indent, statement->attributes);

        if (statement->nodeType == HLSLNodeType_Declaration)
        {
            HLSLDeclaration* declaration = static_cast<HLSLDeclaration*>(statement);
            
            if (declaration->assignment && declaration->assignment->nodeType == HLSLNodeType_FunctionCall)
            {
                OutputFunctionCallStatement(indent, (HLSLFunctionCall*)declaration->assignment, declaration);
            }
            else
            {
                m_writer.BeginLine(indent, declaration->fileName, declaration->line);
                OutputDeclaration(declaration);
                m_writer.EndLine(";");
            }
        }
        else if (statement->nodeType == HLSLNodeType_Struct)
        {
            HLSLStruct* structure = static_cast<HLSLStruct*>(statement);
            OutputStruct(indent, structure);
        }
        else if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer* buffer = static_cast<HLSLBuffer*>(statement);
            OutputBuffer(indent, buffer);
        }
        else if (statement->nodeType == HLSLNodeType_Function)
        {
            HLSLFunction* function = static_cast<HLSLFunction*>(statement);

            if (!function->forward)
            {
                OutputFunction(indent, function);
            }
        }
        else if (statement->nodeType == HLSLNodeType_ExpressionStatement)
        {
            HLSLExpressionStatement* expressionStatement = static_cast<HLSLExpressionStatement*>(statement);
            HLSLExpression* expression = expressionStatement->expression;
            
            if (expression->nodeType == HLSLNodeType_FunctionCall)
            {
                OutputFunctionCallStatement(indent, (HLSLFunctionCall*)expression, NULL);
            }
            else
            {
                m_writer.BeginLine(indent, statement->fileName, statement->line);
                OutputExpression(expressionStatement->expression, NULL);
                m_writer.EndLine(";");
            }
        }
        else if (statement->nodeType == HLSLNodeType_ReturnStatement)
        {
            HLSLReturnStatement* returnStatement = static_cast<HLSLReturnStatement*>(statement);
            if (m_currentFunction->numOutputArguments > 0)
            {
                m_writer.BeginLine(indent, returnStatement->fileName, returnStatement->line);
                m_writer.Write("return { ");

                int numArguments = 0;
                if (returnStatement->expression != NULL)
                {
                    OutputTypedExpression(m_currentFunction->returnType, returnStatement->expression, NULL);
                    numArguments++;
                }
                
                HLSLArgument * argument = m_currentFunction->argument;
                while (argument != NULL)
                {
                    if (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout)
                    {
                        if (numArguments) m_writer.Write(", ");
                        m_writer.Write("%s", argument->name);
                        numArguments++;
                    }
                    argument = argument->nextArgument;
                }
                
                m_writer.EndLine(" };");
            }
            else if (returnStatement->expression != NULL)
            {
                m_writer.BeginLine(indent, returnStatement->fileName, returnStatement->line);
                m_writer.Write("return ");
                OutputTypedExpression(m_currentFunction->returnType, returnStatement->expression, NULL);
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
            m_writer.WriteLineTagged(indent, discardStatement->fileName, discardStatement->line, "discard_fragment();");
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
            
            if (ifStatement->isStatic) {
                int value;
                if (!m_tree->GetExpressionValue(ifStatement->condition, value)) {
                    Error("@if condition could not be evaluated.\n");
                }
                if (value != 0) {
                    OutputStatements(indent + 1, ifStatement->statement);
                }
                else if (ifStatement->elseStatement != NULL) {
                    OutputStatements(indent + 1, ifStatement->elseStatement);
                }
            }
            else {
                m_writer.BeginLine(indent, ifStatement->fileName, ifStatement->line);
                m_writer.Write("if (");
                OutputExpression(ifStatement->condition, NULL);
                m_writer.Write(") {");
                m_writer.EndLine();
                OutputStatements(indent + 1, ifStatement->statement);
                m_writer.WriteLine(indent, "}");
                if (ifStatement->elseStatement != NULL)
                {
                    m_writer.WriteLine(indent, "else {");
                    OutputStatements(indent + 1, ifStatement->elseStatement);
                    m_writer.WriteLine(indent, "}");
                }
            }
        }
        else if (statement->nodeType == HLSLNodeType_ForStatement)
        {
            HLSLForStatement* forStatement = static_cast<HLSLForStatement*>(statement);
            m_writer.BeginLine(indent, forStatement->fileName, forStatement->line);
            m_writer.Write("for (");
            OutputDeclaration(forStatement->initialization);
            m_writer.Write("; ");
            OutputExpression(forStatement->condition, NULL);
            m_writer.Write("; ");
            OutputExpression(forStatement->increment, NULL);
            m_writer.Write(") {");
            m_writer.EndLine();
            OutputStatements(indent + 1, forStatement->statement);
            m_writer.WriteLine(indent, "}");
        }
        else if (statement->nodeType == HLSLNodeType_BlockStatement)
        {
            HLSLBlockStatement* blockStatement = static_cast<HLSLBlockStatement*>(statement);
            m_writer.WriteLineTagged(indent, blockStatement->fileName, blockStatement->line, "{");
            OutputStatements(indent + 1, blockStatement->statement);
            m_writer.WriteLine(indent, "}");
        }
        else if (statement->nodeType == HLSLNodeType_Technique)
        {
            // Techniques are ignored.
        }
        else if (statement->nodeType == HLSLNodeType_Pipeline)
        {
            // Pipelines are ignored.
        }
        else
        {
            // Unhandled statement type.
            ASSERT(0);
        }

        statement = statement->nextStatement;
    }
}

// Called by OutputStatements

void MSLGenerator::OutputAttributes(int indent, HLSLAttribute* attribute)
{
    // IC: These do not appear to exist in MSL.
    while (attribute != NULL) {
        if (attribute->attributeType == HLSLAttributeType_Unroll)
        {
            // @@ Do any of these work?
            //m_writer.WriteLine(indent, attribute->fileName, attribute->line, "#pragma unroll");
            //m_writer.WriteLine(indent, attribute->fileName, attribute->line, "[[unroll]]");
        }
        else if (attribute->attributeType == HLSLAttributeType_Flatten)
        {
            // @@
        }
        else if (attribute->attributeType == HLSLAttributeType_Branch)
        {
            // @@
        }
        
        attribute = attribute->nextAttribute;
    }
}

void MSLGenerator::OutputDeclaration(HLSLDeclaration* declaration)
{
    if (IsSamplerType(declaration->type))
    {
        // Declare a texture and a sampler instead
        // We do not handle multiple textures on the same line
        ASSERT(declaration->nextDeclaration == NULL);
        const char * typeName = "float";
        if (declaration->type.samplerType == HLSLBaseType_Half && !m_options.treatHalfAsFloat)
        {
            typeName = "half";
        }
        
        if (declaration->type.baseType == HLSLBaseType_Sampler2D)
        {
            m_writer.Write("thread texture2d<%s>& %s_texture; thread sampler& %s_sampler", typeName, declaration->name, declaration->name);
        }
        else if (declaration->type.baseType == HLSLBaseType_Sampler3D)
        {
            m_writer.Write("thread texture3d<%s>& %s_texture; thread sampler& %s_sampler", typeName, declaration->name, declaration->name);
        }
        else if (declaration->type.baseType == HLSLBaseType_SamplerCube)
        {
            m_writer.Write("thread texturecube<%s>& %s_texture; thread sampler& %s_sampler", typeName, declaration->name, declaration->name);
        }
        else if (declaration->type.baseType == HLSLBaseType_Sampler2DShadow)
        {
            m_writer.Write("thread depth2d<float>& %s_texture; thread sampler& %s_sampler", declaration->name, declaration->name);
        }
        else if (declaration->type.baseType == HLSLBaseType_Sampler2DMS)
        {
            m_writer.Write("thread texture2d_ms<%s>& %s_texture", typeName, declaration->name);
        }
        else if (declaration->type.baseType == HLSLBaseType_Sampler2DArray)
        {
            m_writer.Write("thread texture2d_array<%s>& %s_texture; thread sampler& %s_sampler", typeName, declaration->name, declaration->name);
        }
        else
        {
            m_writer.Write("<unhandled texture type>");
        }
    }
    else
    {
        OutputDeclaration(declaration->type, declaration->name, declaration->assignment);

        declaration = declaration->nextDeclaration;
        while(declaration != NULL)
        {
            m_writer.Write(",");
            OutputDeclarationBody(declaration->type, declaration->name, declaration->assignment);
            declaration = declaration->nextDeclaration;
        };
    }
}

void MSLGenerator::OutputStruct(int indent, HLSLStruct* structure)
{
    m_writer.WriteLineTagged(indent, structure->fileName, structure->line, "struct %s {", structure->name);
    HLSLStructField* field = structure->field;
    while (field != NULL)
    {
        if (!field->hidden)
        {
            m_writer.BeginLine(indent + 1, field->fileName, field->line);
            OutputDeclaration(field->type, field->name, NULL);
            if (field->sv_semantic)
            {
                m_writer.Write(" [[%s]]", field->sv_semantic);
            }

            m_writer.Write(";");
            m_writer.EndLine();
        }
        field = field->nextField;
    }
    m_writer.WriteLine(indent, "};");
}

void MSLGenerator::OutputBuffer(int indent, HLSLBuffer* buffer)
{
    HLSLDeclaration* field = buffer->field;
    
    m_writer.BeginLine(indent, buffer->fileName, buffer->line);
    m_writer.Write("struct Uniforms_%s", buffer->name);
    m_writer.EndLine(" {");
    while (field != NULL)
    {
        if (!field->hidden)
        {
            m_writer.BeginLine(indent + 1, field->fileName, field->line);
            OutputDeclaration(field->type, field->name, field->assignment, false, false, /*alignment=*/16);
            m_writer.EndLine(";");
        }
        field = (HLSLDeclaration*)field->nextStatement;
    }
    m_writer.WriteLine(indent, "};");

    m_writer.WriteLine(indent, "constant Uniforms_%s & %s;", buffer->name, buffer->name);
}
    
void MSLGenerator::OutputFunction(int indent, HLSLFunction* function)
{
    const char* functionName   = function->name;
    const char* returnTypeName = GetTypeName(function->returnType);
    
    // Declare output tuple.
    if (function->numOutputArguments > 0)
    {
        returnTypeName = m_tree->AddStringFormat("%s_out%d", functionName, function->line); // @@ Find a better way to generate unique name.
    
        m_writer.BeginLine(indent, function->fileName, function->line);
        m_writer.Write("struct %s { ", returnTypeName);
        m_writer.EndLine();
        
        if (function->returnType.baseType != HLSLBaseType_Void)
        {
            m_writer.BeginLine(indent+1, function->fileName, function->line);
            OutputDeclaration(function->returnType, "__result", /*defaultValue=*/NULL, /*isRef=*/false, /*isConst=*/false);
            m_writer.EndLine(";");
        }
        
        HLSLArgument * argument = function->argument;
        while (argument != NULL)
        {
            if (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout)
            {
                m_writer.BeginLine(indent+1, function->fileName, function->line);
                OutputDeclaration(argument->type, argument->name, /*defaultValue=*/NULL, /*isRef=*/false, /*isConst=*/false);
                m_writer.EndLine(";");
            }
            argument = argument->nextArgument;
        }
        
        m_writer.WriteLine(indent, "};");
        
        // Create unique function name to avoid collision with overloads and different return types.
        m_writer.BeginLine(indent, function->fileName, function->line);
        m_writer.Write("%s %s_%d(", returnTypeName, functionName, function->line);
    }
    else
    {
        m_writer.BeginLine(indent, function->fileName, function->line);
        m_writer.Write("%s %s(", returnTypeName, functionName);
    }
    
    OutputArguments(function->argument);
    
    m_writer.EndLine(") {");
    m_currentFunction = function;
    
    // Local declarations for output arguments.
    HLSLArgument * argument = function->argument;
    while (argument != NULL)
    {
        if (argument->modifier == HLSLArgumentModifier_Out)
        {
            m_writer.BeginLine(indent + 1, function->fileName, function->line);
            OutputDeclaration(argument->type, argument->name, /*defaultValue=*/NULL, /*isRef=*/false, /*isConst=*/false);
            m_writer.EndLine(";");
        }
        argument = argument->nextArgument;
    }

    OutputStatements(indent + 1, function->statement); // @@ Modify return statements if function has multiple output arguments!
    
    // Output implicit return.
    if (function->numOutputArguments > 0)
    {
        bool needsImplicitReturn = true;
        HLSLStatement * statement = function->statement;
        if (statement != NULL)
        {
            while (statement->nextStatement != NULL)
            {
                statement = statement->nextStatement;
            }
            needsImplicitReturn = (statement->nodeType != HLSLNodeType_ReturnStatement) && function->returnType.baseType == HLSLBaseType_Void;
        }
        
        if (needsImplicitReturn)
        {
            m_writer.BeginLine(indent+1);
            m_writer.Write("return { ");
            
            int numArguments = 0;
            HLSLArgument * argument = m_currentFunction->argument;
            while (argument != NULL)
            {
                if (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout)
                {
                    if (numArguments) m_writer.Write(", ");
                    m_writer.Write("%s ", argument->name);
                    numArguments++;
                }
                argument = argument->nextArgument;
            }
            
            m_writer.EndLine(" };");
        }
    }
    
    m_writer.WriteLine(indent, "};");
    m_currentFunction = NULL;
}


// @@ We could be a lot smarter removing parenthesis based on the operator precedence of the parent expression.
static bool NeedsParenthesis(HLSLExpression* expression, HLSLExpression* parentExpression) {

    // For now we just omit the parenthesis if there's no parent expression.
    if (parentExpression == NULL)
    {
        return false;
    }

    // One more special case that's pretty common.
    if (parentExpression->nodeType == HLSLNodeType_MemberAccess)
    {
        if (expression->nodeType == HLSLNodeType_IdentifierExpression ||
            expression->nodeType == HLSLNodeType_ArrayAccess ||
            expression->nodeType == HLSLNodeType_MemberAccess)
        {
            return false;
        }
    }

    return true;
}

bool MSLGenerator::NeedsCast(const HLSLType & target, const HLSLType & source)
{
    HLSLBaseType targetType = target.baseType;
    HLSLBaseType sourceType = source.baseType;

    if (sourceType == HLSLBaseType_Int) {
        int k = 1;
    }
    
    /*if (IsScalarType(target))
    {
        // Scalar types do not need casting.
        return false;
    }*/

    if (m_options.treatHalfAsFloat)
    {
        if (IsHalf(targetType)) targetType = HLSLBaseType(targetType + HLSLBaseType_Float - HLSLBaseType_Half);
        if (IsHalf(sourceType)) sourceType = HLSLBaseType(sourceType + HLSLBaseType_Float - HLSLBaseType_Half);
    }
    
    return targetType != sourceType && (BaseTypeDimension[targetType] == BaseTypeDimension[sourceType] || BaseTypeDimension[sourceType] == HLSLTypeDimension_Scalar);
}
    

void MSLGenerator::OutputTypedExpression(const HLSLType& type, HLSLExpression* expression, HLSLExpression* parentExpression)
{
    // If base types are not exactly the same, do explicit cast.
    bool closeCastExpression = false;
    if (NeedsCast(type, expression->expressionType))
    {
        OutputCast(type);
        m_writer.Write("(");
        closeCastExpression = true;
    }
    
    OutputExpression(expression, parentExpression);
    
    if (closeCastExpression)
    {
        m_writer.Write(")");
    }
}

void MSLGenerator::OutputExpression(HLSLExpression* expression, HLSLExpression* parentExpression)
{
    if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression* identifierExpression = static_cast<HLSLIdentifierExpression*>(expression);
        const char* name = identifierExpression->name;
        // For texture declaration, generate a struct instead
        if (identifierExpression->global && IsSamplerType(identifierExpression->expressionType))
        {
            if (identifierExpression->expressionType.baseType == HLSLBaseType_Sampler2D)
            {
                if (identifierExpression->expressionType.samplerType == HLSLBaseType_Half && !m_options.treatHalfAsFloat)
                {
                    m_writer.Write("Texture2DHalfSampler(%s_texture, %s_sampler)", name, name);
                }
                else
                {
                    m_writer.Write("Texture2DSampler(%s_texture, %s_sampler)", name, name);
                }
            }
            else if (identifierExpression->expressionType.baseType == HLSLBaseType_Sampler3D)
                m_writer.Write("Texture3DSampler(%s_texture, %s_sampler)", name, name);
            else if (identifierExpression->expressionType.baseType == HLSLBaseType_SamplerCube)
                m_writer.Write("TextureCubeSampler(%s_texture, %s_sampler)", name, name);
            else if (identifierExpression->expressionType.baseType == HLSLBaseType_Sampler2DShadow)
                m_writer.Write("Texture2DShadowSampler(%s_texture, %s_sampler)", name, name);
            else if (identifierExpression->expressionType.baseType == HLSLBaseType_Sampler2DMS)
                m_writer.Write("%s_texture", name);
            else if (identifierExpression->expressionType.baseType == HLSLBaseType_Sampler2DArray)
                m_writer.Write("Texture2DArraySampler(%s_texture, %s_sampler)", name, name);
            else
                m_writer.Write("<unhandled texture type>");
        }
        else
        {
            if (identifierExpression->global)
            {
                HLSLBuffer * buffer;
                HLSLDeclaration * declaration = m_tree->FindGlobalDeclaration(identifierExpression->name, &buffer);

                if (declaration && declaration->buffer)
                {
                    ASSERT(buffer == declaration->buffer);
                    m_writer.Write("%s.", declaration->buffer->name);
                }
            }
            m_writer.Write("%s", name);

            // IC: Add swizzle if this is a member access of a field that has the swizzle flag.
            /*if (parentExpression->nodeType == HLSLNodeType_MemberAccess)
            {
                HLSLMemberAccess* memberAccess = (HLSLMemberAccess*)parentExpression;
                const HLSLType & objectType = memberAccess->object->expressionType;
                const HLSLStruct* structure = m_tree->FindGlobalStruct(objectType.typeName);
                if (structure != NULL)
                {
                    const HLSLStructField* field = structure->field;
                    while (field != NULL)
                    {
                        if (field->name == name)
                        {
                            if (field->type.flags & HLSLTypeFlag_Swizzle_BGRA)
                            {
                                m_writer.Write(".bgra", name);
                            }
                        }
                    }
                }
            }*/
        }
    }
    else if (expression->nodeType == HLSLNodeType_CastingExpression)
    {
        HLSLCastingExpression* castingExpression = static_cast<HLSLCastingExpression*>(expression);
        OutputCast(castingExpression->type);
        m_writer.Write("(");
        OutputExpression(castingExpression->expression, castingExpression);
        m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_ConstructorExpression)
    {
        HLSLConstructorExpression* constructorExpression = static_cast<HLSLConstructorExpression*>(expression);
        m_writer.Write("%s(", GetTypeName(constructorExpression->type));
        //OutputExpressionList(constructorExpression->type, constructorExpression->argument);   // @@ Get element type.
        OutputExpressionList(constructorExpression->argument);
        m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_LiteralExpression)
    {
        HLSLLiteralExpression* literalExpression = static_cast<HLSLLiteralExpression*>(expression);
        switch (literalExpression->type)
        {
            case HLSLBaseType_Half:
                if (m_options.treatHalfAsFloat) {
                    m_writer.Write("%ff", literalExpression->fValue);
                }
                else {
                    m_writer.Write("%fh", literalExpression->fValue);
                }
                break;
            case HLSLBaseType_Float:
                m_writer.Write("%ff", literalExpression->fValue);
                break;
            case HLSLBaseType_Int:
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
        switch (unaryExpression->unaryOp)
        {
            case HLSLUnaryOp_Negative:      op = "-";  break;
            case HLSLUnaryOp_Positive:      op = "+";  break;
            case HLSLUnaryOp_Not:           op = "!";  break;
            case HLSLUnaryOp_BitNot:        op = "~";  break;
            case HLSLUnaryOp_PreIncrement:  op = "++"; break;
            case HLSLUnaryOp_PreDecrement:  op = "--"; break;
            case HLSLUnaryOp_PostIncrement: op = "++"; pre = false; break;
            case HLSLUnaryOp_PostDecrement: op = "--"; pre = false; break;
        }
        bool addParenthesis = NeedsParenthesis(unaryExpression->expression, expression);
        if (addParenthesis) m_writer.Write("(");
        if (pre)
        {
            m_writer.Write("%s", op);
            OutputExpression(unaryExpression->expression, unaryExpression);
        }
        else
        {
            OutputExpression(unaryExpression->expression, unaryExpression);
            m_writer.Write("%s", op);
        }
        if (addParenthesis) m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_BinaryExpression)
    {
        HLSLBinaryExpression* binaryExpression = static_cast<HLSLBinaryExpression*>(expression);

        bool addParenthesis = NeedsParenthesis(expression, parentExpression);
        if (addParenthesis) m_writer.Write("(");

        bool rewrite_assign = false;
        if (binaryExpression->binaryOp == HLSLBinaryOp_Assign && binaryExpression->expression1->nodeType == HLSLNodeType_ArrayAccess)
        {
            HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(binaryExpression->expression1);
            if (!arrayAccess->array->expressionType.array && IsMatrixType(arrayAccess->array->expressionType.baseType))
            {
                rewrite_assign = true;

                m_writer.Write("set_column(");
                OutputExpression(arrayAccess->array, NULL);
                m_writer.Write(", ");
                OutputExpression(arrayAccess->index, NULL);
                m_writer.Write(", ");
                OutputExpression(binaryExpression->expression2, NULL);
                m_writer.Write(")");
            }
        }

        if (!rewrite_assign)
        {
            if (IsArithmeticOp(binaryExpression->binaryOp) || IsLogicOp(binaryExpression->binaryOp))
            {
                // Do intermediate type promotion, without changing dimension:
                HLSLType promotedType = binaryExpression->expression1->expressionType;
                
                if (ScalarBaseType[binaryExpression->expressionType.baseType] != ScalarBaseType[promotedType.baseType])
                {
                    promotedType.baseType = HLSLBaseType(ScalarBaseType[binaryExpression->expressionType.baseType] + BaseTypeDimension[promotedType.baseType] - 1);
                }
                
                OutputTypedExpression(promotedType, binaryExpression->expression1, binaryExpression);
            }
            else
            {
                OutputExpression(binaryExpression->expression1, binaryExpression);
            }
            
            const char* op = "?";
            switch (binaryExpression->binaryOp)
            {
                case HLSLBinaryOp_Add:          op = " + "; break;
                case HLSLBinaryOp_Sub:          op = " - "; break;
                case HLSLBinaryOp_Mul:          op = " * "; break;
                case HLSLBinaryOp_Div:          op = " / "; break;
                case HLSLBinaryOp_Less:         op = " < "; break;
                case HLSLBinaryOp_Greater:      op = " > "; break;
                case HLSLBinaryOp_LessEqual:    op = " <= "; break;
                case HLSLBinaryOp_GreaterEqual: op = " >= "; break;
                case HLSLBinaryOp_Equal:        op = " == "; break;
                case HLSLBinaryOp_NotEqual:     op = " != "; break;
                case HLSLBinaryOp_Assign:       op = " = "; break;
                case HLSLBinaryOp_AddAssign:    op = " += "; break;
                case HLSLBinaryOp_SubAssign:    op = " -= "; break;
                case HLSLBinaryOp_MulAssign:    op = " *= "; break;
                case HLSLBinaryOp_DivAssign:    op = " /= "; break;
                case HLSLBinaryOp_And:          op = " && "; break;
                case HLSLBinaryOp_Or:           op = " || "; break;
                case HLSLBinaryOp_BitAnd:       op = " & "; break;
                case HLSLBinaryOp_BitOr:        op = " | "; break;
                case HLSLBinaryOp_BitXor:       op = " ^ "; break;
                default:
                    ASSERT(0);
            }
            m_writer.Write("%s", op);
            
            if (binaryExpression->binaryOp == HLSLBinaryOp_MulAssign ||
                     binaryExpression->binaryOp == HLSLBinaryOp_DivAssign ||
                     IsArithmeticOp(binaryExpression->binaryOp) ||
                     IsLogicOp(binaryExpression->binaryOp))
            {
                // Do intermediate type promotion, without changing dimension:
                HLSLType promotedType = binaryExpression->expression2->expressionType;

                if (ScalarBaseType[binaryExpression->expressionType.baseType] != ScalarBaseType[promotedType.baseType])
                {
                    promotedType.baseType = HLSLBaseType(ScalarBaseType[binaryExpression->expressionType.baseType] + BaseTypeDimension[promotedType.baseType] - 1);
                }

                OutputTypedExpression(promotedType, binaryExpression->expression2, binaryExpression);
            }
            else if (IsAssignOp(binaryExpression->binaryOp))
            {
                OutputTypedExpression(binaryExpression->expressionType, binaryExpression->expression2, binaryExpression);
            }
            else
            {
                OutputExpression(binaryExpression->expression2, binaryExpression);
            }
        }
        if (addParenthesis) m_writer.Write(")");
    }
    else if (expression->nodeType == HLSLNodeType_ConditionalExpression)
    {
        HLSLConditionalExpression* conditionalExpression = static_cast<HLSLConditionalExpression*>(expression);
        // @@ Remove parenthesis.
        m_writer.Write("((");
        OutputExpression(conditionalExpression->condition, NULL);
        m_writer.Write(")?(");
        OutputExpression(conditionalExpression->trueExpression, NULL);
        m_writer.Write("):(");
        OutputExpression(conditionalExpression->falseExpression, NULL);
        m_writer.Write("))");
    }
    else if (expression->nodeType == HLSLNodeType_MemberAccess)
    {
        HLSLMemberAccess* memberAccess = static_cast<HLSLMemberAccess*>(expression);
        bool addParenthesis = NeedsParenthesis(memberAccess->object, expression);
        
        if (addParenthesis)
        {
            m_writer.Write("(");
        }
        OutputExpression(memberAccess->object, NULL);
        if (addParenthesis)
        {
            m_writer.Write(")");
        }
        
        m_writer.Write(".%s", memberAccess->field);
    }
    else if (expression->nodeType == HLSLNodeType_ArrayAccess)
    {
        HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(expression);
        if (arrayAccess->array->expressionType.array || !IsMatrixType(arrayAccess->array->expressionType.baseType))
        {
            OutputExpression(arrayAccess->array, expression);
            m_writer.Write("[");
            OutputExpression(arrayAccess->index, NULL);
            m_writer.Write("]");
        }
        else
        {
            // @@ This doesn't work for l-values!
            m_writer.Write("column(");
            OutputExpression(arrayAccess->array, NULL);
            m_writer.Write(", ");
            OutputExpression(arrayAccess->index, NULL);
            m_writer.Write(")");
        }
    }
    else if (expression->nodeType == HLSLNodeType_FunctionCall)
    {
        HLSLFunctionCall* functionCall = static_cast<HLSLFunctionCall*>(expression);
        OutputFunctionCall(functionCall, parentExpression);
    }
    else
    {
        m_writer.Write("<unknown expression>");
    }
}

void MSLGenerator::OutputCast(const HLSLType& type)
{
    if (type.baseType == HLSLBaseType_Float3x3)
    {
        m_writer.Write("matrix_ctor");
    }
    else
    {
        m_writer.Write("(");
        OutputDeclarationType(type, /*isConst=*/false, /*isRef=*/false, /*alignment=*/0, /*isTypeCast=*/true);
        m_writer.Write(")");
    }
}
 
// Called by the various Output functions
void MSLGenerator::OutputArguments(HLSLArgument* argument)
{
    int numArgs = 0;
    while (argument != NULL)
    {
        // Skip hidden and output arguments.
        if (argument->hidden || argument->modifier == HLSLArgumentModifier_Out)
        {
            argument = argument->nextArgument;
            continue;
        }

        if (numArgs > 0)
        {
            m_writer.Write(", ");
        }
        
        //bool isRef = false;
        bool isConst = false;
        /*if (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout)
        {
            isRef = true;
        }*/
        if (argument->modifier == HLSLArgumentModifier_In || argument->modifier == HLSLArgumentModifier_Const)
        {
            isConst = true;
        }
        
        OutputDeclaration(argument->type, argument->name, argument->defaultValue, /*isRef=*/false, isConst);
        argument = argument->nextArgument;
        ++numArgs;
    }
}
    
void MSLGenerator::OutputDeclaration(const HLSLType& type, const char* name, HLSLExpression* assignment, bool isRef, bool isConst, int alignment)
{
    OutputDeclarationType(type, isRef, isConst, alignment);
    OutputDeclarationBody(type, name, assignment, isRef);
}

void MSLGenerator::OutputDeclarationType(const HLSLType& type, bool isRef, bool isConst, int alignment, bool isTypeCast)
{
    const char* typeName = GetTypeName(type);
    
    /*if (!isTypeCast)*/
    {
        if (isRef && !isTypeCast)
        {
            m_writer.Write("thread ");
        }
        if (isConst || type.flags & HLSLTypeFlag_Const)
        {
            m_writer.Write("const ");

            if ((type.flags & HLSLTypeFlag_Static) != 0 && !isTypeCast)
            {
                m_writer.Write("static constant constexpr ");
            }
        }
    }
    if (IsSamplerType(type))
    {
        if (type.baseType == HLSLBaseType_Sampler2D) {
            if (type.samplerType == HLSLBaseType_Half && !m_options.treatHalfAsFloat) {
                typeName = "Texture2DHalfSampler";
            }
            else {
                typeName = "Texture2DSampler";
            }
        }
        else if (type.baseType == HLSLBaseType_Sampler3D)
            typeName = "Texture3DSampler";
        else if (type.baseType == HLSLBaseType_SamplerCube)
            typeName = "TextureCubeSampler";
        else if (type.baseType == HLSLBaseType_Sampler2DShadow)
            typeName = "Texture2DShadowSampler";
        else if (type.baseType == HLSLBaseType_Sampler2DMS)
            typeName = "Texture2DMSSampler";
        else if (type.baseType == HLSLBaseType_Sampler2DArray)
            typeName = "Texture2DArraySampler";
        else
            typeName = "<unhandled texture type>";
    }
    else if (alignment != 0 && !isTypeCast)
    {
        m_writer.Write("alignas(%d) ", alignment);
    }

    m_writer.Write("%s", typeName);

    if (isTypeCast)
    {
        // Do not output modifiers inside type cast expressions.
        return;
    }
    
    // Interpolation modifiers.
    if (type.flags & HLSLTypeFlag_NoInterpolation)
    {
        m_writer.Write(" [[flat]]");
    }
    else
    {
        if (type.flags & HLSLTypeFlag_NoPerspective)
        {
            if (type.flags & HLSLTypeFlag_Centroid)
            {
                m_writer.Write(" [[centroid_no_perspective]]");
            }
            else if (type.flags & HLSLTypeFlag_Sample)
            {
                m_writer.Write(" [[sample_no_perspective]]");
            }
            else
            {
                m_writer.Write(" [[center_no_perspective]]");
            }
        }
        else
        {
            if (type.flags & HLSLTypeFlag_Centroid)
            {
                m_writer.Write(" [[centroid_perspective]]");
            }
            else if (type.flags & HLSLTypeFlag_Sample)
            {
                m_writer.Write(" [[sample_perspective]]");
            }
            else
            {
                // Default.
                //m_writer.Write(" [[center_perspective]]");
            }
        }
    }
}
    
void MSLGenerator::OutputDeclarationBody(const HLSLType& type, const char* name, HLSLExpression* assignment, bool isRef)
{
    if (isRef)
    {
        // Arrays of refs are illegal in C++ and hence MSL, need to "link" the & to the var name
        m_writer.Write("(&");
    }
    
    // Then name
    m_writer.Write(" %s", name);
    
    if (isRef)
    {
        m_writer.Write(")");
    }
    
    // Add brackets for arrays
    if (type.array)
    {
        m_writer.Write("[");
        if (type.arraySize != NULL)
        {
            OutputExpression(type.arraySize, NULL);
        }
        m_writer.Write("]");
    }
    
    // Semantics and registers unhandled for now
    
    // Assignment handling
    if (assignment != NULL)
    {
        m_writer.Write(" = ");
        if (type.array)
        {
            m_writer.Write("{ ");
            OutputExpressionList(assignment);
            m_writer.Write(" }");
        }
        else
        {
            OutputTypedExpression(type, assignment, NULL);
        }
    }
}

void MSLGenerator::OutputExpressionList(HLSLExpression* expression)
{
    int numExpressions = 0;
    while (expression != NULL)
    {
        if (numExpressions > 0)
        {
            m_writer.Write(", ");
        }
        OutputExpression(expression, NULL);
        expression = expression->nextExpression;
        ++numExpressions;
    }
}

// Cast all expressions to given type.
void MSLGenerator::OutputExpressionList(const HLSLType & type, HLSLExpression* expression)
{
    int numExpressions = 0;
    while (expression != NULL)
    {
        if (numExpressions > 0)
        {
            m_writer.Write(", ");
        }
        
        OutputTypedExpression(type, expression, NULL);
        expression = expression->nextExpression;
        ++numExpressions;
    }
}

// Cast each expression to corresponding argument type.
void MSLGenerator::OutputExpressionList(HLSLArgument* argument, HLSLExpression* expression)
{
    int numExpressions = 0;
    while (expression != NULL)
    {
        ASSERT(argument != NULL);
        if (argument->modifier != HLSLArgumentModifier_Out)
        {
            if (numExpressions > 0)
            {
                m_writer.Write(", ");
            }
            
            OutputTypedExpression(argument->type, expression, NULL);
            ++numExpressions;
        }

        expression = expression->nextExpression;
        argument = argument->nextArgument;
    }
}


                                 
inline bool isAddressable(HLSLExpression* expression)
{
    if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        return true;
    }
    if (expression->nodeType == HLSLNodeType_ArrayAccess)
    {
        return true;
    }
    if (expression->nodeType == HLSLNodeType_MemberAccess)
    {
        HLSLMemberAccess* memberAccess = (HLSLMemberAccess*)expression;
        return !memberAccess->swizzle;
    }
    return false;
}


void MSLGenerator::OutputFunctionCallStatement(int indent, HLSLFunctionCall* functionCall, HLSLDeclaration* declaration)
{
    // Nothing special about these cases:
    if (functionCall->function->numOutputArguments == 0)
    {
        m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
        if (declaration)
        {
            OutputDeclaration(declaration);
        }
        else
        {
            OutputExpression(functionCall, NULL);
        }
        m_writer.EndLine(";");
        return;
    }
    
    
    // Transform this:
    // float foo = functionCall(bah, poo);
    
    // Into:
    // auto tmp = functionCall(bah, poo);
    // bah = tmp.bah;
    // poo = tmp.poo;
    // float foo = tmp.__result;

    const char* functionName = functionCall->function->name;

    m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
    m_writer.Write("auto out%d = %s_%d(", functionCall->line, functionName, functionCall->function->line);
    OutputExpressionList(functionCall->function->argument, functionCall->argument);
    m_writer.EndLine(");");

    HLSLExpression * expression = functionCall->argument;
    HLSLArgument * argument = functionCall->function->argument;
    while (argument != NULL)
    {
        if (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout)
        {
            m_writer.BeginLine(indent);
            OutputExpression(expression, NULL);
            // @@ This assignment may need a cast.
            m_writer.Write(" = ");
            if (NeedsCast(expression->expressionType, argument->type)) {
                m_writer.Write("(%s)", GetTypeName(expression->expressionType));
            }
            m_writer.Write("out%d.%s;", functionCall->line, argument->name);
            m_writer.EndLine();
        }
        
        expression = expression->nextExpression;
        argument = argument->nextArgument;
    }

    if (declaration)
    {
        m_writer.BeginLine(indent);
        OutputDeclarationType(declaration->type);
        m_writer.Write(" %s = out%d.__result;", declaration->name, functionCall->line);
        m_writer.EndLine();
    }
    
    
#if 0
    int argumentIndex = 0;
    HLSLArgument* argument = functionCall->function->argument;
    HLSLExpression* expression = functionCall->argument;
    while (argument != NULL)
    {
        if (!isAddressable(expression))
        {
            if (argument->modifier == HLSLArgumentModifier_Out)
            {
                m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
                OutputDeclarationType(argument->type);
                m_writer.Write("tmp%d;", argumentIndex);
                m_writer.EndLine();
            }
            else if (argument->modifier == HLSLArgumentModifier_Inout)
            {
                m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
                OutputDeclarationType(argument->type);
                m_writer.Write("tmp%d = ", argumentIndex);
                OutputExpression(expression, NULL);
                m_writer.EndLine(";");
            }
        }
        argument = argument->nextArgument;
        expression = expression->nextExpression;
        argumentIndex++;
    }

    m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
    const char* name = functionCall->function->name;
    m_writer.Write("%s(", name);
    //OutputExpressionList(functionCall->argument);

    // Output expression list with temporary substitution.
    argumentIndex = 0;
    argument = functionCall->function->argument;
    expression = functionCall->argument;
    while (expression != NULL)
    {
        if (!isAddressable(expression) && (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout))
        {
            m_writer.Write("tmp%d", argumentIndex);
        }
        else
        {
            OutputExpression(expression, NULL);
        }

        argument = argument->nextArgument;
        expression = expression->nextExpression;
        argumentIndex++;
        if (expression)
        {
            m_writer.Write(", ");
        }
    }
    m_writer.EndLine(");");

    argumentIndex = 0;
    argument = functionCall->function->argument;
    expression = functionCall->argument;
    while (expression != NULL)
    {
        if (!isAddressable(expression) && (argument->modifier == HLSLArgumentModifier_Out || argument->modifier == HLSLArgumentModifier_Inout))
        {
            m_writer.BeginLine(indent, functionCall->fileName, functionCall->line);
            OutputExpression(expression, NULL);
            m_writer.Write(" = tmp%d", argumentIndex);
            m_writer.EndLine(";");
        }

        argument = argument->nextArgument;
        expression = expression->nextExpression;
        argumentIndex++;
    }
#endif // 0
}

void MSLGenerator::OutputFunctionCall(HLSLFunctionCall* functionCall, HLSLExpression * parentExpression)
{
    if (functionCall->function->numOutputArguments > 0)
    {
        ASSERT(false);
    }
    
    const char* functionName = functionCall->function->name;
    
    // If function begins with tex, then it returns float4 or half4 depending on options.halfTextureSamplers
    /*if (strncmp(functionName, "tex", 3) == 0)
    {
        if (parentExpression && IsFloat(parentExpression->expressionType.baseType))
        {
            if (m_options.halfTextureSamplers)
            {
                OutputCast(functionCall->expressionType);
            }
        }
    }*/
    
    {
        m_writer.Write("%s(", functionName);
        OutputExpressionList(functionCall->function->argument, functionCall->argument);
        //OutputExpressionList(functionCall->argument);
        m_writer.Write(")");
    }
}

const char* MSLGenerator::TranslateInputSemantic(const char * semantic)
{
    if (semantic == NULL)
        return NULL;

    unsigned int length, index;
    ParseSemantic(semantic, &length, &index);

    if (m_target == MSLGenerator::Target_VertexShader)
    {
        if (String_Equal(semantic, "INSTANCE_ID")) return "instance_id";
        if (String_Equal(semantic, "SV_InstanceID")) return "instance_id";
        if (String_Equal(semantic, "VERTEX_ID")) return "vertex_id";
        if (String_Equal(semantic, "SV_VertexID")) return "vertex_id";

        if (m_options.attributeCallback)
        {
            char name[64];
            ASSERT(length < sizeof(name));

            strncpy(name, semantic, length);
            name[length] = 0;

            int attribute = m_options.attributeCallback(name, index);

            if (attribute >= 0)
            {
                return m_tree->AddStringFormat("attribute(%d)", attribute);
            }
        }
    }
    else if (m_target == MSLGenerator::Target_FragmentShader)
    {
        if (String_Equal(semantic, "POSITION")) return "position";
        if (String_Equal(semantic, "SV_Position")) return "position";
        if (String_Equal(semantic, "VFACE")) return "front_facing";
        if (String_Equal(semantic, "TARGET_INDEX")) return "render_target_array_index";
        if (String_Equal(semantic, "DST_COLOR")) return "color(0)";
        if (String_Equal(semantic, "SAMPLE_ID")) return "sample_id";
        //if (String_Equal(semantic, "SAMPLE_MASK")) return "sample_mask";
        //if (String_Equal(semantic, "SAMPLE_MASK")) return "sample_mask,post_depth_coverage";
    }

    return NULL;
}

const char* MSLGenerator::TranslateOutputSemantic(const char * semantic)
{
    if (semantic == NULL)
        return NULL;

    unsigned int length, index;
    ParseSemantic(semantic, &length, &index);

    if (m_target == MSLGenerator::Target_VertexShader)
    {
        if (String_Equal(semantic, "POSITION")) return "position";
        if (String_Equal(semantic, "SV_Position")) return "position";
        if (String_Equal(semantic, "PSIZE")) return "point_size";
        if (String_Equal(semantic, "POINT_SIZE")) return "point_size";
        if (String_Equal(semantic, "TARGET_INDEX")) return "render_target_array_index";
    }
    else if (m_target == MSLGenerator::Target_FragmentShader)
    {
        if (m_options.flags & MSLGenerator::Flag_NoIndexAttribute)
        {
            // No dual-source blending on iOS, and no index() attribute
            if (String_Equal(semantic, "COLOR0_1")) return NULL;
        }
        else
        {
            // @@ IC: Hardcoded for this specific case, extend ParseSemantic?
            if (String_Equal(semantic, "COLOR0_1")) return "color(0), index(1)";
        }

        if (strncmp(semantic, "SV_Target", length) == 0)
        {
            return m_tree->AddStringFormat("color(%d)", index);
        }
        if (strncmp(semantic, "COLOR", length) == 0)
        {
            return m_tree->AddStringFormat("color(%d)", index);
        }

        if (String_Equal(semantic, "DEPTH")) return "depth(any)";
        if (String_Equal(semantic, "DEPTH_GT")) return "depth(greater)";
        if (String_Equal(semantic, "DEPTH_LT")) return "depth(less)";
        if (String_Equal(semantic, "SAMPLE_MASK")) return "sample_mask";
    }

    return NULL;
}

const char* MSLGenerator::GetTypeName(const HLSLType& type)
{
    bool promote = ((type.flags & HLSLTypeFlag_NoPromote) == 0);
    
    bool float_to_half = false;
    bool half_to_float = promote && m_options.treatHalfAsFloat;
    bool int_to_short  = promote && m_options.use16BitIntegers;
    bool half_samplers = promote && type.samplerType == HLSLBaseType_Half && !m_options.treatHalfAsFloat;
    
    
    switch (type.baseType)
    {
        case HLSLBaseType_Void:             return "void";
        case HLSLBaseType_Float:            return float_to_half ? "half" : "float";
        case HLSLBaseType_Float2:           return float_to_half ? "half2" : "float2";
        case HLSLBaseType_Float3:           return float_to_half ? "half3" : "float3";
        case HLSLBaseType_Float4:           return float_to_half ? "half4" : "float4";
        case HLSLBaseType_Float2x2:         return float_to_half ? "half2x2" : "float2x2";
        case HLSLBaseType_Float3x3:         return float_to_half ? "half3x3" : "float3x3";
        case HLSLBaseType_Float4x4:         return float_to_half ? "half4x4" : "float4x4";
        case HLSLBaseType_Float4x3:         return float_to_half ? "half3x4" : "float3x4";
        case HLSLBaseType_Float4x2:         return float_to_half ? "half2x4" : "float2x4";
        case HLSLBaseType_Half:             return half_to_float ? "float" : "half";
        case HLSLBaseType_Half2:            return half_to_float ? "float2" : "half2";
        case HLSLBaseType_Half3:            return half_to_float ? "float3" : "half3";
        case HLSLBaseType_Half4:            return half_to_float ? "float4" : "half4";
        case HLSLBaseType_Half2x2:          return half_to_float ? "float2x2" : "half2x2";
        case HLSLBaseType_Half3x3:          return half_to_float ? "float3x3" : "half3x3";
        case HLSLBaseType_Half4x4:          return half_to_float ? "float4x4" : "half4x4";
        case HLSLBaseType_Half4x3:          return half_to_float ? "float3x4" : "half3x4";
        case HLSLBaseType_Half4x2:          return half_to_float ? "float2x4" : "half2x4";
        case HLSLBaseType_Bool:             return "bool";
        case HLSLBaseType_Bool2:            return "bool2";
        case HLSLBaseType_Bool3:            return "bool3";
        case HLSLBaseType_Bool4:            return "bool4";
        case HLSLBaseType_Int:              return int_to_short ? "short" : "int";
        case HLSLBaseType_Int2:             return int_to_short ? "short2" : "int2";
        case HLSLBaseType_Int3:             return int_to_short ? "short3" : "int3";
        case HLSLBaseType_Int4:             return int_to_short ? "short4" : "int4";
        case HLSLBaseType_Uint:             return int_to_short ? "ushort" : "uint";
        case HLSLBaseType_Uint2:            return int_to_short ? "ushort2" : "uint2";
        case HLSLBaseType_Uint3:            return int_to_short ? "ushort3" : "uint3";
        case HLSLBaseType_Uint4:            return int_to_short ? "ushort4" : "uint4";
        case HLSLBaseType_Texture:          return "texture";
        case HLSLBaseType_Sampler:          return "sampler";
        case HLSLBaseType_Sampler2D:        return half_samplers ? "texture2d<half>" : "texture2d<float>";
        case HLSLBaseType_Sampler3D:        return half_samplers ? "texture3d<half>" : "texture3d<float>";
        case HLSLBaseType_SamplerCube:      return half_samplers ? "texturecube<half>" : "texturecube<float>";
        case HLSLBaseType_Sampler2DShadow:  return "depth2d<float>";
        case HLSLBaseType_Sampler2DMS:      return half_samplers ? "texture2d_ms<half>" : "texture2d_ms<float>";
        case HLSLBaseType_Sampler2DArray:   return half_samplers ? "texture2d_array<half>" : "texture2d_array<float>";
        case HLSLBaseType_UserDefined:      return type.typeName;
        default:
            ASSERT(0);
            return "<unknown type>";
    }
}
    

    
} // M4
