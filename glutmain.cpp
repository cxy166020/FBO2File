#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <fstream>

#include "texture.h"
#include "timer.h"
#include "md2.h"


#include "fboUtils.h"
#include "imageMix.h"

#define MAX_X  100
#define MIN_X -100

#define MAX_Z  100
#define MIN_Z -100

#ifdef PI
#undef PI
#endif

#define PI 3.14159265

using namespace std;

CMD2Model Ogro;
CMD2Model Weapon;

CImageMix mixer;

bool b_record = true;
bool b_fbo    = false;

bool  bAnimated	 = true;
float angle      = 0.0;
extern float g_angle;

float scale = 0.25;

int window_width;
int window_height;

int ImNum = 0;
int ImCounter = 0;

std::string* ImName;

GLuint colorTextureId, depthTextureId, fboId;

// Terrain vector (In world coordinate system)
float n[3];
// float tx, ty, tz;

// 3D location of AR object origin
float o[3];
// float ox, oy, oz;

// [R T] matrix, will be used for projection transformation
float **u, *uBuf, *f, **r, *rBuf, **t, *tBuf;
  
// Camera center (In world coordinate system)
// float cx, cy, cz;

// Camera principal axis (In world coordinate system)
// float px, py, pz;

// Up vector of current view
float ux, uy, uz;

// Principal axis of first view
// float cx_1, cy_1, cz_1;
float c[3];

// Focal length (In pixels)
float cf = 600;

// Near and far plane
float zNear, zFar;

string OutputDepthName;
string OutputColorName;
/////////////////////////////////////////////////


// Clean up FBO memory
void clearFBO()
{
  glDeleteTextures(1, &colorTextureId);
  glDeleteTextures(1, &depthTextureId);

  glDeleteFramebuffersEXT(1, &fboId);

  // Release config
  cout << "Exiting..." << endl;
  ReleaseConfig(u, uBuf, f, r, rBuf, t, tBuf, ImName);
}


// Nomalize vector
void normalize(float a[3])
{
  float norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  a[0] /= norm;
  a[1] /= norm;
  a[2] /= norm;
}

// Calculate cross product
// c = a x b, c is normalized
void cross(float a[3], float b[3], float c[3])
{
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];

  normalize(c);
}

// Calculate cosine of angle between two vectors
float cos(float a[3], float b[3])
{
  float a_norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  float b_norm = sqrt(b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);

  return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2])/(a_norm*b_norm);
}


void matmul4x4_4x4(float a[16], float b[16], float c[16])
{
  c[0]  = a[0]*b[0]  + a[4]*b[1]  + a[8]*b[2]   + a[12]*b[3];
  c[4]  = a[0]*b[4]  + a[4]*b[5]  + a[8]*b[6]   + a[12]*b[7];
  c[8]  = a[0]*b[8]  + a[4]*b[9]  + a[8]*b[10]  + a[12]*b[11];
  c[12] = a[0]*b[12] + a[4]*b[13] + a[8]*b[14]  + a[12]*b[15];

  c[1]  = a[1]*b[0]  + a[5]*b[1]  + a[9]*b[2]   + a[13]*b[3];
  c[5]  = a[1]*b[4]  + a[5]*b[5]  + a[9]*b[6]   + a[13]*b[7];
  c[9]  = a[1]*b[8]  + a[5]*b[9]  + a[9]*b[10]  + a[13]*b[11];
  c[13] = a[1]*b[12] + a[5]*b[13] + a[9]*b[14]  + a[13]*b[15];
  
  c[2]  = a[2]*b[0]  + a[6]*b[1]  + a[10]*b[2]  + a[14]*b[3];
  c[6]  = a[2]*b[4]  + a[6]*b[5]  + a[10]*b[6]  + a[14]*b[7];
  c[10] = a[2]*b[8]  + a[6]*b[9]  + a[10]*b[10] + a[14]*b[11];
  c[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14]*b[15];
  
  c[3]  = a[3]*b[0]  + a[7]*b[1]  + a[11]*b[2]  + a[15]*b[3];
  c[7]  = a[3]*b[4]  + a[7]*b[5]  + a[11]*b[6]  + a[15]*b[7];
  c[11] = a[3]*b[8]  + a[7]*b[9]  + a[11]*b[10] + a[15]*b[11];
  c[15] = a[3]*b[12] + a[7]*b[13] + a[11]*b[14] + a[15]*b[15];
}


