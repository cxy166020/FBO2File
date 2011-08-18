#ifndef IMG_MIX__
#define IMG_MIX__

#include <iostream>
#include <string>

#include "CImage.h"

class CImageMix
{
 public:
  CImageMix();
  ~CImageMix();

  void loadBackgrounds(std::string* ImNames, int ImNum);
  void drawBackgrounds(int idx);

private:
  CImage* background;

};

#endif
