#ifndef GL_DISPLAY_HPP
#define GL_DISPLAY_HPP

/**
 * @file /home/ryan/uml/robotics/checkers/gl_display.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 29, 2014
 */

#include <iostream>
#include "./points.hpp"

#define SCREEN_BPP     16

class Camera : public dTriplet {
public:
  bool _is_absolute;
  Camera() : dTriplet(0,0,0), _is_absolute(false) {}
};

/* function to release/destroy our resources and restoring the old desktop */
void quit( int returnCode ) {
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

class ScreenData {
  /* This is our SDL surface */
  SDL_Surface *_surface;

  /* Flags to pass to SDL_SetVideoMode */
  int videoFlags;

  /* this holds some info about our display */
  const SDL_VideoInfo *videoInfo;

  /**
   * Adapted from the NeHe openGL tutorials
   * 
   * @param videoInfo:
   * @param screen:
   * @param videoFlags:
   */
  void initSDL(const SDL_VideoInfo *& videoInfo, int &videoFlags) {
    using namespace std;
    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
      cerr << "Video initialization failed: " << SDL_GetError( ) << endl;

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
      std::cerr<<"Video query failed: "<<SDL_GetError( )<<std::endl;


    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
      videoFlags |= SDL_HWSURFACE;
    else
      videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
      videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    _surface = SDL_SetVideoMode( pixWidth, pixHeight, SCREEN_BPP,
				 videoFlags );

    /* Verify there is a surface */
    if ( !_surface ) {
      std::cerr<<"Video mode set failed: "<<SDL_GetError( )<<std::endl;
      quit( 1 );
    }
  }


  /* general OpenGL initialization function */
  int initGL() {
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* todo: Everything I'm rendering is co-planar.  Can I just ditch this code? */
    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    return( 1 );
  }
public:
  int pixWidth, pixHeight;
  double glWidth, glHeight;
  double aspectRatio;

  constexpr static double dFOV = 0.275; //field of view angle factor

  /* so, to project a mouse click onto the game-board plane I can use the center of the screen in terms of pixles and
     center in terms of the OpenGL coordinates to calibrate my offsets */
  double dHalf_x, dHalf_y;
  int iHalf_x, iHalf_y;

  void resize_window(int width, int height) {
    /* handle resize event */
    _surface = SDL_SetVideoMode( width
				 , height
				 , 16, videoFlags );
    if ( !_surface ) {
      std::cerr<<"Could not get a surface after resize: "<<SDL_GetError( )<<std::endl;
      quit( 1 );
    }

    /* Protect against a divide by zero */
    if ( height == 0 ) height = 1;

    aspectRatio = (double)width / (double)height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, aspectRatio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    pixWidth = width;
    pixHeight = height;

    glHeight = 3.0;
    glWidth = aspectRatio * glHeight;

    dHalf_x = glWidth / 2;
    dHalf_y = glHeight / 2;

    iHalf_x = static_cast<int>(pixWidth / 2);
    iHalf_y = static_cast<int>(pixHeight / 2);
  }

  ScreenData(int width = 640, int height = 480) : pixWidth(640), pixHeight(480) {
    initSDL( videoInfo, videoFlags );
    initGL();
    resize_window(width, height);
  }
};

dPair pix_to_gl(const ScreenData &screen
		, const iPair &pix
		, const double &depth) {
  double scale_factor = screen.dFOV * depth;
  /* I could add the arithmetic operator to the Pair objects and clean this up, but for my little project, I don't think
     it's worth the time.  */
  return dPair( (screen.dHalf_x - (((double)pix._x / (double)screen.pixWidth) * screen.glWidth) ) * scale_factor ,
		((((double)pix._y / (double)screen.pixHeight) * screen.glHeight) - screen.dHalf_y) * scale_factor );
}

iPair gl_to_pix(const ScreenData &screen, const double &x, const double &y, const double &z) {
  double scale_factor = -(screen.dFOV * z);
  return iPair( (int)(x / (screen.dHalf_x * scale_factor)) * screen.iHalf_x + screen.iHalf_x
		, (int)(y / (screen.dHalf_y * scale_factor)) * screen.iHalf_y + screen.iHalf_y
		);
}

iPair gl_to_pix(const ScreenData &screen
		,const dPair& point
		,const double &_z) {
  return gl_to_pix(screen, point._x, point._y, _z);
}

iPair gl_to_pix(const ScreenData &screen
		, const dTriplet &point) {
  return gl_to_pix(screen, point._x, point._y, point._z);
}

#endif


