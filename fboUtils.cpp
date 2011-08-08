#include "fboUtils.h"

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

  std::stringstream ss;
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
