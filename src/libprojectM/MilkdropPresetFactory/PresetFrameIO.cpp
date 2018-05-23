#include "PresetFrameIO.hpp"
#include "wipemalloc.h"
#include <math.h>
#include <cassert>
#include <iostream>
#include <cmath>
#include "Renderer/BeatDetect.hpp"
#include <immintrin.h>


PresetInputs::PresetInputs() : PipelineContext()
{
}


void PresetInputs::update(const BeatDetect & music, const PipelineContext & context) {

    // Reflect new values form the beat detection unit
    this->bass = music.bass;
    this->mid = music.mid;
    this->treb = music.treb;
    this->bass_att = music.bass_att;
    this->mid_att = music.mid_att;
    this->treb_att = music.treb_att;

    // Reflect new values from the pipeline context
    this->fps = context.fps;
    this->time = context.time;

    this->frame = context.frame;
    this->progress = context.progress;
}


float **alloc_mesh(size_t gx, size_t gy)
{
	// round gy up to multiple 4 (for possible SSE optimization) 
	gy = (gy+3) & ~(size_t)3;

	float **mesh = (float **)wipe_aligned_alloc(gx * sizeof(float *));
	float *m = (float *)wipe_aligned_alloc(gx * gy * sizeof(float));
	for ( int x = 0; x < gx; x++ )
		mesh[x] = m + (gy * x);
	return mesh;
}

float **free_mesh(float **mesh)
{
	wipe_aligned_free(mesh[0]);
	wipe_aligned_free(mesh);
	return NULL;
}

void copy_mesh(float **dst, float **src, int gx, int gy)
{
	memcpy(dst[0], src[0], gx*gy*sizeof(float));
}


void PresetInputs::Initialize ( int gx, int gy )
{
	int x, y;

	this->gx = gx;
	this->gy = gy;


	/// @bug no clue if this block belongs here
	// ***
	progress = 0;
	frame = 1;

	x_per_pixel = 0;
	y_per_pixel = 0;
	rad_per_pixel = 0;
	ang_per_pixel = 0;
	// ***

	this->x_mesh    = alloc_mesh(gx, gy);
	this->y_mesh    = alloc_mesh(gx, gy);
	this->rad_mesh  = alloc_mesh(gx, gy);
	this->theta_mesh= alloc_mesh(gx, gy);
	this->origtheta = alloc_mesh(gx, gy);
	this->origrad   = alloc_mesh(gx, gy);
	this->origx     = alloc_mesh(gx, gy);
	this->origy     = alloc_mesh(gx, gy);

	for ( x=0;x<gx;x++ )
	{
		for ( y=0;y<gy;y++ )
		{
			this->origx[x][y]=x/ ( float ) ( gx-1 );
			this->origy[x][y]=- ( ( y/ ( float ) ( gy-1 ) )-1 );
			this->origrad[x][y]=hypot ( ( this->origx[x][y]-.5 ) *2, ( this->origy[x][y]-.5 ) *2 ) * .7071067;
			this->origtheta[x][y]=atan2 ( ( ( this->origy[x][y]-.5 ) *2 ), ( ( this->origx[x][y]-.5 ) *2 ) );
		}
	}
}


PresetOutputs::PresetOutputs() : Pipeline()
{}


PresetOutputs::~PresetOutputs()
{
	assert(this->gx > 0);

	this->rad_mesh = free_mesh(this->rad_mesh);
	this->sx_mesh  = free_mesh(this->sx_mesh);
	this->sy_mesh  = free_mesh(this->sy_mesh);
	this->dy_mesh  = free_mesh(this->dy_mesh);
	this->dx_mesh  = free_mesh(this->dx_mesh);
	this->cy_mesh  = free_mesh(this->cy_mesh);
	this->cx_mesh  = free_mesh(this->cx_mesh);
	this->warp_mesh = free_mesh(this->warp_mesh);
	this->zoom_mesh = free_mesh(this->zoom_mesh);
	this->zoomexp_mesh = free_mesh(this->zoomexp_mesh);
	this->rot_mesh = free_mesh(this->rot_mesh);
	this->orig_x   = free_mesh(this->orig_x);
	this->orig_y   = free_mesh(this->orig_y);
}


