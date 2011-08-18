#include "CImage.h"

CImage::CImage(string ImageName)
{
  width = 0;
  height = 0;
  ImageData = NULL;
  FileName = ImageName;
  DataSize = 0;
}


CImage::CImage()
{
  width = 0;
  height = 0;
  ImageData = NULL;
  FileName = "";
  DataSize = 0;
}


CImage::~CImage()
{
  width  = 0;
  height = 0;
	
  FileName = "";
  DataSize = 0;
	
  if(ImageData) 
    delete[] ImageData; 
	
  ImageData = NULL;
}


void CImage::SetFileName(string ImageName)
{
  FileName = ImageName;
}


string CImage::GetLine(ifstream& ifm)
{
  string line="";
  char uc=NULL;

  while(true)
    {
      if(ifm.eof()||line.size()>=line.max_size())
	{
	  break;
	}
      ifm.get(uc);
      if(uc!=BL&&uc!=TB&&uc!=CR&&uc!=LF&&uc!=PD)
	{
	  line+=uc;
	}
      else
	{
	  if(uc==PD)
	    {
	      while(true)
		{	
		  if(ifm.eof())
		    {
		      break;
		    }
		  ifm.get(uc);
		  if(uc==TB||uc==CR||uc==LF||uc==PD)
		    {
		      break;
		    }
		}
	    }
	  break;
	}
    }
  return line;
}


string CImage::GetLineEx(ifstream& ifm)
{
  string line="";
	
  line = GetLine(ifm);
  while(line.length()==0)
    {
      if(ifm.eof())
	{
	  break;
	}
      line = GetLine(ifm);
    }
  return line;
}


void CImage::ReadPPM()
{
  ifstream ifm;
  string MagicNumber;
  const string ppmNumber = "P6";
  int MaxValue;

  ifm.open(FileName.c_str(), ios::binary);
  if(!ifm.good())
    {
      cerr<<"Error opening the image"<<endl;
      exit(1);
    }

  //Check if the file type is supported by this program
  MagicNumber = GetLineEx(ifm);
  if(MagicNumber!=ppmNumber)
    {
      cerr<<"This is not a PPM picture"<<endl;
      exit(1);
    }

  //Read the size of input image
  width = atoi(GetLineEx(ifm).c_str());
  height = atoi(GetLineEx(ifm).c_str());
  MaxValue = atoi(GetLineEx(ifm).c_str());

  //Check if the gray level is supported
  if(MaxValue>255)
    {
      cerr<<"This program does not support images which have more than 255 gray levels!"<<endl;
    }

  DataSize = width*height*DIMENSION;
  ImageData = new unsigned char[DataSize];

  //This section fills the 2D array with the input values
  ifm.read((char*)ImageData, DataSize);
  ifm.close();
}
