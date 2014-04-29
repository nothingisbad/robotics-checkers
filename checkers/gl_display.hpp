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

#define SCREEN_BPP     24

class Camera : public dTriplet {
public:
  bool _is_absolute;
  Camera(double x, double y, double z) : dTriplet(x,y,z), _is_absolute(false) {}
  Camera& operator=(const dTriplet& other) {
    *reinterpret_cast<dTriplet*>(this) = other;
    return *this;
  }
};

struct ScreenData {
  SDL_Window* window;
  SDL_GLContext context;

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
    glViewport( 0, 0, width, height );

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
    using namespace std;
    /* SDL_DisplayMode current; */

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, SCREEN_BPP);

    /* GetCurrentDisplayMode fails on the laptop.  */
    /* if( SDL_GetCurrentDisplayMode( 0, &current ) ) { */
    /*   cout << "Error!" << endl; */
    /*   exit(1); */
    /* } */

    /* pixWidth = current.w; */
    /* pixHeight = current.h; */

    pixWidth = 1366;
    pixHeight = 768;

    window = SDL_CreateWindow("gl_checkers"
			      , SDL_WINDOWPOS_UNDEFINED
			      , SDL_WINDOWPOS_UNDEFINED
			      , pixWidth, pixHeight
			      , SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
			      | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);

    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* todo: Everything I'm rendering is co-planar.  Can I just ditch this code? */
    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    std::cout << "W/H: " << pixWidth << "/" << pixHeight << std::endl;
    resize_window(pixWidth, pixHeight);
  }

  ~ScreenData() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

dPair pix_to_gl(const ScreenData &screen
		, const iPair &pix
		, const double &depth) {
  double scale_factor = screen.dFOV * depth;
  /* I could add the arithmetic operator to the Pair objects and clean this up, but for my little project, I don't think
     it's worth the time.  */
  return dPair( (screen.dHalf_x - (((double)pix.x / (double)screen.pixWidth) * screen.glWidth) ) * scale_factor ,
		((((double)pix.y / (double)screen.pixHeight) * screen.glHeight) - screen.dHalf_y) * scale_factor );
}

iPair gl_to_pix(const ScreenData &screen, const double &x, const double &y, const double &z) {
  double scale_factor = -(screen.dFOV * z);
  return iPair( (int)(x / (screen.dHalf_x * scale_factor)) * screen.iHalf_x + screen.iHalf_x
		, (int)(y / (screen.dHalf_y * scale_factor)) * screen.iHalf_y + screen.iHalf_y
		);
}

iPair gl_to_pix(const ScreenData &screen
		,const dPair& point
		,const double &z) {
  return gl_to_pix(screen, point.x, point.y, z);
}

iPair gl_to_pix(const ScreenData &screen
		, const dTriplet &point) {
  return gl_to_pix(screen, point.x, point.y, point.z);
}

#endif


