#include "imageMix.h"

CImageMix::CImageMix()
{
  background = NULL;
}

CImageMix::~CImageMix()
{
  if(background)
    delete[] background;
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

      std::cout << ImNames[i] << " is loaded" << std::endl;
      std::cout << "width: " << background[i].width
		<< "  height: " << background[i].height << std::endl;

      background[i].FlipPPM();
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
