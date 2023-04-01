#include "IdlePreset.hpp"
#include "MilkdropPreset.hpp"
#include <sstream>
#include <string>

// Original preset name: "Geiss & Sperl - Feedback (projectM idle HDR mix).milk"

std::string IdlePresets::presetText()
{    return R"([preset00]
fRating=2.000000
fGammaAdj=1.700000
fDecay=0.940000
fVideoEchoZoom=1.025000
fVideoEchoAlpha=0.100000
nVideoEchoOrientation=0
nWaveMode=6
bAdditiveWaves=1
bWaveDots=0
bWaveThick=1
bModWaveAlphaByVolume=0
bMaximizeWaveColor=0
bTexWrap=1
bDarkenCenter=0
bRedBlueStereo=0
bBrighten=0
bDarken=0
bSolarize=0
bInvert=0
fWaveAlpha=0.000000
fWaveScale=0.100000
fWaveSmoothing=0.630000
fWaveParam=0.000000
fModWaveAlphaStart=1.00000
fModWaveAlphaEnd=1.000000
fWarpAnimSpeed=1.000000
fWarpScale=1.331000
fZoomExponent=1.000000
fShader=0.000000
zoom=1.000000
rot=-0.020000
cx=0.500000
cy=0.500000
dx=-0.280000
dy=-0.320000
warp=0.010000
sx=1.000000
sy=1.000000
wave_r=1.000000
wave_g=1.000000
wave_b=1.000000
wave_x=0.100000
wave_y=0.500000
ob_size=0.010000
ob_r=0.010000
ob_g=0.000000
ob_b=0.000000
ob_a=1.000000
ib_size=0.010000
ib_r=0.950000
ib_g=0.850000
ib_b=0.650000
ib_a=1.000000
nMotionVectorsX=64.000000
nMotionVectorsY=0.000000
mv_dx=0.000000
mv_dy=0.000000
mv_l=0.900000
mv_r=1.000000
mv_g=1.000000
mv_b=1.000000
mv_a=0.000000
shapecode_0_enabled=1
shapecode_0_sides=4
shapecode_0_additive=0
shapecode_0_thickOutline=0
shapecode_0_textured=1
shapecode_0_image=idlem
shapecode_0_x=0.5
shapecode_0_y=0.5
shapecode_0_rad=0.41222
shapecode_0_ang=0
shapecode_0_tex_ang=0
shapecode_0_tex_zoom=0.707106781186
shapecode_0_r=1
shapecode_0_g=1
shapecode_0_b=1
shapecode_0_a=1
shapecode_0_r2=1
shapecode_0_g2=1
shapecode_0_b2=1
shapecode_0_a2=1
shapecode_0_border_r=0
shapecode_0_border_g=0
shapecode_0_border_b=0
shapecode_0_border_a=0
shape_0_per_frame1=x = x + q1;
shape_0_per_frame2=y = y + q2;
shape_0_per_frame3=r =0.5 + 0.5*sin(q8*0.613 + 1);
shape_0_per_frame4=g = 0.5 + 0.5*sin(q8*0.763 + 2);
shape_0_per_frame5=b = 0.5 + 0.5*sin(q8*0.771 + 5);
shape_0_per_frame6=r2 = 0.5 + 0.5*sin(q8*0.635 + 4);
shape_0_per_frame7=g2 = 0.5 + 0.5*sin(q8*0.616+ 1);
shape_0_per_frame8=b2 = 0.5 + 0.5*sin(q8*0.538 + 3);
shape_0_per_frame9=ang = q10;
shapecode_1_enabled=1
shapecode_1_sides=4
shapecode_1_additive=0
shapecode_1_thickOutline=0
shapecode_1_textured=1
shapecode_1_image=idleheadphones
shapecode_1_x=0.5
shapecode_1_y=0.5
shapecode_1_rad=0.6
shapecode_1_ang=0
shapecode_1_tex_ang=0
shapecode_1_tex_zoom=0.707106781186
shapecode_1_r=1
shapecode_1_g=1
shapecode_1_b=1
shapecode_1_a=1
shapecode_1_r2=1
shapecode_1_g2=1
shapecode_1_b2=1
shapecode_1_a2=1
shapecode_1_border_r=0
shapecode_1_border_g=0
shapecode_1_border_b=0
shapecode_1_border_a=0
shape_1_per_frame1=ang = q10;
shape_1_per_frame2=x = x + q1 + sin(q10) * q9 * rad;
shape_1_per_frame3=y = y + q2 + cos(q10) * q9;
shape_1_per_frame4=rad = rad + bass * 0.1;
shape_1_per_frame5=a = q3;
shape_1_per_frame6=a2 = q3;
wavecode_0_enabled=1
wavecode_0_bDrawThick=1
wavecode_0_bSpectrum=0
wavecode_0_bAdditive=1
wavecode_0_samples=480
wavecode_0_scaling=1.000000
wavecode_0_smoothing=0.0
wavecode_0_r=1.0
wavecode_0_g=0.0
wavecode_0_b=0.0
wavecode_0_a=1.0
wave_0_per_point1=y = sin((sample * 2 * $PI + time)) * (sin(time) * 0.05 + 0.025) + 0.3 + value1 / 2;
wave_0_per_point2=x = sample;
wave_0_per_point3=r = 0.5 + 0.5*sin(q8*0.613 + 1);
wave_0_per_point4=g = 0.5 + 0.5*sin(q8*0.763 + 2);
wave_0_per_point5=b = 0.5 + 0.5*sin(q8*0.771 + 5);
wave_0_per_point6=
wave_0_per_point7=
per_frame_1=ob_r = 0.5 + 0.4*sin(time*1.324);
per_frame_2=ob_g = 0.5 + 0.4*cos(time*1.371);
per_frame_3=ob_b = 0.5+0.4*sin(2.332*time);
per_frame_4=ib_r = 0.5 + 0.25*sin(time*1.424); wave_r = ib_r;
per_frame_5=ib_g = 0.25 + 0.25*cos(time*1.871); wave_g = ib_g;
per_frame_6=ib_b = 1-ob_b; wave_b = ib_b;
per_frame_7=volume = 0.15*(bass+bass_att+treb+treb_att+mid+mid_att);
per_frame_8=xamptarg = if(equal(frame%15,0),min(0.5*volume*bass_att,0.5),xamptarg);
per_frame_9=xamp = xamp + 0.5*(xamptarg-xamp);
per_frame_10=xdir = if(above(abs(xpos),xamp),-sign(xpos),if(below(abs(xspeed),0.1),2*above(xpos,0)-1,xdir));
per_frame_11=xaccel = xdir*xamp - xpos - xspeed*0.055*below(abs(xpos),xamp);
per_frame_12=xspeed = xspeed + xdir*xamp - xpos - xspeed*0.055*below(abs(xpos),xamp);
per_frame_13=xpos = xpos + 0.001*xspeed;
per_frame_14=dx = xpos*0.05;
per_frame_15=yamptarg = if(equal(frame%15,0),min(0.3*volume*treb_att,0.5),yamptarg);
per_frame_16=yamp = yamp + 0.5*(yamptarg-yamp);
per_frame_17=ydir = if(above(abs(ypos),yamp),-sign(ypos),if(below(abs(yspeed),0.1),2*above(ypos,0)-1,ydir));
per_frame_18=yaccel = ydir*yamp - ypos - yspeed*0.055*below(abs(ypos),yamp);
per_frame_19=yspeed = yspeed + ydir*yamp - ypos - yspeed*0.055*below(abs(ypos),yamp);
per_frame_20=ypos = ypos + 0.001*yspeed;
per_frame_21=dy = ypos*0.05;
per_frame_22=q5 = aspecty;
per_frame_23=q8 = oldq8 + 0.0003*(pow(1+1.2*bass+0.4*bass_att+0.1*treb+0.1*treb_att+0.1*mid+0.1*mid_att,6)/fps);
per_frame_24=oldq8 = q8;
per_frame_25=q7 = 0.003*(pow(1+1.2*bass+0.4*bass_att+0.1*treb+0.1*treb_att+0.1*mid+0.1*mid_att,6)/fps);
per_frame_26=rot = 0.4 + 1.5*sin(time*0.273) + 0.4*sin(time*0.379+3);
per_frame_27=q1 = 0.2*sin(time*1.14);
per_frame_28=q2 = 0.15*sin(time*0.93+2); wave_x = 0.58 - q2;
per_frame_29=q3 = if(above(frame,60),1, frame/60.0);
per_frame_30=oldq8 = if(above(oldq8,1000),0,oldq8);
per_frame_31=cx = cx + 0.3 * (0.6 * sin(0.245 * time) + 0.4 * sin(0.123 * time));
per_frame_31=cy = cy + 0.3 * (0.6 * sin(0.263 * time) + 0.4 * sin(0.117 * time));
per_frame_32=q9 = -0.12; // Headphones Y offset
per_frame_33=q10 = 0.5 * sin(time * 0.5623); // Logo tilt
per_pixel_1=zoom = log(sqrt(sin(time * .5) * 0.5 + 2.5) - .5 * rad) + 0.4;
)";
}

std::unique_ptr<Preset>
IdlePresets::allocate()
{
    std::istringstream in(presetText());
    return std::unique_ptr<Preset>(new MilkdropPreset(in));
}
