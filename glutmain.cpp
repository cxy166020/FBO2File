#include <GL/glut.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

#include "texture.h"
#include "timer.h"
#include "md2.h"
// #include "CImage.h"

#define MAX_X 100
#define MIN_X -100

#define MAX_Z 100
#define MIN_Z -100

#ifdef PI
#undef PI
#endif

#define PI 3.14159265

// #define     FLOOR_HEIGHT     -10

// #define     InitKnightX       0
// #define     InitKnightY       0
// #define     InitKnightZ      -50

using namespace std;

/////////////////////////////////////////////////
CMD2Model Ogro;
CMD2Model Weapon;

// CMD2Model Knight;
// CMD2Model Knight_Weapon;

bool  bTextured	 = true;
bool  bLighGL    = false;
bool  bAnimated	 = true;
float angle      = 0.0;
extern float g_angle;

// GLuint texName;

// Terrain vector (In world coordinate system)
float tx, ty, tz;

// 3D location of AR object origin
float ox, oy, oz;
  
// Camera center (In world coordinate system)
float cx, cy, cz;

// Camera principal axis (In world coordinate system)
float px, py, pz;

// Focal length (In pixels)
float cf;
/////////////////////////////////////////////////

// Calculate cross product
// c = a x b
void cross(float a[3], float b[3], float c[3])
{
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}

// Calculate cosine of angle between two vectors
float cos(float a[3], float b[3])
{
  float a_norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  float b_norm = sqrt(b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);

  return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2])/(a_norm*b_norm);
}

// Transform model coordinate system to world coordinate system
void Model2World()
{
  // Find the vector representations of model coordinate system
  // in the world coordinate system
  float xWorld[3], yWorld[3], zWorld[3];
  float xModel[3], yModel[3], zModel[3];
  
  yWorld[0] = tx; yWorld[1] = ty; yWorld[2] = tz;
  
  // This is only temporary
  // Note that the z axis of the model is sort of pointing back
  // to the camera, so the temporary vector is the negation of
  // principal axis
  zWorld[0] = -px; zWorld[1] = -py; zWorld[2] = -pz;

  cross(yWorld, zWorld, xWorld);
  cross(xWorld, yWorld, zWorld);

  xModel[0] = 1; xModel[1] = 0; xModel[2] = 0;
  yModel[0] = 0; yModel[1] = 1; yModel[2] = 0;
  zModel[0] = 0; zModel[1] = 0; zModel[2] = 1;

  // Note that matrices in OpenGL are stored column-wise
  //
  // Use direction cosine to calculate the rotation matrix
  //  - x2 -      -- cos(x2x1) cos(x2y1) cos(x2z1) --   - x1 -
  //  | y2 |   =  |  cos(y2x1) cos(y2y1) cos(y2z1)  | * | y1 |
  //  - z2 -      |- cos(z2x1) cos(z2y1) cos(z2z1) -|   - z1 -
  // cout << xWorld[0] << " " << xWorld[1] << " " << xWorld[2] << endl;
  // cout << yWorld[0] << " " << yWorld[1] << " " << yWorld[2] << endl;
  // cout << zWorld[0] << " " << zWorld[1] << " " << zWorld[2] << endl;

  float m[16];

  m[0]  = cos(xWorld, xModel); 
  m[4]  = cos(xWorld, yModel); 
  m[8]  = cos(xWorld, zModel);
  m[12] = ox;

  m[1]  = cos(yWorld, xModel);
  m[5]  = cos(yWorld, yModel);
  m[9]  = cos(yWorld, zModel);
  m[13] = oy;

  m[2]  = cos(zWorld, xModel);
  m[6]  = cos(zWorld, yModel);
  m[10] = cos(zWorld, zModel);
  m[14] = oz;

  m[3]  = 0;
  m[7]  = 0;
  m[11] = 0;
  m[15] = 1;

  // for(int i=0; i<16; i++)
  //   cout << m[i] << " ";

  cout << endl;

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // Multiply the precomputed transformation matrix to 
  // model view matrix
  glMultMatrixf(m);
}

// --------------------------------------------------
// Display() - draw the main scene.
// ----------------------------------------------

