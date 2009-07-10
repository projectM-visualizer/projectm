#include "PresetMerge.hpp"
#include "RenderItemMatcher.hpp"
#include "RenderItemMergeFunction.hpp"

void PipelineMerger::MergePipelines(const Pipeline & a, const Pipeline & b, Pipeline & out, RenderItemMatcher::MatchResults & results, RenderItemMergeFunction & mergeFunction, float ratio)
{

	double invratio = 1.0 - ratio;

	out.textureWrap = (ratio < 0.5) ? a.textureWrap : b.textureWrap;

	out.screenDecay = lerp( b.screenDecay, a.screenDecay, ratio);
	out.drawables.clear();
	

	for (RenderItemMatchList::iterator pos = results.matches.begin(); pos != results.matches.end(); ++pos) {		
				
		RenderItem * itemA = pos->first;
		RenderItem * itemB = pos->second;

		RenderItem * itemC = mergeFunction(itemA, itemB, ratio);

		if (itemC == 0) {
			itemA->masterAlpha = ratio;
	        	out.drawables.push_back(itemA);
			itemB->masterAlpha = invratio;
	        	out.drawables.push_back(itemB);
		} else 
			out.drawables.push_back(itemC);
		
	}

	for (std::vector<RenderItem*>::const_iterator pos = results.unmatchedLeft.begin();
		pos != results.unmatchedLeft.end(); ++pos)
	    {
	       (*pos)->masterAlpha = invratio;
	       out.drawables.push_back(*pos);
	    }

	for (std::vector<RenderItem*>::const_iterator pos = results.unmatchedRight.begin();
		pos != results.unmatchedRight.end(); ++pos)
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
