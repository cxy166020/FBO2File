
// in order to get function prototypes from glext.h, define GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>


// global variables
// ID of FBO
GLuint fboId;

// ID of texture
GLuint colorTextureId;              
GLuint depthTextureId; 


///////////////////////////////////////////////////////////////////////////////
int initFBO(int ImWidth, int ImHeight)
{
 
  // Texture object for colors
  glGenTextures(1, &colorTextureId);
  glBindTexture(GL_TEXTURE_2D, colorTextureId);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ImWidth, ImHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  glBindTexture(GL_TEXTURE_2D, 0);


  // Texture object for depth
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, colorTextureId);

  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // GL11.glBindTexture(GL11.GL_TEXTURE_2D, texID); // Bind texture
  // GL11.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL11.GL_DEPTH_COMPONENT, GL11.GL_UNSIGNED_BYTE, tex);
	
  // GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_NEAREST);
  // GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_NEAREST);
	
  // GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_S, GL11.GL_CLAMP);
  // GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_T, GL11.GL_CLAMP);
  
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); 

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ImWidth, ImHeight, 0, GL_;

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
      std::cout << "Video card supports GL_EXT_framebuffer_object." << std::endl;
    }
  else
    {
      fboSupported = fboUsed = false;
      std::cout << "Video card does NOT support GL_EXT_framebuffer_object." << std::endl;
    }

  if(fboSupported)
    {
      // create a framebuffer object, you need to delete them when program exits.
      glGenFramebuffersEXT(1, &fboId);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

      // attach a texture to FBO color attachement point
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTextureId, 0);

      // attach a renderbuffer to depth attachment point
      // Note: Also attach this to a texture object, we don't need to render this !!!!!!!!!!!!!!!!!!!!!!!!!!!
      // lFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);


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
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
  glDeleteTextures(1, &colorTextureId);

  // clean up FBO, RBO
  if(fboSupported)
    {
      glDeleteFramebuffersEXT(1, &fboId);
    }
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
  //  glBindTexture(GL_TEXTURE_2D, colorTextureId);
  // glGenerateMipmapEXT(GL_TEXTURE_2D);
  // glBindTexture(GL_TEXTURE_2D, 0);


  // clear framebuffer, someone claims that you don't really need to clean frame
  // buffers, if an error occurs, comment this line out to see if the probelm is solved
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
