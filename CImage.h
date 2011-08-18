// Tested on 1:03, Aug 21 2009
// CImage.h and CImage.cpp work normally

#ifndef _CIMAGE
#define _CIMAGE

#define BL 0x20
#define TB 0x09
#define CR 0x0D
#define LF 0x0A
#define PD 0x23

#define DIMENSION 3

#include <fstream>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

class CImage
{
 public:
  CImage();
  CImage(string ImageName);
  virtual ~CImage();

 private:
  string GetLine(ifstream& ifm);
  string GetLineEx(ifstream& ifm);

 public:
  void   ReadPPM();
  void   SetFileName(string ImageName);

 public:
  unsigned char* ImageData;
  int width;
  int height;

 private:
  string FileName;
  int DataSize;
};

#endif