// Transform model coordinate system to world coordinate system
void Model2World()
{
  // Image counter
  int i = ImCounter;

  // Find the vector representations of model coordinate system
  // in the world coordinate system
  float xWorld[3], yWorld[3], zWorld[3];
  // float xModel[3], yModel[3], zModel[3];
  
  // yWorld[0] = tx; yWorld[1] = ty; yWorld[2] = tz;
  yWorld[0] = n[0]; yWorld[1] = n[1]; yWorld[2] = n[2];

  // The program assumes that the first view is frontal parallel
  // to the model
  // Note that the z axis of the model is sort of pointing back
  // to the camera, so the temporary vector is the negation of
  // principal axis
  // zWorld[0] = cx_1-ox; zWorld[1] = cy_1-oy; zWorld[2] = cz_1-oz;
  zWorld[0] = c[0]-o[0]; zWorld[1] = c[1]-o[1]; zWorld[2] = c[2]-o[2];

  cross(yWorld, zWorld, xWorld);
  cross(xWorld, yWorld, zWorld);
  normalize(yWorld);

  // xModel[0] = 1; xModel[1] = 0; xModel[2] = 0;
  // yModel[0] = 0; yModel[1] = 1; yModel[2] = 0;
  // zModel[0] = 0; zModel[1] = 0; zModel[2] = 1;

  // Note that matrices in OpenGL are stored column-wise
  //
  // Use direction cosine to calculate the rotation matrix
  //  - x2 -      |- cos(x2x1) cos(x2y1) cos(x2z1) -|   - x1 -
  //  | y2 |   =  |  cos(y2x1) cos(y2y1) cos(y2z1)  | * | y1 |
  //  - z2 -      |- cos(z2x1) cos(z2y1) cos(z2z1) -|   - z1 -

  float m[16];

  m[0]  = xWorld[0]; // cos(xWorld, xModel); 
  m[4]  = yWorld[0]; // cos(xWorld, yModel); 
  m[8]  = zWorld[0]; // cos(xWorld, zModel);
  m[12] = o[0];

  m[1]  = xWorld[1]; // cos(yWorld, xModel);
  m[5]  = yWorld[1]; // cos(yWorld, yModel);
  m[9]  = zWorld[1]; // cos(yWorld, zModel);
  m[13] = o[1];

  m[2]  = xWorld[2]; // cos(zWorld, xModel);
  m[6]  = yWorld[2]; // cos(zWorld, yModel);
  m[10] = zWorld[2]; // cos(zWorld, zModel);
  m[14] = o[2];

  m[3]  = 0;
  m[7]  = 0;
  m[11] = 0;
  m[15] = 1;

  float rt[16];

  // R matrix (From bundle adjustment)
  rt[0]  = r[i][0];
  rt[4]  = r[i][1];
  rt[8]  = r[i][2];

  rt[1]  = r[i][3];
  rt[5]  = r[i][4];
  rt[9]  = r[i][5];

  rt[2]  = r[i][6];
  rt[6]  = r[i][7];
  rt[10] = r[i][8];

  // T matrix (From bundle adjustment, T is associated with focal length,
  // bundle adjustment should use pre-calibrated focal length in order
  // to preserve scale)
  rt[12] = t[i][0];
  rt[13] = t[i][1];
  rt[14] = t[i][2];

  rt[3]  = 0;
  rt[7]  = 0;
  rt[11] = 0;
  rt[15] = 1;

  // Counter increment
  // if(i<ImNum-1)
  //   i++;

  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // Multiply the precomputed transformation matrix to 
  // model view matrix
  float rm[16];
  
  matmul4x4_4x4(rt, m, rm);
  
  // glMultMatrixf(r);
  // glMultMatrixf(m);
  glMultMatrixf(rm);
}

void record()
{
  if(!b_record)
    return;

  const int ColorChannel = 3;

  unsigned char* depth = new unsigned char[window_width*window_height];
  unsigned char* color = new unsigned char[window_width*window_height*ColorChannel];

  // glBindTexture(GL_TEXTURE_2D, depthTextureId);
  glReadPixels(0, 0, window_width, window_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depth);
  glReadPixels(0, 0, window_width, window_height, GL_RGB,             GL_UNSIGNED_BYTE, color);
  // glBindTexture(GL_TEXTURE_2D, 0);

  // ofstream ofm;
  // ofm.open(OutputDepthName.c_str(), ios::binary | ios::trunc);
  // ofm.write((char*)depth, window_width*window_height);
  // ofm.close();

  // ofm.open(OutputColorName.c_str(), ios::binary | ios::trunc);
  // ofm.write((char*)color, window_width*window_height*ColorChannel);
  // ofm.close();

  delete[] depth;
  delete[] color;

  b_record = false;
}

// --------------------------------------------------
// Display() - draw the main scene.
// ----------------------------------------------

void Display( void )
{
  // Render to FBO
  bool temp_b_fbo = b_fbo;
  
  if (temp_b_fbo)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
  
  
  // clear color and depth buffer
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();


  glPushMatrix();


  CTimer::GetInstance()->Update();
  float timesec = CTimer::GetInstance()->GetTimeMSec() / 1000.0;


  if( g_angle > 360.0 )
    g_angle -= 360.0;

  if( g_angle < 0.0 )
    g_angle += 360.0;

  if( angle < 0.0 )
    angle += 360.0;

  if( angle > 360.0 )
    angle -= 360.0;


  Model2World();

  // Respond to left and right key strokes
  glRotatef( angle, 0.0, 1.0, 0.0 );
  
  // draw models
  Ogro.DrawModel( bAnimated ? timesec : 0.0 );
  Weapon.DrawModel( bAnimated ? timesec : 0.0 );

  glPopMatrix();

  // record();

   //glRasterPos2i(0, 0);
  // glPixelZoom(1.0, 1.0);
  
  mixer.drawBackgrounds(ImCounter);

  // back to normal window-system-provided framebuffer
  if (temp_b_fbo)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  
  // swap buffers and redisplay the scene
  glutSwapBuffers();
  glutPostRedisplay();
}



