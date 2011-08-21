#include "imageMix.h"

CImageMix::CImageMix()
{
  background = NULL;
  stereoDepth = NULL;
  width = 0;
  height = 0;
}

CImageMix::~CImageMix()
{
  if(background)
    delete[] background;

  if(stereoDepth)
    delete[] stereoDepth;
}

void CImageMix::loadBackgrounds(std::string* ImNames, int ImNum)
{
  if(background)
    delete[] background;

  background = new CImage[ImNum];

  for(int i=0; i<ImNum; i++)
    {
      background[i].SetFileName(ImNames[i]);
      background[i].ReadPPM();

#ifdef VERBOSE__
      std::cout << ImNames[i] << " is loaded" << std::endl;
      std::cout << "width: " << background[i].width
		<< "  height: " << background[i].height << std::endl;
#endif
      
      background[i].FlipPPM();
    }

  width = background[0].width;
  height = background[0].height;
}


// This function must be called after loadBackgrounds();
void CImageMix::loadDepthmaps(std::string* DepthNames, int ImNum)
{
  if(stereoDepth)
    delete[] stereoDepth;

  int ImSize = width*height;

  stereoDepth = new float[ImSize*ImNum];

  for(int i=0; i<ImNum; i++)
    {
      std::ifstream ifm;

      ifm.open(DepthNames[i].c_str(), std::ios::binary);
      ifm.read((char*)&(stereoDepth[i*ImSize]), ImSize*sizeof(float));
      ifm.close();
    }
}

// Draw background to frame buffer, idx specify
// which background to draw
void CImageMix::drawBackgrounds(int idx)
{
  // If no background is stored in memory, do nothing
  if(!background)
    return;
  
  // Draw to frame buffer
  glDisable( GL_TEXTURE_2D );
  glWindowPos2i(0, 0);
  glDrawPixels(background[idx].width, background[idx].height,
	       GL_RGB, GL_UNSIGNED_BYTE, background[idx].ImageData);
  glEnable( GL_TEXTURE_2D );
}


void CImageMix::mixBuffers(unsigned char* renderedDepth,
			   unsigned char* renderedColor,
			   int idx, float zFar, float zNear)
{
  const int colorChannel = 3;
  const int maxDepth = 255;

  unsigned char* mixed = new unsigned char[width*height*colorChannel];
  
  int counter = 0;
  int colorCounter = 0;

  float coefA = (zFar*zNear)/(zFar-zNear);
  float coefB = zFar-zNear;
  float coefC = zFar+zNear;
  // depth = coefA./((coefC/(2*coefB))-depth+0.5);

  int offset = width*height*idx;

  for(int i=0; i<height; i++)
    {
      for(int j=0; j<width; j++)
	{
	  
	  if(renderedDepth[counter]==maxDepth)
	    {
	      mixed[colorCounter] = 
		background[idx].ImageData[colorCounter];
	      mixed[colorCounter+1] = 
		background[idx].ImageData[colorCounter+1];
	      mixed[colorCounter+2] = 
		background[idx].ImageData[colorCounter+2];
	    }
	  else
	    {
	      // render depth buffer to range [0 1]
	      float depth_f = renderedDepth[counter]/maxDepth;
	      // non_linear transformation for depth buffer
	      depth_f = coefA/((coefC/(2*coefB))-depth_f+0.5);

	      float stereo_depth_f = stereoDepth[offset+counter];
	      
	      if(stereo_depth_f <= depth_f)
		{
		  mixed[colorCounter] = 
		    renderedColor[colorCounter];
		  mixed[colorCounter+1] = 
		    renderedColor[colorCounter+1];
		  mixed[colorCounter+2] = 
		    renderedColor[colorCounter+2];
		}
	      else
		{
		  mixed[colorCounter] = 
		    background[idx].ImageData[colorCounter];
		  mixed[colorCounter+1] = 
		    background[idx].ImageData[colorCounter+1];
		  mixed[colorCounter+2] = 
		    background[idx].ImageData[colorCounter+2];
		}
	    }
	  
	  counter++;
	  colorCounter += colorChannel;
	}
    }

  glDisable( GL_TEXTURE_2D );
  glWindowPos2i(0, 0);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, mixed);
  glEnable( GL_TEXTURE_2D );

  delete[] mixed;
}
