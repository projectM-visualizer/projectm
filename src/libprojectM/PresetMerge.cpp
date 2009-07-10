#include "PresetMerge.hpp"
#include "RenderItemMatcher.hpp"
#include "RenderItemMergeFunction.hpp"

void PipelineMerger::MergePipelines(const Pipeline & a, const Pipeline & b, Pipeline & out, const RenderItemMatchList & matching, RenderItemMergeFunction & mergeFunction, float ratio)
{

	double invratio = 1.0 - ratio;

	out.textureWrap = (ratio < 0.5) ? a.textureWrap : b.textureWrap;

	out.screenDecay = lerp( b.screenDecay, a.screenDecay, ratio);
	out.drawables.clear();
	
	for (RenderItemMatchList::const_iterator pos = matching.begin(); pos != matching.end(); ++pos) {		
		

		const RenderItem * itemA = pos->first;
		const RenderItem * itemB = pos->second;
				
		//mergeFunction(itemA, itemB, out, ratio);
	}

	for (std::vector<RenderItem*>::const_iterator pos = a.drawables.begin();
		pos != a.drawables.end(); ++pos)
	    {
	       (*pos)->masterAlpha = invratio;
	       out.drawables.push_back(*pos);
	    }

	for (std::vector<RenderItem*>::const_iterator pos = b.drawables.begin();
		pos != b.drawables.end(); ++pos)
		{
		  (*pos)->masterAlpha = ratio;
		   out.drawables.push_back(*pos);
		}

	if (a.staticPerPixel && b.staticPerPixel)
	{
		out.staticPerPixel = true;
		 for (int x=0;x<a.gx;x++)
		    {
		      for(int y=0;y<a.gy;y++)
			{
			  out.x_mesh[x][y]  = a.x_mesh[x][y]* invratio + b.x_mesh[x][y]*ratio;
			}
		    }
		 for (int x=0;x<a.gx;x++)
		    {
		      for(int y=0;y<a.gy;y++)
			{
			  out.y_mesh[x][y]  = a.y_mesh[x][y]* invratio + b.y_mesh[x][y]*ratio;
			}
		    }
	}

	out.compositeShader = a.compositeShader;
	out.warpShader = a.warpShader;
}