void PresetOutputs::Render(const BeatDetect &music, const PipelineContext &context)
{
	PerPixelMath(context);

	drawables.clear();

	drawables.push_back(&mv);

	for (PresetOutputs::cshape_container::iterator pos = customShapes.begin();
			pos != customShapes.end(); ++pos)
	{
		if ((*pos)->enabled==1)
			drawables.push_back((*pos));
	}

	for (PresetOutputs::cwave_container::iterator pos = customWaves.begin();
			pos != customWaves.end(); ++pos)
	{
		if ((*pos)->enabled==1)
			drawables.push_back((*pos));
	}

	drawables.push_back(&wave);

	if (bDarkenCenter==1)
		drawables.push_back(&darkenCenter);
	drawables.push_back(&border);

	compositeDrawables.clear();
	compositeDrawables.push_back(&videoEcho);

	if (bBrighten==1)
		compositeDrawables.push_back(&brighten);

	if (bDarken==1)
		compositeDrawables.push_back(&darken);

	if (bSolarize==1)
		compositeDrawables.push_back(&solarize);

	if (bInvert==1)
		compositeDrawables.push_back(&invert);
}


// N.B. The more optimization that can be done on this method, the better! This is called a lot and can probably be improved.
void PresetOutputs::PerPixelMath_c(const PipelineContext &context)
{
	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y++)
		{
			const float fZoom2 = std::pow(this->zoom_mesh[x][y], std::pow(this->zoomexp_mesh[x][y],
					rad_mesh[x][y] * 2.0f - 1.0f));
			const float fZoom2Inv = 1.0f / fZoom2;
			this->x_mesh[x][y] = this->orig_x[x][y] * 0.5f * fZoom2Inv + 0.5f;
			this->x_mesh[x][y] = (this->x_mesh[x][y] - this->cx_mesh[x][y]) / this->sx_mesh[x][y] + this->cx_mesh[x][y];
			this->y_mesh[x][y] = this->orig_y[x][y] * 0.5f * fZoom2Inv + 0.5f;
			this->y_mesh[x][y] = (this->y_mesh[x][y] - this->cy_mesh[x][y]) / this->sy_mesh[x][y] + this->cy_mesh[x][y];
		}
	}

	const float fWarpTime = context.time * this->fWarpAnimSpeed;
	const float fWarpScaleInv = 1.0f / this->fWarpScale;
	float f[4];
	f[0] = 11.68f + 4.0f * cosf(fWarpTime * 1.413f + 10);
	f[1] = 8.77f + 3.0f * cosf(fWarpTime * 1.113f + 7);
	f[2] = 10.54f + 3.0f * cosf(fWarpTime * 1.233f + 3);
	f[3] = 11.49f + 4.0f * cosf(fWarpTime * 0.933f + 5);

	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y++)
		{
			const float orig_x = this->orig_x[x][y];
			const float orig_y = this->orig_y[x][y];
			const float warp_mesh = this->warp_mesh[x][y] * 0.0035f;

			this->x_mesh[x][y] +=
				(warp_mesh * sinf(fWarpTime * 0.333f + fWarpScaleInv * (orig_x * f[0] - orig_y * f[3]))) +
				(warp_mesh * cosf(fWarpTime * 0.753f - fWarpScaleInv * (orig_x * f[1] - orig_y * f[2])));

			this->y_mesh[x][y] +=
				(warp_mesh * cosf(fWarpTime * 0.375f - fWarpScaleInv * (orig_x * f[2] + orig_y * f[1]))) +
				(warp_mesh * sinf(fWarpTime * 0.825f + fWarpScaleInv * (orig_x * f[0] + orig_y * f[3])));
		}
	}

	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y++)
		{
			const float u2 = this->x_mesh[x][y] - this->cx_mesh[x][y];
			const float v2 = this->y_mesh[x][y] - this->cy_mesh[x][y];

			const float rot = this->rot_mesh[x][y];
			const float cos_rot = cosf(rot);
			const float sin_rot = sinf(rot);

			this->x_mesh[x][y] = u2 * cos_rot - v2 * sin_rot + this->cx_mesh[x][y] - this->dx_mesh[x][y];
			this->y_mesh[x][y] = u2 * sin_rot + v2 * cos_rot + this->cy_mesh[x][y] - this->dy_mesh[x][y];
		}
	}
}


