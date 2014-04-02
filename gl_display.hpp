#ifndef GL_DISPLAY_HPP
#define GL_DISPLAY_HPP

/**
 * @file /home/ryan/uml/robotics/checkers/gl_display.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 29, 2014
 */

#include <iostream>
#include <vector>
#include <functional>

#include "./points.hpp"

#define SCREEN_BPP     16

class Camera : public dTriplet {
public:
  bool _is_absolute;
  Camera(double x, double y, double z) : dTriplet(x,y,z), _is_absolute(false) {}
};

/* function to release/destroy our resources and restoring the old desktop */
void quit( int returnCode ) {
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

struct ScreenData {
  /**
   * Adapted from the NeHe openGL tutorials and sdl2_opengl.c gist
   */
  SDL_Window* window;
  SDL_Renderer* renderer;

  // public:
  typedef std::function<void (ScreenData&)> ResizeCallback;
  typedef std::vector<ResizeCallback> ResizeVec;
  ResizeVec _resize_callback;

  /* so, to project a mouse click onto the game-board plane I can use the center of the screen in terms of pixles and
     center in terms of the OpenGL coordinates to calibrate my offsets */
  double dHalf_x, dHalf_y;
  int iHalf_x, iHalf_y;

  int pixWidth, pixHeight;
  double glWidth, glHeight;
  double aspectRatio;

  constexpr static double dFOV = 0.275; //field of view angle factor


  void resize_callback(std::function<void (ScreenData&)> fn) {
    _resize_callback.push_back(fn);
  }

  void resize_window(int width, int height) {
    /* Protect against a divide by zero */
    if ( height == 0 ) height = 1;

    aspectRatio = (double)width / (double)height;

    SDL_SetWindowSize(window, width, height);

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, aspectRatio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    pixWidth = width;
    pixHeight = height;

    glHeight = 3.0;
    glWidth = aspectRatio * glHeight;

    dHalf_x = glWidth / 2;
    dHalf_y = glHeight / 2;

    iHalf_x = static_cast<int>(pixWidth / 2);
    iHalf_y = static_cast<int>(pixHeight / 2);

    for(auto i = _resize_callback.begin(), end = _resize_callback.end()
	  ; i != end
	  ; ++i)
      (*i)(*this);
  }

  ScreenData() {
    SDL_DisplayMode current;

    SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_OPENGL, &window, &renderer);

    if( SDL_GetCurrentDisplayMode(0, &current) ) {
      std::cerr << "error getting display" << std::endl;
      quit(1);
    }
      
    pixWidth = current.w;
    pixHeight = current.h;

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

    resize_window(pixWidth, pixHeight);
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


