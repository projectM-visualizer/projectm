#include "IdlePreset.hpp"
#include <sstream>
#include <string>

const std::string IdlePreset::IDLE_PRESET_NAME
	("Geiss & Sperl - Feedback (projectM idle HDR mix)");

std::string IdlePreset::presetText() {

std::ostringstream out;

out << "[preset00]\n" <<
"fRating=3.000000\n" <<
"fGammaAdj=1\n" <<
"fDecay=0.9\n" <<
"fVideoEchoZoom=0.710682\n" <<
"fVideoEchoAlpha=0"
"nVideoEchoOrientation=0\n" <<
"nWaveMode=6\n" <<
"bAdditiveWaves=1\n" <<
"bWaveDots=0\n" <<
"bWaveThick=1\n" <<
"bModWaveAlphaByVolume=1\n" <<
"bMaximizeWaveColor=0\n" <<
"bTexWrap=1\n" <<
"bDarkenCenter=0\n" <<
"bRedBlueStereo=0\n" <<
"bBrighten=0\n" <<
"bDarken=0\n" <<
"bSolarize=0\n" <<
"bInvert=0\n" <<
"fWaveAlpha=0.311604\n" <<
"fWaveScale=1.22891\n" <<
"fWaveSmoothing=0\n" <<
"fWaveParam=0.2\n" <<
"fModWaveAlphaStart=0.71\n" <<
"fModWaveAlphaEnd=1.3\n" <<
"fWarpAnimSpeed=1\n" <<
"fWarpScale=1\n" <<
"fZoomExponent=1\n" <<
"fShader=0\n" <<
"zoom=0.999902\n" <<
"rot=0\n" <<
"cx=0.5\n" <<
"cy=0.5\n" <<
"dx=0\n" <<
"dy=0\n" <<
"warp=0.207965\n" <<
"sx=1\n" <<
"sy=1\n" <<
"wave_r=0.65\n" <<
"wave_g=0.65\n" <<
"wave_b=0.65\n" <<
"wave_x=0.5\n" <<
"wave_y=0.5\n" <<
"ob_size=0.0065\n" <<
"ob_r=0\n" <<
"ob_g=0\n" <<
"ob_b=0\n" <<
"ob_a=0\n" <<
"ib_size=0.26\n" <<
"ib_r=0.25\n" <<
"ib_g=0.25\n" <<
"ib_b=0.25\n" <<
"ib_a=0\n" <<
"nMotionVectorsX=64\n" <<
"nMotionVectorsY=48\n" <<
"mv_dx=0\n" <<
"mv_dy=0\n" <<
"mv_l=0.85\n" <<
"mv_r=0.4999\n" <<
"mv_g=0.4999\n" <<
"mv_b=0.4999\n" <<
"mv_a=0\n" <<
"shapecode_0_enabled=1\n" <<
"shapecode_0_sides=4\n" <<
"shapecode_0_additive=0\n" <<
"shapecode_0_thickOutline=0\n" <<
"shapecode_0_textured=0\n" <<
"shapecode_0_x=0.37\n" <<
"shapecode_0_y=0.5\n" <<
"shapecode_0_rad=4.999997\n" <<
"shapecode_0_ang=3.644249\n" <<
"shapecode_0_tex_ang=0\n" <<
"shapecode_0_tex_zoom=1\n" <<
"shapecode_0_r=0\n" <<
"shapecode_0_g=0\n" <<
"shapecode_0_b=0\n" <<
"shapecode_0_a=0.5\n" <<
"shapecode_0_r2=0\n" <<
"shapecode_0_g2=0\n" <<
"shapecode_0_b2=0\n" <<
"shapecode_0_a2=0.3\n" <<
"shapecode_0_border_r=1\n" <<
"shapecode_0_border_g=1\n" <<
"shapecode_0_border_b=1\n" <<
"shapecode_0_border_a=1\n" <<
"shape_0_init1=t1 = rand(100)*0.01;\n" <<
"shape_0_init2=t2 = rand(100)*0.01;\n" <<
"shape_0_per_frame1=ang = time*(0.3 + 0.1*t1);\n" <<
"shape_0_per_frame2=rad = rad * (0.9 + 0.2*t2);\n" <<
"shape_0_per_frame3=r = min(1,max(0,r + 0.2*sin(time*0.417 + 1)));\n" <<
"shape_0_per_frame4=g = min(1,max(0,g + 0.2*sin(time*0.391 + 2)));\n" <<
"shape_0_per_frame5=b = min(1,max(0,b + 0.2*sin(time*0.432 + 4)));\n" <<
"shape_0_per_frame6=r2 = min(1,max(0,r2 + 0.2*sin(time*0.657 + 3)));\n" <<
"shape_0_per_frame7=g2 = min(1,max(0,g2 + 0.2*sin(time*0.737 + 5)));\n" <<
"shape_0_per_frame8=b2 = min(1,max(0,b2 + 0.2*sin(time*0.884 + 6)));\n" <<
"shapecode_1_enabled=1\n" <<
"shapecode_1_sides=5\n" <<
"shapecode_1_additive=1\n" <<
"shapecode_1_thickOutline=0\n" <<
"shapecode_1_textured=1\n" <<
"shapecode_1_x=0.37\n" <<
"shapecode_1_y=0.5\n" <<
"shapecode_1_rad=0.706533\n" <<
"shapecode_1_ang=3.644249\n" <<
"shapecode_1_tex_ang=0\n" <<
"shapecode_1_tex_zoom=1\n" <<
"shapecode_1_r=1\n" <<
"shapecode_1_g=1\n" <<
"shapecode_1_b=1\n" <<
"shapecode_1_a=0.8\n" <<
"shapecode_1_r2=1\n" <<
"shapecode_1_g2=1\n" <<
"shapecode_1_b2=1\n" <<
"shapecode_1_a2=0\n" <<
"shapecode_1_border_r=1\n" <<
"shapecode_1_border_g=1\n" <<
"shapecode_1_border_b=1\n" <<
"shapecode_1_border_a=0\n" <<
"shape_1_init1=t1 = rand(100)*0.01;\n" <<
"shape_1_init2=t2 = rand(100)*0.01;\n" <<
"shape_1_per_frame1=x = x + 0.05*sin(time*1.25+3);\n" <<
"shape_1_per_frame2=y = y + 0.03*sin(time*1.49+1);\n" <<
"shape_1_per_frame3=ang = time*(0.3 + 0.1*t1);\n" <<
"shape_1_per_frame4=rad = rad * (0.9 + 0.2*t2);\n" <<
"shape_1_per_frame5=r = min(1,max(0,r + 0.1*sin(time*0.417 + 1)));\n" <<
"shape_1_per_frame6=g = min(1,max(0,g + 0.1*sin(time*0.391 + 2)));\n" <<
"shape_1_per_frame7=b = min(1,max(0,b + 0.1*sin(time*0.432 + 4)));\n" <<
"shape_1_per_frame8=r2 = min(1,max(0,r2 + 0.1*sin(time*0.457 + 3)));\n" <<
"shape_1_per_frame9=g2 = min(1,max(0,g2 + 0.1*sin(time*0.437 + 5)));\n" <<
"shape_1_per_frame10=b2 = min(1,max(0,b2 + 0.1*sin(time*0.484 + 6)));\n" <<
"shapecode_2_enabled=1\n" <<
"shapecode_2_sides=5\n" <<
"shapecode_2_additive=1\n" <<
"shapecode_2_thickOutline=0\n" <<
"shapecode_2_textured=1\n" <<
"shapecode_2_x=0.67\n" <<
"shapecode_2_y=0.43\n" <<
"shapecode_2_rad=0.706533\n" <<
"shapecode_2_ang=4.209736\n" <<
"shapecode_2_tex_ang=0\n" <<
"shapecode_2_tex_zoom=1\n" <<
"shapecode_2_r=1\n" <<
"shapecode_2_g=1\n" <<
"shapecode_2_b=1\n" <<
"shapecode_2_a=0.8\n" <<
"shapecode_2_r2=1\n" <<
"shapecode_2_g2=1\n" <<
"shapecode_2_b2=1\n" <<
"shapecode_2_a2=0\n" <<
"shapecode_2_border_r=1\n" <<
"shapecode_2_border_g=1\n" <<
"shapecode_2_border_b=1\n" <<
"shapecode_2_border_a=0\n" <<
"shape_2_init1=t1 = rand(100)*0.01;\n" <<
"shape_2_init2=t2 = rand(100)*0.01;\n" <<
"shape_2_per_frame1=x = x + 0.05*sin(time*2.17);\n" <<
"shape_2_per_frame2=y = y + 0.03*sin(time*1.83);\n" <<
"shape_2_per_frame3=ang = time*(0.3 + 0.1*t1);\n" <<
"shape_2_per_frame4=rad = rad * (0.9 + 0.2*t2);\n" <<
"shape_2_per_frame5=r = min(1,max(0,r + 0.1*sin(time*0.417 + 1)));\n" <<
"shape_2_per_frame6=g = min(1,max(0,g + 0.1*sin(time*0.391 + 2)));\n" <<
"shape_2_per_frame7=b = min(1,max(0,b + 0.1*sin(time*0.432 + 4)));\n" <<
"shape_2_per_frame8=r2 = min(1,max(0,r2 + 0.1*sin(time*0.457 + 3)));\n" <<
"shape_2_per_frame9=g2 = min(1,max(0,g2 + 0.1*sin(time*0.437 + 5)));\n" <<
"shape_2_per_frame10=b2 = min(1,max(0,b2 + 0.1*sin(time*0.484 + 6)));\n" <<
"shapecode_5_enabled=1\n" <<
"shapecode_5_sides=16\n" <<
"shapecode_5_additive=1\n" <<
"shapecode_5_thickOutline=1\n" <<
"shapecode_5_textured=0\n" <<
"shapecode_5_x=0.68\n" <<
"shapecode_5_y=0.5\n" <<
"shapecode_5_rad=0.509999\n" <<
"shapecode_5_ang=0\n" <<
"shapecode_5_tex_ang=0\n" <<
"shapecode_5_tex_zoom=1\n" <<
"shapecode_5_r=1\n" <<
"shapecode_5_g=0\n" <<
"shapecode_5_b=0\n" <<
"shapecode_5_a=0.46\n" <<
"shapecode_5_r2=0\n" <<
"shapecode_5_g2=1\n" <<
"shapecode_5_b2=0\n" <<
"shapecode_5_a2=0\n" <<
"shapecode_5_border_r=0\n" <<
"shapecode_5_border_g=1\n" <<
"shapecode_5_border_b=1\n" <<
"shapecode_5_border_a=0\n" <<
"shape_5_per_frame1=ang = time*1.4;\n" <<
"shape_5_per_frame2=x = x + q1;\n" <<
"shape_5_per_frame3=y = y + q2;\n" <<
"shape_5_per_frame4=r = 0.5 + 0.5*sin(time*0.713 + 1);\n" <<
"shape_5_per_frame5=g = 0.5 + 0.5*sin(time*0.563 + 2);\n" <<
"shape_5_per_frame6=b = 0.5 + 0.5*sin(time*0.654 + 5);\n" <<
"shape_5_per_frame7=r2 = 0.5 + 0.5*sin(time*0.885 + 4);\n" <<
"shape_5_per_frame8=g2 = 0.5 + 0.5*sin(time*0.556+ 1);\n" <<
"shape_5_per_frame9=b2 = 0.5 + 0.5*sin(time*0.638 + 3);\n" <<
"shapecode_3_enabled=1\n" <<
"shapecode_3_sides=4\n" <<
"shapecode_3_additive=0\n" <<
"shapecode_3_thickOutline=0\n" <<
"shapecode_3_textured=1\n" <<
"shapecode_3_ImageURL=M.tga\n" <<
"shapecode_3_x=0.68\n" <<
"shapecode_3_y=0.5\n" <<
"shapecode_3_rad=0.41222\n" <<
"shapecode_3_ang=0\n" <<
"shapecode_3_tex_ang=0\n" <<
"shapecode_3_tex_zoom=0.71\n" <<
"shapecode_3_r=1\n" <<
"shapecode_3_g=1\n" <<
"shapecode_3_b=1\n" <<
"shapecode_3_a=1\n" <<
"shapecode_3_r2=1\n" <<
"shapecode_3_g2=1\n" <<
"shapecode_3_b2=1\n" <<
"shapecode_3_a2=1\n" <<
"shapecode_3_border_r=0\n" <<
"shapecode_3_border_g=0\n" <<
"shapecode_3_border_b=0\n" <<
"shapecode_3_border_a=0\n" <<
"shape_3_per_frame1=x = x + q1;\n" <<
"shape_3_per_frame2=y = y + q2;\n" <<
"shape_3_per_frame3=b2 = (sin(time)+1)*0.5;\n" <<
"shape_3_per_frame4=b = (sin(time)+1)*0.5;\n" <<
"shape_3_per_frame5=g2 = (cos(time*0.75+1)+1)*0.5;\n" <<
"shape_3_per_frame6=g = (cos(time*0.75+1)+1)*0.5;\n" <<
"shape_3_per_frame7=r2 = (sin(time*1.25+2)+1)*0.5;\n" <<
"shape_3_per_frame8=r = (sin(time*1.25+2)+1)*0.5;\n" <<
"shape_3_per_frame9=a = q3;\n" <<
"shape_3_per_frame10=a2 = q3;\n" <<
"shapecode_4_enabled=1\n" <<
"shapecode_4_sides=4\n" <<
"shapecode_4_additive=0\n" <<
"shapecode_4_thickOutline=0\n" <<
"shapecode_4_textured=1\n" <<
"shapecode_4_ImageURL=headphones.tga\n" <<
"shapecode_4_x=0.68\n" <<
"shapecode_4_y=0.58\n" <<
"shapecode_4_rad=0.6\n" <<
"shapecode_4_ang=0\n" <<
"shapecode_4_tex_ang=0\n" <<
"shapecode_4_tex_zoom=0.71\n" <<
"shapecode_4_r=1\n" <<
"shapecode_4_g=1\n" <<
"shapecode_4_b=1\n" <<
"shapecode_4_a=1\n" <<
"shapecode_4_r2=1\n" <<
"shapecode_4_g2=1\n" <<
"shapecode_4_b2=1\n" <<
"shapecode_4_a2=1\n" <<
"shapecode_4_border_r=0\n" <<
"shapecode_4_border_g=0\n" <<
"shapecode_4_border_b=0\n" <<
"shapecode_4_border_a=0\n" <<
"shape_4_per_frame1=x = x + q1;\n" <<
"shape_4_per_frame2=y = y + q2;\n" <<
"shape_4_per_frame3=rad = rad + bass * 0.1;\n" <<
"shape_4_per_frame4=a = q3;\n" <<
"shape_4_per_frame5=a2 = q3;\n" <<
"shapecode_6_enabled=1\n" <<
"shapecode_6_sides=4\n" <<
"shapecode_6_additive=0\n" <<
"shapecode_6_thickOutline=0\n" <<
"shapecode_6_textured=1\n" <<
"shapecode_6_ImageURL=project.tga\n" <<
"shapecode_6_x=0.38\n" <<
"shapecode_6_y=0.435\n" <<
"shapecode_6_rad=0.8\n" <<
"shapecode_6_ang=0\n" <<
"shapecode_6_tex_ang=0\n" <<
"shapecode_6_tex_zoom=0.71\n" <<
"shapecode_6_r=1\n" <<
"shapecode_6_g=1\n" <<
"shapecode_6_b=1\n" <<
"shapecode_6_a=1\n" <<
"shapecode_6_r2=1\n" <<
"shapecode_6_g2=1\n" <<
"shapecode_6_b2=1\n" <<
"shapecode_6_a2=1\n" <<
"shapecode_6_border_r=0\n" <<
"shapecode_6_border_g=0\n" <<
"shapecode_6_border_b=0\n" <<
"shapecode_6_border_a=0\n" <<
"shape_6_per_frame1=x = x + q1;\n" <<
"shape_6_per_frame2=y = y + q2;\n" <<
"shape_6_per_frame3=a = q3;\n" <<
"shape_6_per_frame4=a2 = q3;\n" <<
"per_frame_1=wave_mystery = time%5*0.2;\n" <<
"per_frame_2=q1 = 0.15*sin(time*1.14);\n" <<
"per_frame_3=q2 = 0.1*sin(time*0.93+2);\n" <<
"per_frame_4=q3 = if(above(frame,60),1, frame/60.0);\n";

return out.str();

}

std::auto_ptr<Preset> IdlePreset::allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs)
{

  std::istringstream in(presetText());
  return std::auto_ptr<Preset>(new Preset(in, IDLE_PRESET_NAME, presetInputs, presetOutputs));
}

