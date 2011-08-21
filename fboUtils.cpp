#include "fboUtils.h"


///////////////////////////////////////////////////////////////////////////////
// To be called by GL init function
///////////////////////////////////////////////////////////////////////////////
void initFBO(int ImWidth, int ImHeight, GLuint& colorTextureId, GLuint& depthTextureId, GLuint& fboId)
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
  glBindTexture(GL_TEXTURE_2D, depthTextureId);
  
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ImWidth, ImHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  

  // get OpenGL info
  glInfo glInfo;
  glInfo.getInfo();
  // glInfo.printSelf();

  if(glInfo.isExtensionSupported("GL_EXT_framebuffer_object"))
    {
      std::cout << "Video card supports GL_EXT_framebuffer_object." << std::endl;
    }
  else
    {
      std::cout << "Video card does NOT support GL_EXT_framebuffer_object." << std::endl;

      return;
    }


  // create a framebuffer object, you need to delete them when program exits.
  glGenFramebuffersEXT(1, &fboId);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

  // attach a texture to FBO color attachement point
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTextureId, 0);

  // attach depth texture
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureId, 0);

  // Chekc if frame buffer is correctly setup
  checkFramebufferStatus();

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
 
}


///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
// void showInfo()
// {
//   // backup current model-view matrix
//   glPushMatrix();                     // save current modelview matrix
//   glLoadIdentity();                   // reset modelview matrix

//   // set to 2D orthogonal projection
//   glMatrixMode(GL_PROJECTION);        // switch to projection matrix
//   glPushMatrix();                     // save current projection matrix
//   glLoadIdentity();                   // reset projection matrix
//   gluOrtho2D(0, screenWidth, 0, screenHeight);  // set to orthogonal projection

//   const int FONT_HEIGHT = 14;
//   float color[4] = {1, 1, 1, 1};

//   std::stringstream ss;
//   ss << "FBO: ";
//   if(fboUsed)
//     ss << "on" << ends;
//   else
//     ss << "off" << ends;

//   drawString(ss.str().c_str(), 1, screenHeight-FONT_HEIGHT, color, font);
//   ss.str(""); // clear buffer

//   ss << std::fixed << std::setprecision(3);
//   ss << "Render-To-Texture Time: " << renderToTextureTime << " ms" << ends;
//   drawString(ss.str().c_str(), 1, screenHeight-(2*FONT_HEIGHT), color, font);
//   ss.str("");

//   ss << "Press SPACE to toggle FBO." << ends;
//   drawString(ss.str().c_str(), 1, 1, color, font);

//   // unset floating format
//   ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

//   // restore projection matrix
//   glPopMatrix();                   // restore to previous projection matrix

//   // restore modelview matrix
//   glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
//   glPopMatrix();                   // restore to previous modelview matrix
// }



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
  std::cout << "\n***** FBO STATUS *****\n";

  // print max # of colorbuffers supported by FBO
  int colorBufferCount = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &colorBufferCount);
  std::cout << "Max Number of Color Buffer Attachment Points: " << colorBufferCount << std::endl;

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

	  std::cout << "Color Attachment " << i << ": ";
	  if(objectType == GL_TEXTURE)
	    std::cout << "GL_TEXTURE, " << getTextureParameters(objectId) << std::endl;
	  else if(objectType == GL_RENDERBUFFER_EXT)
	    std::cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << std::endl;
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

      std::cout << "Depth Attachment: ";
      switch(objectType)
        {
        case GL_TEXTURE:
	  std::cout << "GL_TEXTURE, " << getTextureParameters(objectId) << std::endl;
	  break;
        case GL_RENDERBUFFER_EXT:
	  std::cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << std::endl;
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

      std::cout << "Stencil Attachment: ";
      switch(objectType)
        {
        case GL_TEXTURE:
	  std::cout << "GL_TEXTURE, " << getTextureParameters(objectId) << std::endl;
	  break;
        case GL_RENDERBUFFER_EXT:
	  std::cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << std::endl;
	  break;
        }
    }

  std::cout << std::endl;
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