#ifdef __SSE2__

// is there an SSE way to do this?
inline __m128 _mm_pow(__m128 x, __m128 y)
{
	float X[4];
	float Y[4];
	_mm_store_ps(X,x);
	_mm_store_ps(Y,x);
	X[0] = __builtin_powf(X[0],Y[0]);
	X[1] = __builtin_powf(X[1],Y[1]);
	X[2] = __builtin_powf(X[2],Y[2]);
	X[3] = __builtin_powf(X[3],Y[3]);
	return _mm_load_ps(X);
}
inline void _mm_sincosf(__m128 x, __m128 &sinx, __m128 &cosx)
{
	float X[4], S[4], C[4];
	_mm_store_ps(X,x);
	S[0] = sinf(X[0]);
	C[0] = cosf(X[0]);
	S[1] = sinf(X[1]);
	C[1] = cosf(X[1]);
	S[2] = sinf(X[2]);
	C[2] = cosf(X[2]);
	S[3] = sinf(X[3]);
	C[3] = cosf(X[3]);
	sinx = _mm_load_ps(S);
	cosx = _mm_load_ps(C);
}
inline __m128 _mm_sinf(__m128 x)
{
	float X[4];
	_mm_store_ps(X,x);
	X[0] = sinf(X[0]);
	X[1] = sinf(X[1]);
	X[2] = sinf(X[2]);
	X[3] = sinf(X[3]);
	return _mm_load_ps(X);
}
inline __m128 _mm_cosf(__m128 x)
{
	float X[4];
	_mm_store_ps(X,x);
	X[0] = cosf(X[0]);
	X[1] = cosf(X[1]);
	X[2] = cosf(X[2]);
	X[3] = cosf(X[3]);
	return _mm_load_ps(X);
}


