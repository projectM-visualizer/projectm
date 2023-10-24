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

#define iProgressLinear durationParams.x
#define iProgressCosine durationParams.y
#define iProgressBicubic durationParams.z
#define iTransitionDuration durationParams.w

#define iTime timeParams.x
#define iTimeDelta timeParams.y

#define iBass iBeatValues.x;
#define iMid iBeatValues.y;
#define iTreb iBeatValues.z;

#define iBassAtt iBeatAttValues.x;
#define iMidAtt iBeatAttValues.y;
#define iTrebAtt iBeatAttValues.z;

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

#define iNoiseLQ sampler_noise_lq;
#define iNoiseLQNearest sampler_pw_noise_lq;
#define iNoiseMQ sampler_noise_mq;
#define iNoiseMQNearest sampler_pw_noise_mq;
#define iNoiseHQ sampler_noise_hq;
#define iNoiseHQNearest sampler_pw_noise_hq;
#define iNoiseVolLQ sampler_noisevol_lq;
#define iNoiseVolLQNearest sampler_pw_noisevol_lq;
#define iNoiseVolHQ sampler_noisevol_hq;
#define iNoiseVolHQNearest sampler_pw_noisevol_hq;

// Shader output
out vec4 _prjm_transition_out;