// Read config file
// ImNum: Total number of input images
// scale: scale of model
// n: terrain vector, y axis of model
// o: 3D location of AR object origin
// c: 3D locations of camera
// u: up vector of current view
// f: focal length 
// r: rotation matrix
// t: translation matrix
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
		 std::string*& DepthName )
{
  std::ifstream ifm;

  // Open the configuration file
  ifm.open(ConfigName.c_str());

  // Exit if the file cannot be openned
  if(!ifm.good())
    {
      std::cerr<<"Cannot open config file " << ConfigName <<  " !!!"<<std::endl;
      exit(1);
    }

  // Read number of images, sweeping distance, image name and scale
  ifm >> ImNum >> scale >> zNear >> zFar >> window_width >> window_height;

#ifdef VERBOSE__
  std::cout << "Model scale is                    : " << scale << std::endl;
  std::cout << "zNear is                          : " << zNear << std::endl;
  std::cout << "zFar is                           : " << zFar << std::endl;
  std::cout << "Number of input images in sequence: " << ImNum  << std::endl;
  std::cout << "Image width                       : " << window_width << std::endl;
  std::cout << "Image height                      : " << window_height << std::endl;
#endif

  // Allocate memory for t, o, c, r
  u = Allocate2DArray<float>(ImNum, 3, uBuf);
  r = Allocate2DArray<float>(ImNum, 9, rBuf);
  t = Allocate2DArray<float>(ImNum, 3, tBuf);
  f = new float[ImNum];
  ImName = new std::string[ImNum];
  DepthName = new std::string[ImNum];

  // Read terrain vector
  ifm >> n[0] >> n[1] >> n[2];
#ifdef VERBOSE__
  std::cout << "Terrain vector                    : " 
	    << n[0] << " " << n[1] << " " << n[2] << std::endl; 
#endif

  // Read AR object coordinates
  ifm >> o[0] >> o[1] >> o[2];
#ifdef VERBOSE__
  std::cout << "Location of AR object             : " 
	    << o[0] << " " << o[1] << " " << o[2] << std::endl;
#endif

  // Read the coordinate of camera center of view 1
  ifm >> c[0] >> c[1] >> c[2];
#ifdef VERBOSE__
  std::cout << "Camera center of view 1           : " 
	    << c[0] << " " << c[1] << " " << c[1] << std::endl;
#endif

  // Start reading projective matrices
  for(int i=0; i<ImNum; i++)
    {
      // Read image name
      ifm >> ImName[i];
#ifdef VERBOSE__
      std::cout << "Image name                        : "
		<< ImName[i] << std::endl;
#endif

      // Read depth name
      ifm >> DepthName[i];

      // Up vector
      ifm >> u[i][0] >> u[i][1] >> u[i][2]; 
#ifdef VERBOSE__
      std::cout << "Up vector                         : " 
		<< u[i][0] << " " << u[i][1] << " " << u[i][2] << std::endl;
#endif

      // Focal length
      ifm >> f[i];
#ifdef VERBOSE__
      std::cout << "Focal length                      : "
		<< f[i] << std::endl;

      std::cout << "Rotation                          : ";
#endif

      // Rotation
      for(int j=0; j<9; j++)
	{
	  ifm >> r[i][j];
#ifdef VERBOSE__
	  std::cout << r[i][j] << " ";
#endif
	}
#ifdef VERBOSE__
      std::cout << std::endl;
#endif

      // Translation
      ifm >> t[i][0] >> t[i][1] >> t[i][2];
#ifdef VERBOSE__
      std::cout << "Translation                       : " 
		<< t[i][0] << " " << t[i][1] << " " << t[i][2] << std::endl;
#endif
    }

  ifm.close();
}

void ReleaseConfig( float**& u, float*& uBuf,
		    float*& f, 
		    float**& r, float*& rBuf,
		    float**& t, float*& tBuf,
		    std::string*& ImName,
		    std::string*& DepthName )
{
  Delete2DArray<float>(u, uBuf);
  Delete2DArray<float>(r, rBuf);
  Delete2DArray<float>(t, tBuf);

  delete[] f;
  delete[] ImName;
  delete[] DepthName;
}
