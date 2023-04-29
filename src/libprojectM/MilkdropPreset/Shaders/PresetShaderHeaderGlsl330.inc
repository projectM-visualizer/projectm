#define  M_PI   3.14159265359
#define  M_PI_2 6.28318530718
#define  M_INV_PI_2  0.159154943091895

uniform float4   rand_frame;    // random float4, updated each frame
uniform float4   rand_preset;   // random float4, updated once per *preset*
uniform float4   _c0;           // .xy: multiplier to use on UV's to paste
                                // an image fullscreen, *aspect-aware*
                                // .zw = inverse.
uniform float4   _c1, _c2, _c3, _c4;
uniform float4   _c5;           // .xy = scale, bias for reading blur1
                                // .zw = scale, bias for reading blur2
uniform float4   _c6;           // .xy = scale, bias for reading blur3
                                // .zw = blur1_min, blur1_max
uniform float4   _c7;           // .xy ~= float2(1024,768)
                                // .zw ~= float2(1/1024.0, 1/768.0)
uniform float4   _c8;           // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.3, ~1.3, ~5, ~20))
uniform float4   _c9;           // .xyzw ~= same, but using sin()
uniform float4   _c10;          // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.005, ~0.008, ~0.013,
                                //                 ~0.022))
uniform float4   _c11;          // .xyzw ~= same, but using sin()
uniform float4   _c12;          // .xyz = mip info for main image
                                // (.x=#across, .y=#down, .z=avg)
                                // .w = unused
uniform float4   _c13;          // .xy = blur2_min, blur2_max
                                // .zw = blur3_min, blur3_max
uniform float4   _qa;           // q vars bank 1 [q1-q4]
uniform float4   _qb;           // q vars bank 2 [q5-q8]
uniform float4   _qc;           // q vars ...
uniform float4   _qd;           // q vars
uniform float4   _qe;           // q vars
uniform float4   _qf;           // q vars
uniform float4   _qg;           // q vars
uniform float4   _qh;           // q vars bank 8 [q29-q32]

// note: in general, don't use the current time w/the *dynamic* rotations!

// four random, static rotations, randomized at preset load time.
// minor translation component (<1).
uniform float4x3 rot_s1;
uniform float4x3 rot_s2;
uniform float4x3 rot_s3;
uniform float4x3 rot_s4;

// four random, slowly changing rotations.
uniform float4x3 rot_d1;
uniform float4x3 rot_d2;
uniform float4x3 rot_d3;
uniform float4x3 rot_d4;

// faster-changing.
uniform float4x3 rot_f1;
uniform float4x3 rot_f2;
uniform float4x3 rot_f3;
uniform float4x3 rot_f4;

// very-fast-changing.
uniform float4x3 rot_vf1;
uniform float4x3 rot_vf2;
uniform float4x3 rot_vf3;
uniform float4x3 rot_vf4;

// ultra-fast-changing.
uniform float4x3 rot_uf1;
uniform float4x3 rot_uf2;
uniform float4x3 rot_uf3;
uniform float4x3 rot_uf4;

// Random every frame.
uniform float4x3 rot_rand1;
uniform float4x3 rot_rand2;
uniform float4x3 rot_rand3;
uniform float4x3 rot_rand4;

#define time     _c2.x
#define fps      _c2.y
#define frame    _c2.z
#define progress _c2.w
#define bass _c3.x
#define mid  _c3.y
#define treb _c3.z
#define vol  _c3.w
#define bass_att _c4.x
#define mid_att  _c4.y
#define treb_att _c4.z
#define vol_att  _c4.w
#define q1 _qa.x
#define q2 _qa.y
#define q3 _qa.z
#define q4 _qa.w
#define q5 _qb.x
#define q6 _qb.y
#define q7 _qb.z
#define q8 _qb.w
#define q9 _qc.x
#define q10 _qc.y
#define q11 _qc.z
#define q12 _qc.w
#define q13 _qd.x
#define q14 _qd.y
#define q15 _qd.z
#define q16 _qd.w
#define q17 _qe.x
#define q18 _qe.y
#define q19 _qe.z
#define q20 _qe.w
#define q21 _qf.x
#define q22 _qf.y
#define q23 _qf.z
#define q24 _qf.w
#define q25 _qg.x
#define q26 _qg.y
#define q27 _qg.z
#define q28 _qg.w
#define q29 _qh.x
#define q30 _qh.y
#define q31 _qh.z
#define q32 _qh.w

#define aspect   _c0

// .xy = (w,h); .zw = (1/(float)w, 1/(float)h)
#define texsize  _c7

#define roam_cos _c8
#define roam_sin _c9
#define slow_roam_cos _c10
#define slow_roam_sin _c11
#define mip_x   _c12.x
#define mip_y   _c12.y
#define mip_xy  _c12.xy
#define mip_avg _c12.z
#define blur1_min _c6.z
#define blur1_max _c6.w
#define blur2_min _c13.x
#define blur2_max _c13.y
#define blur3_min _c13.z
#define blur3_max _c13.w

#define sampler_FC_main sampler_fc_main
#define sampler_PC_main sampler_pc_main
#define sampler_FW_main sampler_fw_main
#define sampler_PW_main sampler_pw_main

#define GetMain(uv) (tex2D(sampler_main,uv).xyz)
#define GetPixel(uv) (tex2D(sampler_main,uv).xyz)
#define GetBlur1(uv) (tex2D(sampler_blur1,uv).xyz*_c5.x + _c5.y)
#define GetBlur2(uv) (tex2D(sampler_blur2,uv).xyz*_c5.z + _c5.w)
#define GetBlur3(uv) (tex2D(sampler_blur3,uv).xyz*_c6.x + _c6.y)

#define lum(x) (dot(x,float3(0.32,0.49,0.29)))
#define tex2d tex2D
#define tex3d tex3D
