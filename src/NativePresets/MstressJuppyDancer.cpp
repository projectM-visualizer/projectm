#include <cmath>

#include "Pipeline.hpp"
#include "Transformation.hpp"
#include "MilkdropCompatability.hpp"
#include "Waveform.hpp"
#include "NativePreset.hpp"

class Dancer : public Waveform
{
public:

	float meanbass;
	float meanmid;
	float meantreb;
	float mi2_prg;
	float gam;
	float mi_prg;
	float ba_prg;
	float tr_prg;

	float dim;
	float xpos;
	float ypos;


	float rand_offset1;
	float rand_offset2;
	float rand_offset3;


	Dancer():Waveform(16)
	{
		thick = true;
		smoothing = 0.5;

		meanbass = 0;
		meanmid = 0;
		meantreb = 0;
		mi2_prg = 0;
		gam = 0;
		mi_prg = 0;
		ba_prg = 0;
		tr_prg = 0;

		dim = (rand()%20)*.01;
		//dim *= 0.5;
		dim += 0.4;


		ypos = ((rand()%60)*.01)-0.3;
		xpos = ((rand()%60)*.01)-0.3;
		xpos+=(1-dim) * 0.5;
		ypos+=(1-dim) * 0.5;

		rand_offset1= (rand()%200)*.01;
		rand_offset2= (rand()%400)*.01;
		rand_offset3= (rand()%600)*.01;

	}

	ColoredPoint PerPoint(ColoredPoint p, const WaveformContext context)
	{


		meanbass = 0.01*(meanbass*99+context.music->bass);
		meantreb = 0.01*(meantreb*99+context.music->treb);
		meanmid = 0.01*(meanmid*99+context.music->mid);
		float bassdiff = (context.music->bass - meanbass)*15;
		float trebdiff = (context.music->treb - meantreb)*15;
		float middiff = (context.music->mid - meanmid)*15;
		float ba = min(above(bassdiff,0)*bassdiff*.005,.11);
		float tr = min(above(trebdiff,0)*trebdiff*.005,.11);
		float mi = min(above(middiff,0)*middiff*.005,.11);

		mi2_prg = mi2_prg+mi;
        gam = std::abs(gam-above(mi2_prg,5));
		mi2_prg= if_milk(above(mi2_prg,5),0,mi2_prg);

		float s = context.sample_int;
		//Gambe
		p.x= if_milk(equal(int(s),1),.4,.4);
		p.y= if_milk(equal(int(s),1),.2+((ba+tr)*.5)*gam,.2+((ba+tr)*.5)*gam);

		p.x= if_milk(equal(int(s),2),.5+sin(ba*100)*.03,p.x);
		p.y= if_milk(equal(int(s),2),.4,p.y);

		p.x= if_milk(equal(int(s),3),.6,p.x);
		p.y= if_milk(equal(int(s),3),.2+((ba+tr)*.5)*(1-gam),p.y);

		p.x= if_milk(equal(int(s),4),.5+sin(ba*100)*.03,p.x);
		p.y= if_milk(equal(int(s),4),.4,p.y);

		//Corpo
		p.x= if_milk(equal(int(s),5),.5,p.x);
		p.y= if_milk(equal(int(s),5),.6,p.y);

		//Braccia
		p.x= if_milk(equal(int(s),6),.4-mi*.23,p.x);
		p.y= if_milk(equal(int(s),6),.5+mi,p.y);

		p.x= if_milk(equal(int(s),7),.5,p.x);
		p.y= if_milk(equal(int(s),7),.6,p.y);

		p.x= if_milk(equal(int(s),8),.6+tr*.23,p.x);
		p.y= if_milk(equal(int(s),8),.5+tr,p.y);

		p.x= if_milk(equal(int(s),9),.5,p.x);
		p.y= if_milk(equal(int(s),9),.6,p.y);

		//Testa
		p.x= if_milk(equal(int(s),10),.5,p.x);
		p.y= if_milk(equal(int(s),10),.62,p.y);

		p.x= if_milk(equal(int(s),11),.47-ba*.23,p.x);
		p.y= if_milk(equal(int(s),11),.62,p.y);
		p.x= if_milk(equal(int(s),12),.47-ba*.23,p.x);
		p.y= if_milk(equal(int(s),12),.67+ba*.23,p.y);

		p.x= if_milk(equal(int(s),13),.53+ba*.23,p.x);
		p.y= if_milk(equal(int(s),13),.67+ba*.23,p.y);

		p.x= if_milk(equal(int(s),14),.53+ba*.23,p.x);
		p.y= if_milk(equal(int(s),14),.62,p.y);

		p.x= if_milk(equal(int(s),15),.50,p.x);
		p.y= if_milk(equal(int(s),15),.62,p.y);



		mi_prg= if_milk(above(mi_prg,5),0,mi_prg+mi*.1);
		ba_prg= if_milk(above(ba_prg,5),0,ba_prg+ba*.1);
		tr_prg= if_milk(above(tr_prg,5),0,tr_prg+tr*.1);

		float temp_dim = dim + 0.2 * sin(mi_prg + rand_offset1);
		float temp_xpos = xpos + 0.2 * cos(ba_prg + rand_offset2);
		float temp_ypos = ypos + 0.2* sin(tr_prg + rand_offset3);

		p.x=p.x*temp_dim+temp_xpos;
		p.y=p.y*temp_dim+temp_ypos;

		float hm=context.sample+mi2_prg;
		float ht=context.sample+tr_prg;
		float hb=context.sample+ba_prg;

		p.r=hm;
		p.g=ht;
		p.b=hb;
		p.a=.8;
		return p;
	}
};

class MstressJuppyDancer : public Pipeline
{
public:
	Dancer dancer[10];

	float q1,meanmid,middiff,mi_prg,gam, bass;

	MstressJuppyDancer() : Pipeline()
	{

		for(int x = 0; x< 10; x++)
		drawables.push_back(&dancer[x]);

		textureWrap = false;
		screenDecay = 0.3;


	q1 = 0;
	meanmid = 0;
	middiff = 0;
	mi_prg = 0;
	gam = 0;

	}

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{
		meanmid = 0.01*(meanmid*99+music.mid);
		middiff = (music.mid - meanmid)*15;
		float mi = min(above(middiff,0)*middiff*.005,.11);
		mi_prg = mi_prg+mi;
        gam = std::abs(gam-above(mi_prg,.5));
		mi_prg= if_milk(above(mi_prg,.5),0,mi_prg);
		q1 = gam;
		bass = music.bass;
	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{

		float sx = -below(p.y,0.8)*.001;
        float dx = .01*bass*(1-q1*2)*below(p.y,0.8);
        float rot= .051*below(p.y,0.8);
        float cx=trunc(p.x*16)/16;
        float cy=trunc(p.y*16)/16;

        Transforms::Scale(p,context,sx,1,cx,cy);
        Transforms::Rotate(p,context,rot,cx,cy);
        Transforms::Transform(p,context,dx,0);

		return p;
	}

};



typedef NativePreset<MstressJuppyDancer> MstressJuppyDancerPreset;

extern "C" MstressJuppyDancerPreset * create(const char * url) {
	return new MstressJuppyDancerPreset(std::string(url));
}

extern "C" void destroy(MstressJuppyDancerPreset * preset) {
    delete preset;
}