void PresetOutputs::PerPixelMath_sse(const PipelineContext &context)
{
	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y += 4)
		{
			// fZoom2 = std::pow(this->zoom_mesh[x][y], std::pow(this->zoomexp_mesh[x][y],
			// 		rad_mesh[x][y] * 2.0f - 1.0f));
			__m128 rad_mesh_scaled = 
				_mm_sub_ps(
					_mm_mul_ps(
						_mm_load_ps(&this->rad_mesh[x][y]), 
						_mm_set_ps1(2.0f)), 
					_mm_set_ps1(1.0f));
			__m128 zoom_mesh = _mm_load_ps(&this->zoom_mesh[x][y]);
			__m128 zoomexp_mesh = _mm_load_ps(&this->zoomexp_mesh[x][y]);
			__m128 fZoom2 = _mm_pow(zoom_mesh, _mm_pow(zoomexp_mesh, rad_mesh_scaled));
			// fZoom2Inv = 1.0f / fZoom2;
			__m128 fZoomInv = _mm_rcp_ps(fZoom2);

			// this->x_mesh[x][y] = this->orig_x[x][y] * 0.5f * fZoom2Inv + 0.5f;
			__m128 x_mesh = 
				_mm_add_ps(
					_mm_mul_ps(
						_mm_load_ps(&this->orig_x[x][y]), 
						_mm_mul_ps(fZoomInv,_mm_set_ps1(0.5f))),		// CONSIDER: common sub-expression
					_mm_set_ps1(0.5f));
			// this->x_mesh[x][y] = (this->x_mesh[x][y] - this->cx_mesh[x][y]) / this->sx_mesh[x][y] + this->cx_mesh[x][y];
			__m128 cx_mesh = _mm_load_ps(&this->cx_mesh[x][y]);
			__m128 sx_mesh = _mm_load_ps(&this->sx_mesh[x][y]);
			_mm_store_ps(&this->x_mesh[x][y],
				_mm_add_ps(
					_mm_div_ps(
						_mm_sub_ps(x_mesh,cx_mesh),
						sx_mesh),
					cx_mesh
				));

			// this->y_mesh[x][y] = this->orig_y[x][y] * 0.5f * fZoom2Inv + 0.5f;
			__m128 y_mesh =  
				_mm_add_ps(
					_mm_mul_ps(
						_mm_load_ps(&this->orig_y[x][y]), 
						_mm_mul_ps(fZoomInv,_mm_set_ps1(0.5f))),
					_mm_set_ps1(0.5f));
			// this->y_mesh[x][y] = (this->y_mesh[x][y] - this->cy_mesh[x][y]) / this->sy_mesh[x][y] + this->cy_mesh[x][y];
			__m128 cy_mesh = _mm_load_ps(&this->cy_mesh[x][y]);
			__m128 sy_mesh = _mm_load_ps(&this->sy_mesh[x][y]);
			_mm_store_ps(&this->y_mesh[x][y],
				_mm_add_ps(
					_mm_div_ps(
						_mm_sub_ps(y_mesh,cy_mesh),
						sy_mesh),
					cy_mesh
				));
		}
	}

	const float fWarpTime = context.time * this->fWarpAnimSpeed;
	const float fWarpScaleInv = 1.0f / this->fWarpScale;
	const float f[4] = 
	{
		11.68f + 4.0f * cosf(fWarpTime * 1.413f + 10),
		 8.77f + 3.0f * cosf(fWarpTime * 1.113f + 7),
		10.54f + 3.0f * cosf(fWarpTime * 1.233f + 3),
		11.49f + 4.0f * cosf(fWarpTime * 0.933f + 5)
	};

	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y+=4)
		{
			//float orig_x = this->orig_x[x][y];
			//float orig_y = this->orig_y[x][y];
			//float warp_mesh = this->warp_mesh[x][y] * 0.0035f;
			const __m128 orig_x = _mm_load_ps(&this->orig_x[x][y]);
			const __m128 orig_y = _mm_load_ps(&this->orig_y[x][y]);
			const __m128 warp_mesh = _mm_mul_ps(_mm_load_ps(&this->warp_mesh[x][y]), _mm_set_ps1(0.0035f));

			// this->x_mesh[x][y] += 
			// 	(warp_mesh * sinf(fWarpTime * 0.333f + fWarpScaleInv * (orig_x * f[0] - orig_y * f[3]))) +
			// 	(warp_mesh * cosf(fWarpTime * 0.753f - fWarpScaleInv * (orig_x * f[1] - orig_y * f[2])));
			_mm_store_ps(&this->x_mesh[x][y],
				_mm_add_ps(_mm_load_ps(&this->x_mesh[x][y]),
					_mm_add_ps(
						_mm_mul_ps(warp_mesh, _mm_sinf(
							_mm_add_ps(
								_mm_set_ps1(fWarpTime*0.333f),
								_mm_mul_ps(_mm_set_ps1(fWarpScaleInv),
									_mm_sub_ps(
										_mm_mul_ps(orig_x, _mm_set_ps1(f[0])),
										_mm_mul_ps(orig_y, _mm_set_ps1(f[3]))
									))))),
						_mm_mul_ps(warp_mesh, _mm_cosf(
							_mm_sub_ps(
								_mm_set_ps1(fWarpTime*0.753f),
								_mm_mul_ps(_mm_set_ps1(fWarpScaleInv),
									_mm_sub_ps(
										_mm_mul_ps(orig_x, _mm_set_ps1(f[1])),
										_mm_mul_ps(orig_y, _mm_set_ps1(f[2]))
									))))))));

			// this->y_mesh[x][y] += 
			// 	(warp_mesh * cosf(fWarpTime * 0.375f - fWarpScaleInv * (orig_x * f[2] + orig_y * f[1]))) +
			// 	(warp_mesh * sinf(fWarpTime * 0.825f + fWarpScaleInv * (orig_x * f[0] + orig_y * f[3])));
			_mm_store_ps(&this->y_mesh[x][y],
				_mm_add_ps(_mm_load_ps(&this->y_mesh[x][y]),
					_mm_add_ps(
						_mm_mul_ps(warp_mesh, _mm_cosf(
							_mm_sub_ps(
								_mm_set_ps1(fWarpTime*0.375f),
								_mm_mul_ps(_mm_set_ps1(fWarpScaleInv),
									_mm_add_ps(
										_mm_mul_ps(orig_x, _mm_set_ps1(f[2])),
										_mm_mul_ps(orig_y, _mm_set_ps1(f[1]))
									))))),
						_mm_mul_ps(warp_mesh, _mm_sinf(
							_mm_add_ps(
								_mm_set_ps1(fWarpTime*0.825f),
								_mm_mul_ps(_mm_set_ps1(fWarpScaleInv),
									_mm_add_ps(
										_mm_mul_ps(orig_x, _mm_set_ps1(f[0])),
										_mm_mul_ps(orig_y, _mm_set_ps1(f[3]))
									))))))));
		}
	}
	for (int x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y+=4)
		{
			// const float u2 = this->x_mesh[x][y] - this->cx_mesh[x][y];
			// const float v2 = this->y_mesh[x][y] - this->cy_mesh[x][y];
			const __m128 u2 = _mm_sub_ps(_mm_load_ps(&this->x_mesh[x][y]),_mm_load_ps(&this->cx_mesh[x][y]));
			const __m128 v2 = _mm_sub_ps(_mm_load_ps(&this->y_mesh[x][y]),_mm_load_ps(&this->cy_mesh[x][y]));

			// const float rot = this->rot_mesh[x][y];
			// const float cos_rot = cosf(rot);
			// const float sin_rot = sinf(rot);
			__m128 sin_rot, cos_rot;
			_mm_sincosf(_mm_load_ps(&this->rot_mesh[x][y]), sin_rot, cos_rot);

			// this->x_mesh[x][y] = u2 * cos_rot - v2 * sin_rot + this->cx_mesh[x][y] - this->dx_mesh[x][y];
			_mm_store_ps(&this->x_mesh[x][y],
				_mm_add_ps(
					_mm_sub_ps(_mm_mul_ps(u2, cos_rot), _mm_mul_ps(v2,sin_rot)),
					_mm_sub_ps(_mm_load_ps(&this->cx_mesh[x][y]), _mm_load_ps(&this->dx_mesh[x][y]))
					));
			// this->y_mesh[x][y] = u2 * sin_rot + v2 * cos_rot + this->cy_mesh[x][y] - this->dy_mesh[x][y];
			_mm_store_ps(&this->y_mesh[x][y],
				_mm_add_ps(
					_mm_add_ps(_mm_mul_ps(u2, sin_rot), _mm_mul_ps(v2,cos_rot)),
					_mm_sub_ps(_mm_load_ps(&this->cy_mesh[x][y]), _mm_load_ps(&this->dy_mesh[x][y]))
					));
		}
	}
}
#endif


