
// in order to get function prototypes from glext.h, define GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glext.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include "glInfo.h"                             // glInfo struct
#include "Timer.h"
#include "teapot.h"

using std::stringstream;
using std::string;
using std::cout;
using std::endl;
using std::ends;


// global variables
void *font = GLUT_BITMAP_8_BY_13;
GLuint fboId;                       // ID of FBO
GLuint imTextureId;                   // ID of texture
GLuint rboId;                       // ID of Renderbuffer object
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int screenWidth, screenHeight;
bool fboSupported;
bool fboUsed;
int drawMode = 0;
Timer timer, t1;
float playTime;                     // to compute rotation angle
float renderToTextureTime;          // elapsed time for render-to-texture


///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
 
  // Create a texture object,
  glGenTextures(1, &imTextureId);
  glBindTexture(GL_TEXTURE_2D, imTextureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // 0 or techniqually speaking, NULL texture object unbinds the texutre buffer, so whatever settings
  // we have done so far will not haunt us later
  glBindTexture(GL_TEXTURE_2D, 0);

  // Note !!!!!!!!!!!!!!!!!!!!!!!!
  // When you create a texture image for depth buffer, use GL_RGBA32F_ARB to make sure than memory use floating number
  // rather than integers !!!!!!!!
  // Remember to set the texture image size to image width and image height
  // We don't need mipmaps, get rid of them

  // get OpenGL info
  glInfo glInfo;
  glInfo.getInfo();
  glInfo.printSelf();


  if(glInfo.isExtensionSupported("GL_EXT_framebuffer_object"))
    {
      fboSupported = fboUsed = true;
      cout << "Video card supports GL_EXT_framebuffer_object." << endl;
    }
  else
    {
      fboSupported = fboUsed = false;
      cout << "Video card does NOT support GL_EXT_framebuffer_object." << endl;
    }

  if(fboSupported)
    {
      // create a framebuffer object, you need to delete them when program exits.
      glGenFramebuffersEXT(1, &fboId);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

      // create a renderbuffer object to store depth info
      // NOTE: A depth renderable image should be attached the FBO for depth test.
      // If we don't attach a depth renderable image to the FBO, then
      // the rendering output will be corrupted because of missing depth test.
      // If you also need stencil test for your rendering, then you must
      // attach additional image to the stencil attachement point, too.
      glGenRenderbuffersEXT(1, &rboId);
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
      glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

      // attach a texture to FBO color attachement point
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, imTextureId, 0);

      // attach a renderbuffer to depth attachment point
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);

      //@ disable color buffer if you don't attach any color buffer image,
      //@ for example, rendering depth buffer only to a texture.
      //@ Otherwise, glCheckFramebufferStatusEXT will not be complete.
      //glDrawBuffer(GL_NONE);
      //glReadBuffer(GL_NONE);

      // check FBO status
      printFramebufferInfo();
      bool status = checkFramebufferStatus();
      if(!status)
	fboUsed = false;

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

  // start timer, the elapsed time will be used for rotating the teapot
  timer.start();

  // the last GLUT call (LOOP)
  // window will be shown and display callback is triggered by events
  // NOTE: this call never return main().
  glutMainLoop(); /* Start GLUT event-processing loop */

  return 0;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
  glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

  // enable /disable features
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);

  // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  glClearColor(0, 0, 0, 0);                   // background color
  glClearStencil(0);                          // clear stencil buffer
  glClearDepth(1.0f);                         // 0 is near, 1 is far
  glDepthFunc(GL_LEQUAL);

  initLights();
  setCamera(0, 0, 6, 0, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
  screenWidth = SCREEN_WIDTH;
  screenHeight = SCREEN_HEIGHT;
  mouseLeftDown = mouseRightDown = false;
  cameraAngleX = cameraAngleY = 45;
  cameraDistance = 0;
  playTime = 0;
  return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
  glDeleteTextures(1, &imTextureId);

  // clean up FBO, RBO
  if(fboSupported)
    {
      glDeleteFramebuffersEXT(1, &fboId);
      glDeleteRenderbuffersEXT(1, &rboId);
    }
}