// --------------------------------------------------
// Reshape() - used when the window is resized.
// --------------------------------------------------

void Reshape( int width, int height )
{
  // prevent division by zero
  if( height == 0 )
    height = 1;

  glViewport( 0, 0, width, height );

  // reset projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  // gluPerspective( 70.0, (float)width/(float)height, 0.1, 50.0 );
  float fovy = 2*atan(height/(2*cf))*180/PI;

  gluPerspective(fovy, (float)width/(float)height, zNear, zFar);

  // reset model/view matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // Transform to world coordinate system
  // Model2World();
}



// --------------------------------------------------
// Init() - setup opengl.
// --------------------------------------------------

void Init( int width, int height )
{
  // color used to clear the window
  glClearColor( 0.0, 0.0, 0.0, 0.0 );

  // smoothing polygons
  glShadeModel( GL_SMOOTH );

  // enable depth test
  glEnable( GL_DEPTH_TEST );

  // enable texture mapping
  glEnable( GL_TEXTURE_2D );

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // Remeber to set clear depth value !!!
  // glClearDepth(1.0f);

  initFBO(width, height, colorTextureId, depthTextureId, fboId);

  /////////////////////////////////////////////

  CTimer::GetInstance()->Initialize();
  CTextureManager::GetInstance()->Initialize();

  // load and initialize the Ogros model
  Ogro.LoadModel( "models/Ogros.md2" );
  Ogro.LoadSkin( "models/igdosh.pcx" );
  Ogro.ScaleModel( scale );
	

  // load and initialize Ogros' weapon model
  Weapon.LoadModel( "models/Weapon.md2" );
  Weapon.LoadSkin( "models/Weapon.pcx" );
  Weapon.ScaleModel( scale );
	

  // opengl lighting initialization
  glDisable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );

  float lightpos[]	= { 10.0, 10.0, 100.0 };
  float lightcolor[]	= { 1.0, 1.0, 1.0, 1.0 };

  glLightfv( GL_LIGHT0, GL_POSITION, lightpos );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, lightcolor );
  glLightfv( GL_LIGHT0, GL_SPECULAR, lightcolor );
}



// --------------------------------------------------
// Special() - keyboard input (special keys).
// --------------------------------------------------

void Special( int key, int x, int y )
{
  switch( key )
    {
    case GLUT_KEY_LEFT:
      angle -= 5.0;
      break;

    case GLUT_KEY_RIGHT:
      angle += 5.0;
      break;

    case GLUT_KEY_UP:

      if(ImCounter<ImNum-1)
	ImCounter++;
      else
	ImCounter=0; 
      
      break;

    case GLUT_KEY_DOWN:
      
      if(ImCounter>0)
	ImCounter--;
      else
	ImCounter=ImNum-1;

      break;
    }
}

void Keyboard(unsigned char key, int x, int y)
{
  if( key == ' ' )
    b_fbo = !b_fbo;
  else if( key != 'd' )
    b_record = true;
}

void Idle()
{
  CTimer::GetInstance()->Update();
	
  glutPostRedisplay();
}



// --------------------------------------------------
// main() - main function.
// --------------------------------------------------
// Note: This program should also has a depth map as output
// so that the AR program can do its own depth test
int main( int argc, char *argv[] )
{
  // if (argc<13)
  //   {
  //     cout << "Insufficient number of arguments" << endl;
  //     return 0;
  //   }

  int ArgCount = 1;
  
  std::string ConfigName = argv[ArgCount++];

 
  ReadConfig(ConfigName, ImNum, scale, window_width, window_height,
	     zNear, zFar, n, o, c, u, uBuf, f, r, rBuf, t, tBuf, ImName);

  mixer.loadBackgrounds(ImName, ImNum);

  // OutputDepthName = argv[ArgCount++];
  // OutputColorName = argv[ArgCount++];


  // setup glut
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

  // initialize window size
  glutInitWindowSize( window_width, window_height );

  // initialize window position
  glutInitWindowPosition( 100, 100 );

  // create the window
  glutCreateWindow( "Quake2's MD2 Model Loader" );

  // setup opengl
  Init( window_width, window_height );

  // Clean up functions
  atexit(clearFBO);

  // callback functions
  glutSpecialFunc( Special );
  glutReshapeFunc( Reshape );
  glutDisplayFunc( Display );
  glutKeyboardFunc( Keyboard );
  glutIdleFunc( Idle );


  glutMainLoop();

  // // Release config
  // cout << "Exiting..." << endl;
  // ReleaseConfig(u, uBuf, f, r, rBuf, t, tBuf);

  return 0;
}
