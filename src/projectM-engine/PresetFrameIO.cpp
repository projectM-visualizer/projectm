#include "PresetFrameIO.cpp"

PresetInputs(int gx, int gy)
{
  int x, y;
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
