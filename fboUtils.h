#ifndef FBO_UTILS_
#define FBO_UTILS

#include <iostream>
#inlcude <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glext.h"
#include "glInfo.h"

void showInfo();
bool checkFramebufferStatus();
void printFramebufferInfo();
std::string getTextureParameters(GLuint id);
std::string getRenderbufferParameters(GLuint id);
std::string convertInternalFormatToString(GLenum format);

#endif
