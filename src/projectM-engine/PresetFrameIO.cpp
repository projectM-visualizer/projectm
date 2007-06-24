#include "PresetFrameIO.cpp"

PresetInputs(int gx, int gy)
{
  int x, y;

  this->gx =gx; 
  this->gy=gy;

  this->x_mesh=(float **)wipemalloc(gx * sizeof(float *));
  for(x = 0; x < gx; x++)
    {
      this->presetInputs.x_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->y_mesh=(float **)wipemalloc(gx * sizeof(float *));
  for(x = 0; x < gx; x++)
    {
      this->y_mesh[x] = (float *)wipemalloc(gy * sizeof(float));
      
    }
  this->rad_mesh=(float **)wipemalloc(gx * sizeof(float *));
  for(x = 0; x < gx; x++)
    {
      this->rad_mesh[x] = (float *)wipemalloc(gy * sizeof(float));
    }
  this->theta_mesh=(float **)wipemalloc(gx * sizeof(float *));
  for(x = 0; x <gx; x++)
    {
      this->theta_mesh[x] = (float *)wipemalloc(gy * sizeof(float));
    }

 this->origtheta=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->origtheta[x] = (float *)wipemalloc(gy * sizeof(float));
    }
  this->origrad=(float **)wipemalloc(gx * sizeof(float *));
     for(x = 0; x < gx; x++)
    {
      this->origrad[x] = (float *)wipemalloc(gy * sizeof(float));
    }
  this->origx=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->origx[x] = (float *)wipemalloc(gy * sizeof(float));
    }
  this->origy=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->origy[x] = (float *)wipemalloc(gy * sizeof(float));
    }

for (x=0;x<gx;x++)
    {
      for(y=0;y<gy;y++)
	{
	   this->origx[x][y]=x/(float)(gx-1);
	   this->origy[x][y]=-((y/(float)(gy-1))-1);
	   this->origrad[x][y]=hypot((this->origx[x][y]-.5)*2,(this->origy[x][y]-.5)*2) * .7071067;
  	   this->origtheta[x][y]=atan2(((this->origy[x][y]-.5)*2),((this->origx[x][y]-.5)*2));
        }
    }
  
}

PresetOutputs(int gx, int gy)
{
  this->sx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->sx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->sy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->sy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->dx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->dx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->dy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->dy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->cx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->cx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->cy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->cy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->zoom_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->zoom_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->zoomexp_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->zoomexp_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->rot_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    { 
      this->rot_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
}

~presetInputs()
{
 for(x = 0; x < this->gx; x++)
    {
      
     
      free(this->origtheta[x]);
      free(this->origrad[x]);
      free(this->origx[x]);
      free(this->origy[x]);
    
      free(this->x_mesh[x]);
      free(this->mesh[x]);
      free(this->rad_mesh[x]);
      free(this->theta_mesh[x]);
      
    }

  
  free(this->origx);
  free(this->origy);
  free(this->origrad);
  free(this->origtheta);
  
  free(this->x_mesh);
  free(this->y_mesh);
  free(this->rad_mesh);
  free(this->theta_mesh);

  this->origx = NULL;
  this->origy = NULL;
  this->origtheta = NULL;
  this->origrad = NULL;
  
  this->x_mesh = NULL;
  this->y_mesh = NULL;
  this->rad_mesh = NULL;
  this->theta_mesh = NULL;
}

PresetInputs::ResetMesh()
{
  int x,y;
 
  
    for (x=0;x<this->gx;x++)
    {
      for(y=0;y<this->gy;y++)
	{   
          x_mesh[x][y]=this->origx[x][y];
	  y_mesh[x][y]=this->origy[x][y];
	  rad_mesh[x][y]=this->origrad[x][y];
	  theta_mesh[x][y]=this->origtheta[x][y];	  
	}
    }
 
 }