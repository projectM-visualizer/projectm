// Uniforms
uniform vec3 iResolution;
uniform vec4 durationParams;
uniform vec2 timeParams;
uniform float iFrameRate;
uniform int iFrame;
uniform ivec4 iRandStatic;
uniform ivec4 iRandFrame;
uniform vec3 iBeatValues;
uniform vec3 iBeatAttValues;

// Aspect ratio correction uniforms for geometry-sensitive transitions.
uniform float iAspectX;
uniform float iAspectY;
uniform float iInvAspectX;
uniform float iInvAspectY;

// Milkdrop-style easing function for transition progress.
// easingType: 0 = linear, 1 = smoothstep (default), 2 = ease-in, 3 = ease-out
float _prjm_getEasedProgress(float t, float easingType)
{
    t = clamp(t, 0.0, 1.0);
    if (easingType < 0.5)
    {
        return t;                                         // 0 = Linear
    }
    else if (easingType < 1.5)
    {
        return t * t * (3.0 - 2.0 * t);                   // 1 = Smoothstep (default)
    }
    else if (easingType < 2.5)
    {
        return t * t;                                     // 2 = Ease-in (quadratic)
    }
    else
    {
        return 1.0 - (1.0 - t) * (1.0 - t);               // 3 = Ease-out (quadratic)
    }
}

#define iProgressLinear  durationParams.x
#define iProgressCosine  durationParams.y
#define iProgressBicubic durationParams.z
#define iEasingType      durationParams.w
#define iProgressEased   _prjm_getEasedProgress(durationParams.x, durationParams.w)

#define iTime timeParams.x
#define iTimeDelta timeParams.y

#define iBass    iBeatValues.x
#define iMid     iBeatValues.y
#define iTreb    iBeatValues.z

#define iBassAtt iBeatAttValues.x
#define iMidAtt  iBeatAttValues.y
#define iTrebAtt iBeatAttValues.z

// Samplers
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

// These are named as in Milkdrop shaders so we can reuse the code.
uniform sampler2D sampler_noise_lq;
uniform sampler2D sampler_pw_noise_lq;
uniform sampler2D sampler_noise_mq;
uniform sampler2D sampler_pw_noise_mq;
uniform sampler2D sampler_noise_hq;
uniform sampler2D sampler_pw_noise_hq;
uniform sampler3D sampler_noisevol_lq;
uniform sampler3D sampler_pw_noisevol_lq;
uniform sampler3D sampler_noisevol_hq;
uniform sampler3D sampler_pw_noisevol_hq;

#define iNoiseLQ            sampler_noise_lq
#define iNoiseLQNearest     sampler_pw_noise_lq
#define iNoiseMQ            sampler_noise_mq
#define iNoiseMQNearest     sampler_pw_noise_mq
#define iNoiseHQ            sampler_noise_hq
#define iNoiseHQNearest     sampler_pw_noise_hq
#define iNoiseVolLQ         sampler_noisevol_lq
#define iNoiseVolLQNearest  sampler_pw_noisevol_lq
#define iNoiseVolHQ         sampler_noisevol_hq
#define iNoiseVolHQNearest  sampler_pw_noisevol_hq

// Shader output
out vec4 _prjm_transition_out;
