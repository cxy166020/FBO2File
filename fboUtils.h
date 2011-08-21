#ifndef FBO_UTILS_
#define FBO_UTILS

// in order to get function prototypes from glext.h, define
// GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif


#include "glInfo.h"

void initFBO(int ImWidth, int ImHeight, GLuint& colorTextureId, GLuint& depthTextureId, GLuint& fboId);

// void showInfo();
bool checkFramebufferStatus();
void printFramebufferInfo();
std::string getTextureParameters(GLuint id);
std::string getRenderbufferParameters(GLuint id);
std::string convertInternalFormatToString(GLenum format);

// Template to allocate a 3 dimensional array
template <typename T> T*** Allocate3DArray(int row, int col, int dim, T*& buffer)
{
  int i, j;

  buffer = new T[row*col*dim];
  memset(buffer, 0, sizeof(T)*row*col*dim);

  T*** arr = new T**[dim];

  for(i=0; i<dim; i++)
    arr[i] = new T*[row];

  for(i=0; i<dim; i++)
    for(j=0; j<row; j++)
      arr[i][j] = buffer+i*row*col+j*col;

  return arr;
}


// Template to release the 3 dimensional array allocated by Allocate3DArray()
template <typename T> void Delete3DArray(T*** arr, T* buffer, int dim)
{
  delete[] buffer;

  for(int i=0; i<dim; i++)
    delete[] arr[i];

  delete[] arr;
}


// Template to allocate a 2 dimensional array
template <typename T> T** Allocate2DArray(int row, int col, T*& buffer)
{
  int i;

  buffer = new T[row*col];
  memset(buffer, 0, sizeof(T)*row*col);

  T** arr = new T*[row];

  for(i=0; i<row; i++)
    arr[i] = buffer+i*col;

  return arr;
}


//// Template to release the 2 dimensional array allocated by Allocate2DArray()
template <typename T> void Delete2DArray(T**& arr, T*& buffer)
{
  if(buffer!=NULL)
    {
      delete[] buffer;
      buffer = NULL;
    }

  if(arr!=NULL)
    {
      delete[] arr;
      arr = NULL;
    }
}

void ReadConfig( std::string ConfigName, int& ImNum, float& scale, 
		 int& window_width, int& window_height,
		 float& zNear, float& zFar,
		 float n[3],
		 float o[3],
		 float c[3],
		 float**& u, float*& uBuf,
		 float*& f, 
		 float**& r, float*& rBuf,
		 float**& t, float*& tBuf,
		 std::string*& ImName,
		 std::string*& DepthName );


void ReleaseConfig( float**& u, float*& uBuf,
		    float*& f, 
		    float**& r, float*& rBuf,
		    float**& t, float*& tBuf,
		    std::string*& ImName,
		    std::string*& DepthName );

#endif
