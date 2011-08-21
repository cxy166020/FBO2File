#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#ifndef IMG_MIX__
#define IMG_MIX__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include <iostream>
#include <string>
#include <fstream>

#include "CImage.h"

class CImageMix
{
 public:
  CImageMix();
  ~CImageMix();

  void loadBackgrounds(std::string* ImNames, int ImNum);
  void loadDepthmaps(std::string* DepthNames, int ImNum);
  void drawBackgrounds(int idx);
  void mixBuffers(unsigned char* renderedDepth,
		  unsigned char* renderedColor,
		  int idx, float zNear, float zFar);

private:
  CImage* background;
  float* stereoDepth;
  int width, height;
};

#endif
