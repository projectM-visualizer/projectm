#include "IdlePreset.hpp"
#include <sstream>
#include <string>

std::string IdlePreset::presetText() {

std::ostringstream out;

out <<
"[preset00]\n" << 
"fRating=2.000000\n"
"fGammaAdj=1.000000\n"
"fDecay=0.992000\n" <<
"fVideoEchoZoom=0.999400\n" <<
"fVideoEchoAlpha=0.500000\n" <<
"nVideoEchoOrientation=3\n" <<
"nWaveMode=7\n" <<
"bAdditiveWaves=1\n" <<
"bWaveDots=0\n" <<
"bWaveThick=1\n" <<
"bModWaveAlphaByVolume=0\n" <<
"bMaximizeWaveColor=0\n" <<
"bTexWrap=0\n" <<
"bDarkenCenter=0\n" <<
"bRedBlueStereo=0\n" <<
"bBrighten=0\n" <<
"bDarken=1\n" <<
"bSolarize=0\n" <<
"bInvert=0\n" <<
"fWaveAlpha=1.059200\n" <<
"fWaveScale=0.653093\n" <<
"fWaveSmoothing=0.270000\n" <<
"fWaveParam=-0.380000\n" <<
"fModWaveAlphaStart=0.750000\n" <<
"fModWaveAlphaEnd=0.950000\n" <<
"fWarpAnimSpeed=5.995790\n" <<
"fWarpScale=1.331000\n" <<
"fZoomExponent=1.000000\n" <<
"fShader=0.000000\n" <<
"zoom=1.008000\n" <<
"rot=0.001900\n" <<
"cx=0.500000\n" <<
"cy=0.500000\n" <<
"dx=0.000000\n" <<
"dy=0.000000\n" <<
"warp=0.010000\n" <<
"sx=1.000000\n" <<
"sy=1.000000\n" <<
"wave_r=1.000000\n" <<
"wave_g=1.000000\n" <<
"wave_b=1.000000\n" <<
"wave_x=0.500000\n" <<
"wave_y=0.500000\n" <<
"ob_size=0.005000\n" <<
"ob_r=0.000000\n" <<
"ob_g=0.000000\n" <<
"ob_b=0.000000\n" <<
"ob_a=1.000000\n" <<
"ib_size=0.010000\n" <<
"ib_r=0.500000\n" <<
"ib_g=0.900000\n" <<
"ib_b=0.500000\n" <<
"ib_a=1.000000\n" <<
"nMotionVectorsX=24.959999\n" <<
"nMotionVectorsY=19.199999\n" <<
"mv_dx=0.000000\n" <<
"mv_dy=0.000000\n" <<
"mv_l=0.850000\n" <<
"mv_r=0.499900\n" <<
"mv_g=0.499900\n" <<
"mv_b=0.499900\n" <<
"mv_a=1.000000\n" <<
"per_frame_1=warp=0;\n" <<
"per_frame_2=old_bass_flop=bass_flop;\n" <<
"per_frame_3=old_treb_flop=treb_flop;\n" <<
"per_frame_4=old_mid_flop=mid_flop;\n" <<
"per_frame_5=chaos=.9+.1*sin(pulse);\n" <<
"per_frame_6=entropy=if(equal(pulse,-20),1+bass_flop+treb_flop+mid_flop+rand(2),entropy);\n" <<
"per_frame_7=bass_thresh = above(bass_att,bass_thresh)*2 +(1-above(bass_att,bass_thresh))*((bass_thresh-1.6)*chaos+1.6);\n" <<
"per_frame_8=bass_flop=abs(bass_flop-equal(bass_thresh,2));\n" <<
"per_frame_9=treb_thresh=above(treb_att,treb_thresh)*2 + (1-above(treb_att,treb_thresh))*((treb_thresh-1.6)*chaos+1.6);\n" <<
"per_frame_10=treb_flop=abs(treb_flop-equal(treb_thresh,2));\n" <<
"per_frame_11=mid_thresh=above(mid_att,mid_thresh)*2 + (1-above(mid_att,mid_thresh))*((mid_thresh-1.6)*chaos+1.6);\n" <<
"per_frame_12=mid_flop=abs(mid_flop-equal(mid_thresh,2));\n" <<
"per_frame_13=bass_changed=bnot(equal(old_bass_flop,bass_flop));\n" <<
"per_frame_14=mid_changed=bnot(equal(old_mid_flop,mid_flop));\n" <<
"per_frame_15=treb_changed=bnot(equal(old_treb_flop,treb_flop));\n" <<
"per_frame_16=bass_residual = bass_changed*sin(pulse*3) + bnot(bass_changed)*bass_residual;\n" <<
"per_frame_17=treb_residual = treb_changed*sin(pulse*3) + bnot(treb_changed)*treb_residual;\n" <<
"per_frame_18=mid_residual = mid_changed*sin(pulse*3) + bnot(mid_changed)*mid_residual;\n" <<
"per_frame_19=pulse=if(above(abs(pulse),20),-20,pulse+(bass_thresh+mid_thresh+treb_thresh)*.018);\n" <<
"per_frame_20=q1=mid_residual;\n" <<
"per_frame_21=q2=bass_residual;\n" <<
"per_frame_22=q3=treb_residual;\n" <<
"per_frame_23=q4=sin(pulse);\n" <<
"per_frame_24=q5=cos(pulse*(.5+.1*entropy));\n" <<
"per_frame_25=q6=sin(pulse*(.5+pow(.25,entropy)));\n" <<
"per_frame_26=q7=above(q1,0)+above(q2,0)+above(q3,0)+above(q3,0)*treb_flop+above(q2,0)*bass_flop+above(q1,0)*mid_flop;\n" <<
"per_frame_27=q8=entropy;\n" <<
"per_frame_28=ob_r=.4+.4*sin(time*2.157+q6);\n" <<
"per_frame_29=ob_b=.8+.2*sin(time*1.689+q5);\n" <<
"per_frame_30=ob_g=.6+.4*sin(time*.413+q4);\n" <<
"per_frame_31=ib_r=.5+.5*cos(time*1.2+q1*.1);\n" <<
"per_frame_32=ib_b=.4+.4*cos(time*2.811+q2*.1);\n" <<
"per_frame_33=ib_g=.4+.4*cos(time*1.666+q3*.1);\n" <<
"per_frame_34=ib_size=.05+.03*q2;\n" <<
"per_frame_35=ob_size=.03+.02*sin(time*2.321+q2*.2);\n" <<
"per_frame_36=ob_a=.6+.4*q3;\n" <<
"per_frame_37=ib_a=.9+.1*sin(q2*.3+q4+q1*.5);\n" <<
"per_frame_38=mv_r=mv_r+.5*sin(q4+time*.678);\n" <<
"per_frame_39=mv_b=mv_b+.5*sin(q4+time*.789);\n" <<
"per_frame_40=mv_g=mv_g+.5*sin(q5+time*.456);\n" <<
"per_frame_41=mv_a=.2+.2*sin(time*1.178+q5*1.14);\n" <<
"per_frame_42=rot=0;\n" <<
"per_frame_43=wave_r=.6+.4*sin(q1+time*2.183);\n" <<
"per_frame_44=wave_b=.6+.4*sin(q2+time*1.211);\n" <<
"per_frame_45=wave_g=.6+.4*sin(q3+time*1.541);\n" <<
"per_frame_46=wave_mystery=wave_mystery+.5*sin(time*2.18+q6);\n" <<
"per_frame_47=wave_x=wave_x+.3*sin(time*.811)+.005*(frame%3);\n" <<
"per_frame_48=wave_y=wave_y+.3*sin(time*.788)+.005*(frame%3);\n" <<
"per_frame_49=wave_a=3+sin(time*1.414)+q3;\n" <<
"per_frame_50=wave_mode=q7;\n" <<
"per_frame_51=zoom=1+.7*sin(time*1.51);\n" <<
"per_pixel_1=rot=if(above(q4,0),rad*.2*q5,sin(rad*(q4+q3)));\n" <<
"per_pixel_2=zoom=if(above(q2,0),zoom+(1-zoom)*rot*cos(rad*3.14*q2),if(above(q3,0)*above(x,.5+.5*q5),zoom+(1-zoom)*sin(q1*rot*3.14),zoom+(1-zoom)*cos(rad*10*q6)));\n" <<
"per_pixel_3=dx=above(q1,0)*sin(rad*.5*q2);\n" <<
"per_pixel_4=dy=above(q3,0)*sin(rad*.5*q3);\n";

return out.str();

}

std::auto_ptr<Preset> IdlePreset::allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs)
{

  std::istringstream in(presetText());
  return std::auto_ptr<Preset>(new Preset(in, presetInputs, presetOutputs));
}