void PresetOutputs::PerPixelMath(const PipelineContext &context)
{
#ifdef __SSE2__
	PerPixelMath_sse(context);
#else
	PerPixelMath_c(context);
#endif
}


void PresetOutputs::Initialize ( int gx, int gy )
{
	assert(gx > 0);

	this->gx = gx;
	this->gy = gy;

	staticPerPixel = true;
	setStaticPerPixel(gx,gy);

	assert(this->gx > 0);
	int x;
	this->x_mesh  = alloc_mesh( gx, gy );
	this->y_mesh  = alloc_mesh( gx, gy );
	this->sx_mesh = alloc_mesh( gx, gy );
	this->sy_mesh = alloc_mesh( gx, gy );
	this->dx_mesh = alloc_mesh( gx, gy );
	this->dy_mesh = alloc_mesh( gx, gy );
	this->cx_mesh = alloc_mesh( gx, gy );
	this->cy_mesh = alloc_mesh( gx, gy );
	this->zoom_mesh = alloc_mesh( gx, gy );
	this->zoomexp_mesh = alloc_mesh( gx, gy );
	this->rot_mesh = alloc_mesh( gx, gy );

	this->warp_mesh = alloc_mesh( gx, gy );
	this->rad_mesh = alloc_mesh( gx, gy );
	this->orig_x  = alloc_mesh( gx, gy );
	this->orig_y  = alloc_mesh( gx, gy );

	//initialize reference grid values
	for (x = 0; x < gx; x++)
	{
		for (int y = 0; y < gy; y++)
		{
			float origx = x / (float) (gx - 1);
			float origy = -((y / (float) (gy - 1)) - 1);

			rad_mesh[x][y]=hypot ( ( origx-.5 ) *2, ( origy-.5 ) *2 ) * .7071067;
			orig_x[x][y] = (origx - .5) * 2;
			orig_y[x][y] = (origy - .5) * 2;
		}
	}
}