///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
  // backup current model-view matrix
  glPushMatrix();                     // save current modelview matrix
  glLoadIdentity();                   // reset modelview matrix

  // set to 2D orthogonal projection
  glMatrixMode(GL_PROJECTION);        // switch to projection matrix
  glPushMatrix();                     // save current projection matrix
  glLoadIdentity();                   // reset projection matrix
  gluOrtho2D(0, screenWidth, 0, screenHeight);  // set to orthogonal projection

  const int FONT_HEIGHT = 14;
  float color[4] = {1, 1, 1, 1};

  stringstream ss;
  ss << "FBO: ";
  if(fboUsed)
    ss << "on" << ends;
  else
    ss << "off" << ends;

  drawString(ss.str().c_str(), 1, screenHeight-FONT_HEIGHT, color, font);
  ss.str(""); // clear buffer

  ss << std::fixed << std::setprecision(3);
  ss << "Render-To-Texture Time: " << renderToTextureTime << " ms" << ends;
  drawString(ss.str().c_str(), 1, screenHeight-(2*FONT_HEIGHT), color, font);
  ss.str("");

  ss << "Press SPACE to toggle FBO." << ends;
  drawString(ss.str().c_str(), 1, 1, color, font);

  // unset floating format
  ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

  // restore projection matrix
  glPopMatrix();                   // restore to previous projection matrix

  // restore modelview matrix
  glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
  glPopMatrix();                   // restore to previous modelview matrix
}






///////////////////////////////////////////////////////////////////////////////
// check FBO completeness
///////////////////////////////////////////////////////////////////////////////
bool checkFramebufferStatus()
{
  // check FBO status
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      std::cout << "Framebuffer complete." << std::endl;
      return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
      return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
      return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
      return false;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
      return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
      return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
      return false;

    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
      return false;

    default:
      std::cout << "[ERROR] Unknow error." << std::endl;
      return false;
    }
}



///////////////////////////////////////////////////////////////////////////////
// print out the FBO infos
///////////////////////////////////////////////////////////////////////////////
void printFramebufferInfo()
{
  cout << "\n***** FBO STATUS *****\n";

  // print max # of colorbuffers supported by FBO
  int colorBufferCount = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &colorBufferCount);
  cout << "Max Number of Color Buffer Attachment Points: " << colorBufferCount << endl;

  int objectType;
  int objectId;

  // print info of the colorbuffer attachable image
  for(int i = 0; i < colorBufferCount; ++i)
    {
      glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					       GL_COLOR_ATTACHMENT0_EXT+i,
					       GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
					       &objectType);
      if(objectType != GL_NONE)
        {
	  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
						   GL_COLOR_ATTACHMENT0_EXT+i,
						   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
						   &objectId);

	  std::string formatName;

	  cout << "Color Attachment " << i << ": ";
	  if(objectType == GL_TEXTURE)
	    cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
	  else if(objectType == GL_RENDERBUFFER_EXT)
	    cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
        }
    }

  // print info of the depthbuffer attachable image
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					   GL_DEPTH_ATTACHMENT_EXT,
					   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
					   &objectType);
  if(objectType != GL_NONE)
    {
      glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					       GL_DEPTH_ATTACHMENT_EXT,
					       GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
					       &objectId);

      cout << "Depth Attachment: ";
      switch(objectType)
        {
        case GL_TEXTURE:
	  cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
	  break;
        case GL_RENDERBUFFER_EXT:
	  cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
	  break;
        }
    }

  // print info of the stencilbuffer attachable image
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					   GL_STENCIL_ATTACHMENT_EXT,
					   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
					   &objectType);
  if(objectType != GL_NONE)
    {
      glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					       GL_STENCIL_ATTACHMENT_EXT,
					       GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
					       &objectId);

      cout << "Stencil Attachment: ";
      switch(objectType)
        {
        case GL_TEXTURE:
	  cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
	  break;
        case GL_RENDERBUFFER_EXT:
	  cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
	  break;
        }
    }

  cout << endl;
}



///////////////////////////////////////////////////////////////////////////////
// return texture parameters as string using glGetTexLevelParameteriv()
///////////////////////////////////////////////////////////////////////////////
std::string getTextureParameters(GLuint id)
{
  if(glIsTexture(id) == GL_FALSE)
    return "Not texture object";

  int width, height, format;
  std::string formatName;
  glBindTexture(GL_TEXTURE_2D, id);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);            // get texture width
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);          // get texture height
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format); // get texture internal format
  glBindTexture(GL_TEXTURE_2D, 0);

  formatName = convertInternalFormatToString(format);

  std::stringstream ss;
  ss << width << "x" << height << ", " << formatName;
  return ss.str();
}



///////////////////////////////////////////////////////////////////////////////
// return renderbuffer parameters as string using glGetRenderbufferParameterivEXT
///////////////////////////////////////////////////////////////////////////////
std::string getRenderbufferParameters(GLuint id)
{
  if(glIsRenderbufferEXT(id) == GL_FALSE)
    return "Not Renderbuffer object";

  int width, height, format;
  std::string formatName;
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id);
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width);    // get renderbuffer width
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &height);  // get renderbuffer height
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &format); // get renderbuffer internal format
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

  formatName = convertInternalFormatToString(format);

  std::stringstream ss;
  ss << width << "x" << height << ", " << formatName;
  return ss.str();
}