void Display( void )
{
  // Ogro.Automatic(Knight.GetX(), Knight.GetY(), Knight.GetZ());
  // Weapon.Automatic(Knight.GetX(), Knight.GetY(), Knight.GetZ());
	
  //Knight.Automatic(10.0, -10.0, 10.0, -10.0, 10.0, -10.0, 10.0, 0.0, -9.5);
  //Knight_Weapon.Automatic(10.0, -10.0, 10.0, -10.0, 10.0, -10.0, 10.0, 0.0, -9.5);
	
  // clear color and depth buffer
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();


  // gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  gluLookAt(cx, cy, cz, ox, oy, oz, tx, ty, tz);

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


  //glTranslatef( 0.0, 0.0, 25.0 );
  // Respond to left and right key strokes
  glRotatef( angle, 0.0, 1.0, 0.0 );

  // draw models
  Ogro.DrawModel( bAnimated ? timesec : 0.0 );
  Weapon.DrawModel( bAnimated ? timesec : 0.0 );
	
  // Knight.DrawModel( bAnimated ? timesec : 0.0 );
  // Knight_Weapon.DrawModel( bAnimated ? timesec : 0.0 );
	
  // Draw floor
  // glEnable(GL_TEXTURE_2D);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // glBindTexture(GL_TEXTURE_2D, texName);
	
  // glColor3f(1.0, 1.0, 1.0);
  // glBegin(GL_QUADS);
	
  // glTexCoord2f(0.0, 0.0);
  // glVertex3f(MIN_X, FLOOR_HEIGHT, MIN_Z);
	
  // glTexCoord2f(0.0, 1.0);
  // glVertex3f(MIN_X, FLOOR_HEIGHT, MAX_Z);
	
  // glTexCoord2f(1.0, 1.0);
  // glVertex3f(MAX_X, FLOOR_HEIGHT, MAX_Z);
	
  // glTexCoord2f(1.0, 0.0);
  // glVertex3f(MAX_X, FLOOR_HEIGHT, MIN_Z);
	
  // glEnd();
	
  //glDisable(GL_TEXTURE_2D);


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

  gluPerspective(fovy, (float)width/(float)height, 0.1, 50.0);

  // reset model/view matrix
  // glMatrixMode( GL_MODELVIEW );
  // glLoadIdentity();

  // Transform to world coordinate system
  Model2World();
}



// --------------------------------------------------
// Init() - setup opengl.
// --------------------------------------------------

void Init( void )
{
  // color used to clear the window
  glClearColor( 0.0, 0.0, 0.0, 0.0 );

  // smoothing polygons
  glShadeModel( GL_SMOOTH );

  // enable depth test
  glEnable( GL_DEPTH_TEST );

  // enable texture mapping
  glEnable( GL_TEXTURE_2D );

  /////////////////////////////////////////////

  CTimer::GetInstance()->Initialize();
  CTextureManager::GetInstance()->Initialize();

  // load and initialize the Ogros model
  Ogro.LoadModel( "models/Ogros.md2" );
  Ogro.LoadSkin( "models/igdosh.pcx" );
  Ogro.ScaleModel( 0.25 );
	
  // Knight.LoadModel("models/knight.md2");
  // Knight.LoadSkin("models/knight_white.tga" );
  // Knight.ScaleModel(0.25);

  // load and initialize Ogros' weapon model
  Weapon.LoadModel( "models/Weapon.md2" );
  Weapon.LoadSkin( "models/Weapon.pcx" );
  Weapon.ScaleModel( 0.25 );
	
  // Knight_Weapon.LoadModel("models/weapon_knight.md2");
  // Knight_Weapon.LoadSkin("models/weapon_knight.tga");
  // Knight_Weapon.ScaleModel(0.25);
	
  // Set Knight Initial Position
  // Knight.SetPosition(InitKnightX, InitKnightY, InitKnightZ);
  // Knight_Weapon.SetPosition(InitKnightX, InitKnightY, InitKnightZ);

  /////////////////////////////////////////////

  // opengl lighting initialization
  glDisable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );

  float lightpos[]	= { 10.0, 10.0, 100.0 };
  float lightcolor[]	= { 1.0, 1.0, 1.0, 1.0 };

  glLightfv( GL_LIGHT0, GL_POSITION, lightpos );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, lightcolor );
  glLightfv( GL_LIGHT0, GL_SPECULAR, lightcolor );
	
  // CImage floor;
  // floor.SetFileName("models/floor.ppm");
  // floor.ReadPPM();
	
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // glGenTextures(1, &texName);
  // glBindTexture(GL_TEXTURE_2D, texName);
	
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexImage2D(GL_TEXTURE_2D, 0, 3, floor.width, floor.height, 0, GL_RGB,GL_UNSIGNED_BYTE, floor.ImageData);
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
      g_angle -= 10.0;
      break;

    case GLUT_KEY_DOWN:
      g_angle += 10.0;
      break;
    }
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
  if (argc<13)
    {
      cout << "Insufficient number of arguments" << endl;
      return 0;
    }

  int ArgCount = 1;

  // Terrain vector (In world coordinate system)
  tx = atof(argv[ArgCount++]);
  ty = atof(argv[ArgCount++]);
  tz = atof(argv[ArgCount++]);

  // 3D location of AR object origin
  ox = atof(argv[ArgCount++]);
  oy = atof(argv[ArgCount++]);
  oz = atof(argv[ArgCount++]);
  
  // Camera center (In world coordinate system)
  cx = atof(argv[ArgCount++]);
  cy = atof(argv[ArgCount++]);
  cz = atof(argv[ArgCount++]);

  // Camera principal axis (In world coordinate system)
  px = atof(argv[ArgCount++]);
  py = atof(argv[ArgCount++]);
  pz = atof(argv[ArgCount++]);

  // Focal length (In pixels)
  cf = atof(argv[ArgCount++]);

  // setup glut
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

  // initialize window size
  glutInitWindowSize( 640, 480 );

  // initialize window position
  glutInitWindowPosition( 100, 100 );

  // create the window
  glutCreateWindow( "Quake2's MD2 Model Loader" );

  // setup opengl
  Init();

  // callback functions
  glutSpecialFunc( Special );
  glutReshapeFunc( Reshape );
  glutDisplayFunc( Display );
  glutIdleFunc( Idle );


  glutMainLoop();

  return 0;
}