PresetInputs::~PresetInputs()
{
	this->origx = free_mesh ( this->origx );
	this->origy = free_mesh ( this->origy );
	this->origrad = free_mesh ( this->origrad );
	this->origtheta = free_mesh ( this->origtheta );
	this->x_mesh = free_mesh ( this->x_mesh );
	this->y_mesh = free_mesh ( this->y_mesh );
	this->rad_mesh = free_mesh ( this->rad_mesh );
	this->theta_mesh = free_mesh ( this->theta_mesh );
}


void PresetInputs::resetMesh()
{
	assert ( x_mesh );
	assert ( y_mesh );
	assert ( rad_mesh );
	assert ( theta_mesh );

	copy_mesh(this->x_mesh, this->origx, gx, gy);
	copy_mesh(this->y_mesh, this->origy, gx, gy);
	copy_mesh(this->rad_mesh, this->origrad, gx, gy);
	copy_mesh(this->theta_mesh, this->origtheta, gx, gy);
}


#ifdef USE_MERGE_PRESET_CODE
void PresetMerger::MergePresets(PresetOutputs & A, PresetOutputs & B, double ratio, int gx, int gy)
{

double invratio = 1.0 - ratio;
  //Merge Simple Waveforms
  //
  // All the mess is because of Waveform 7, which is two lines.
  //


  //Merge Custom Shapes and Custom Waves

  for (PresetOutputs::cshape_container::iterator pos = A.customShapes.begin();
	pos != A.customShapes.end(); ++pos)
    {
       (*pos)->a *= invratio;
       (*pos)->a2 *= invratio;
       (*pos)->border_a *= invratio;
    }

  for (PresetOutputs::cshape_container::iterator pos = B.customShapes.begin();
	pos != B.customShapes.end(); ++pos)
    {
       (*pos)->a *= ratio;
       (*pos)->a2 *= ratio;
       (*pos)->border_a *= ratio;

        A.customShapes.push_back(*pos);

    }
 for (PresetOutputs::cwave_container::iterator pos = A.customWaves.begin();
	pos != A.customWaves.end(); ++pos)
    {
       (*pos)->a *= invratio;
      for (int x=0; x <   (*pos)->samples; x++)
	{
	   (*pos)->a_mesh[x]= (*pos)->a_mesh[x]*invratio;
	}
    }

  for (PresetOutputs::cwave_container::iterator pos = B.customWaves.begin();
	pos != B.customWaves.end(); ++pos)
    {
       (*pos)->a *= ratio;
      for (int x=0; x < (*pos)->samples; x++)
	{
	   (*pos)->a_mesh[x]= (*pos)->a_mesh[x]*ratio;
	}
       A.customWaves.push_back(*pos);
    }


  //Interpolate Per-Pixel mesh

  for (int x=0;x<gx;x++)
    {
      for(int y=0;y<gy;y++)
	{
	  A.x_mesh[x][y]  = A.x_mesh[x][y]* invratio + B.x_mesh[x][y]*ratio;
	}
    }
 for (int x=0;x<gx;x++)
    {
      for(int y=0;y<gy;y++)
	{
	  A.y_mesh[x][y]  = A.y_mesh[x][y]* invratio + B.y_mesh[x][y]*ratio;
	}
    }



 //Interpolate PerFrame floats

  A.screenDecay = A.screenDecay * invratio + B.screenDecay * ratio;

  A.wave.r = A.wave.r* invratio + B.wave.r*ratio;
  A.wave.g = A.wave.g* invratio + B.wave.g*ratio;
  A.wave.b = A.wave.b* invratio + B.wave.b*ratio;
  A.wave.a = A.wave.a* invratio + B.wave.a*ratio;
  A.wave.x = A.wave.x* invratio + B.wave.x*ratio;
  A.wave.y = A.wave.y* invratio + B.wave.y*ratio;
  A.wave.mystery = A.wave.mystery* invratio + B.wave.mystery*ratio;

  A.border.outer_size = A.border.outer_size* invratio + B.border.outer_size*ratio;
  A.border.outer_r = A.border.outer_r* invratio + B.border.outer_r*ratio;
  A.border.outer_g = A.border.outer_g* invratio + B.border.outer_g*ratio;
  A.border.outer_b = A.border.outer_b* invratio + B.border.outer_b*ratio;
  A.border.outer_a = A.border.outer_a* invratio + B.border.outer_a*ratio;

  A.border.inner_size = A.border.inner_size* invratio + B.border.inner_size*ratio;
  A.border.inner_r = A.border.inner_r* invratio + B.border.inner_r*ratio;
  A.border.inner_g = A.border.inner_g* invratio + B.border.inner_g*ratio;
  A.border.inner_b = A.border.inner_b* invratio + B.border.inner_b*ratio;
  A.border.inner_a = A.border.inner_a* invratio + B.border.inner_a*ratio;

  A.mv.a  = A.mv.a* invratio + B.mv.a*ratio;
  A.mv.r  = A.mv.r* invratio + B.mv.r*ratio;
  A.mv.g  = A.mv.g* invratio + B.mv.g*ratio;
  A.mv.b  = A.mv.b* invratio + B.mv.b*ratio;
  A.mv.length = A.mv.length* invratio + B.mv.length*ratio;
  A.mv.x_num = A.mv.x_num* invratio + B.mv.x_num*ratio;
  A.mv.y_num = A.mv.y_num* invratio + B.mv.y_num*ratio;
  A.mv.y_offset = A.mv.y_offset* invratio + B.mv.y_offset*ratio;
  A.mv.x_offset = A.mv.x_offset* invratio + B.mv.x_offset*ratio;


  A.fRating = A.fRating* invratio + B.fRating*ratio;
  A.fGammaAdj = A.fGammaAdj* invratio + B.fGammaAdj*ratio;
  A.videoEcho.zoom = A.videoEcho.zoom* invratio + B.videoEcho.zoom*ratio;
  A.videoEcho.a = A.videoEcho.a* invratio + B.videoEcho.a*ratio;


  A.fWarpAnimSpeed = A.fWarpAnimSpeed* invratio + B.fWarpAnimSpeed*ratio;
  A.fWarpScale = A.fWarpScale* invratio + B.fWarpScale*ratio;
  A.fShader = A.fShader* invratio + B.fShader*ratio;

  //Switch bools and discrete values halfway.  Maybe we should do some interesting stuff here.

  if (ratio > 0.5)
    {
      A.videoEcho.orientation = B.videoEcho.orientation;
      A.textureWrap = B.textureWrap;
      A.bDarkenCenter = B.bDarkenCenter;
      A.bRedBlueStereo = B.bRedBlueStereo;
      A.bBrighten = B.bBrighten;
      A.bDarken = B.bDarken;
      A.bSolarize = B.bSolarize;
      A.bInvert = B.bInvert;
      A.bMotionVectorsOn = B.bMotionVectorsOn;
    }

  return;
}
#endif
