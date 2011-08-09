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

#endif