///////////////////////////////////////////////////////////////////////////////
// convert OpenGL internal format enum to string
///////////////////////////////////////////////////////////////////////////////
std::string convertInternalFormatToString(GLenum format)
{
  std::string formatName;

  switch(format)
    {
    case GL_STENCIL_INDEX:
      formatName = "GL_STENCIL_INDEX";
      break;
    case GL_DEPTH_COMPONENT:
      formatName = "GL_DEPTH_COMPONENT";
      break;
    case GL_ALPHA:
      formatName = "GL_ALPHA";
      break;
    case GL_RGB:
      formatName = "GL_RGB";
      break;
    case GL_RGBA:
      formatName = "GL_RGBA";
      break;
    case GL_LUMINANCE:
      formatName = "GL_LUMINANCE";
      break;
    case GL_LUMINANCE_ALPHA:
      formatName = "GL_LUMINANCE_ALPHA";
      break;
    case GL_ALPHA4:
      formatName = "GL_ALPHA4";
      break;
    case GL_ALPHA8:
      formatName = "GL_ALPHA8";
      break;
    case GL_ALPHA12:
      formatName = "GL_ALPHA12";
      break;
    case GL_ALPHA16:
      formatName = "GL_ALPHA16";
      break;
    case GL_LUMINANCE4:
      formatName = "GL_LUMINANCE4";
      break;
    case GL_LUMINANCE8:
      formatName = "GL_LUMINANCE8";
      break;
    case GL_LUMINANCE12:
      formatName = "GL_LUMINANCE12";
      break;
    case GL_LUMINANCE16:
      formatName = "GL_LUMINANCE16";
      break;
    case GL_LUMINANCE4_ALPHA4:
      formatName = "GL_LUMINANCE4_ALPHA4";
      break;
    case GL_LUMINANCE6_ALPHA2:
      formatName = "GL_LUMINANCE6_ALPHA2";
      break;
    case GL_LUMINANCE8_ALPHA8:
      formatName = "GL_LUMINANCE8_ALPHA8";
      break;
    case GL_LUMINANCE12_ALPHA4:
      formatName = "GL_LUMINANCE12_ALPHA4";
      break;
    case GL_LUMINANCE12_ALPHA12:
      formatName = "GL_LUMINANCE12_ALPHA12";
      break;
    case GL_LUMINANCE16_ALPHA16:
      formatName = "GL_LUMINANCE16_ALPHA16";
      break;
    case GL_INTENSITY:
      formatName = "GL_INTENSITY";
      break;
    case GL_INTENSITY4:
      formatName = "GL_INTENSITY4";
      break;
    case GL_INTENSITY8:
      formatName = "GL_INTENSITY8";
      break;
    case GL_INTENSITY12:
      formatName = "GL_INTENSITY12";
      break;
    case GL_INTENSITY16:
      formatName = "GL_INTENSITY16";
      break;
    case GL_R3_G3_B2:
      formatName = "GL_R3_G3_B2";
      break;
    case GL_RGB4:
      formatName = "GL_RGB4";
      break;
    case GL_RGB5:
      formatName = "GL_RGB4";
      break;
    case GL_RGB8:
      formatName = "GL_RGB8";
      break;
    case GL_RGB10:
      formatName = "GL_RGB10";
      break;
    case GL_RGB12:
      formatName = "GL_RGB12";
      break;
    case GL_RGB16:
      formatName = "GL_RGB16";
      break;
    case GL_RGBA2:
      formatName = "GL_RGBA2";
      break;
    case GL_RGBA4:
      formatName = "GL_RGBA4";
      break;
    case GL_RGB5_A1:
      formatName = "GL_RGB5_A1";
      break;
    case GL_RGBA8:
      formatName = "GL_RGBA8";
      break;
    case GL_RGB10_A2:
      formatName = "GL_RGB10_A2";
      break;
    case GL_RGBA12:
      formatName = "GL_RGBA12";
      break;
    case GL_RGBA16:
      formatName = "GL_RGBA16";
      break;
    default:
      formatName = "Unknown Format";
    }

  return formatName;
}



//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
  // with FBO
  // render directly to a texture

  // set the rendering destination to FBO
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

  // back to normal window-system-provided framebuffer
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // unbind

  // trigger mipmaps generation explicitly
  // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
  // triggers mipmap generation automatically. However, the texture attached
  // onto a FBO should generate mipmaps manually via glGenerateMipmapEXT().
  //  glBindTexture(GL_TEXTURE_2D, imTextureId);
  // glGenerateMipmapEXT(GL_TEXTURE_2D);
  // glBindTexture(GL_TEXTURE_2D, 0);


  // clear framebuffer
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}



void exitCB()
{
  clearSharedMem();
}
