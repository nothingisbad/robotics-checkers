
/**
 * @file /home/ryan/uml/robotics/checkers/GUI.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 29, 2014
 *
 * Simple GUI for playing checkers against the AI without the robot.
 * I'm recycling an older project for the interface.
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

#include <iostream>
#include <chrono>

#include "./gl_display.hpp"
#include "./checkers.hpp"

/* screen width, height, and bit depth */
#define SCREEN_BPP     16

using namespace std;
using namespace std::chrono;

namespace simple_colors {
  static const dTriplet green = dTriplet(0.1, 0.9, 0.1);
  static const dTriplet red = dTriplet(0.9, 0.1, 0.1);
  static const dTriplet blue = dTriplet(0.1, 0.1, 0.9);
  static const dTriplet black = dTriplet(0.4, 0.4, 0.4);
}

inline void color(dTriplet c) { glColor3f(c._x, c._y, c._z); }

class SimpleTimer {
  time_point<steady_clock> m_ending;
  milliseconds m_how_long;
public:
  template<class DurationType>
  SimpleTimer(DurationType d) : m_how_long(d) {};

  void reset() {
    m_ending = chrono::steady_clock::now() + m_how_long;
  }

  bool done() {
    return chrono::steady_clock::now() > m_ending;
  }
};

/* This is our SDL surface */
SDL_Surface *surface;

/* function to release/destroy our resources and restoring the old desktop */
void quit( int returnCode ) {
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

/* function to handle key press events */
void handleKeyPress( SDL_keysym *keysym ) {
    switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
	    /* ESC key was pressed */
	    quit( 0 );
	    break;
	case SDLK_F1:
	    /* F1 key was pressed
	     * this toggles fullscreen mode
	     */

	    break;
	default:
	    break;
	}
    return;
}

/**
 * Adapted from the NeHe openGL tutorials
 * 
 * @param videoInfo:
 * @param screen:
 * @param videoFlags:
 */
void initSDL(const SDL_VideoInfo *& videoInfo, screen_data & screen, int &videoFlags) {
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
  surface = SDL_SetVideoMode( screen.pixWidth, screen.pixHeight, SCREEN_BPP,
			      videoFlags );

  /* Verify there is a surface */
  if ( !surface ) {
    std::cerr<<"Video mode set failed: "<<SDL_GetError( )<<std::endl;
    quit( 1 );
  }
}

/* general OpenGL initialization function */
int initGL( void ) {

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

inline void draw_square() {
  glBegin( GL_QUADS ); {
    glVertex3f(  0.9f,  0.9f,  0.0f ); /* Top Right Of The Quad    */
    glVertex3f( -0.9f,  0.9f,  0.0f ); /* Top Left Of The Quad     */
    glVertex3f( -0.9f, -0.9f,  0.0f ); /* Bottom Left Of The Quad  */
    glVertex3f(  0.9f, -0.9f,  0.0f ); /* Bottom Right Of The Quad */
  }
  glEnd( ); 
}

/**
 * draw a triangle in the current scene
 * 
 * @param color:
 * @param pos:
 * @param bbox: bounding box
 */
inline void draw_triangle(dTriplet color, dTriplet pos, dPair bbox) {
  glLoadIdentity( );
  glTranslatef(pos._x, pos._y, pos._z);
  glColor3f( color._x, color._y, color._z );

  glBegin( GL_TRIANGLES ); {
    glVertex3f( -(bbox._x), (bbox._y), 0 );
    glVertex3f( -(bbox._x), -(bbox._y) , 0);
    glVertex3f( -(bbox._x), 0.0, 0);
  }
  glEnd();
}  

bool is_point_on( dPair point
		  , dTriplet thing_pos
		  , dPair thing_bounds) {
  return ( (point._x > (thing_pos._x - thing_bounds._x))
	   && (point._x < (thing_pos._x + thing_bounds._x))
	   && (point._y > (thing_pos._y - thing_bounds._y))
	   && (point._y < (thing_pos._y + thing_bounds._y)) );
}


/**************************/
/*  __  __       _        */
/* |  \/  | __ _(_)_ __   */
/* | |\/| |/ _` | | '_ \  */
/* | |  | | (_| | | | | | */
/* |_|  |_|\__,_|_|_| |_| */
/**************************/
int main( int argc, char **argv ) {
    /* Flags to pass to SDL_SetVideoMode */
    int videoFlags;
    /* main loop variable */
    bool done = false;
    /* used to collect events */
    SDL_Event event;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;
    /* whether or not the window is active */
    bool isActive = true;

    screen_data screen;

    //mouse:
    dPair dMouse, mouseOffset;
    iPair iMouse;
    bool bMouseButtonDown = false;
    bool bDragging = false;

    SimpleTimer drag_timer( milliseconds(250) );

    //selection
    iPair clicked;
    dTriplet click_projection;

    dTriplet grid_center;

    Camera camera;
    dTriplet cameraDragStart;

    //game grid and logic
    Board board;

    SimpleTimer iterateGrid( milliseconds(100) );

    static const dPair cell_bbox(0.9, 0.9);

    initSDL( videoInfo, screen, videoFlags );

    /* initialize OpenGL */
    initGL( );

    /* resize the initial window */
    screen.resize_window(screen.pixWidth, screen.pixHeight );

    //setup the camera
    grid_center._x = (double)board._rows - 1.0;
    grid_center._y = (double)board._columns - 1.0;
    grid_center._z = 0;

    camera._x = grid_center._x;
    camera._y = grid_center._y;
    camera._z = -45;

    /* wait for events */ 
    while ( !done ) {
      /* handle the events in the queue */
      while ( SDL_PollEvent( &event ) ) {
	if( bMouseButtonDown ) {
	  if(bDragging) {
	    SDL_GetMouseState(&iMouse._x, &iMouse._y);
	    mouseOffset = pix_to_gl(screen,
				    iMouse,
				    camera._z);
	    camera._x = cameraDragStart._x + dMouse._x - mouseOffset._x;
	    camera._y = cameraDragStart._y + dMouse._y - mouseOffset._y;
	  } else bDragging = drag_timer.done();
	}
	switch( event.type ) {
	case SDL_ACTIVEEVENT:
	  /* Something's happend with our focus
	   * If we lost focus or we are iconified, we
	   * shouldn't draw the screen
	   */
	  if ( event.active.gain == 0 )
	    isActive = false;
	  else
	    isActive = true;
	  break; 

	case SDL_VIDEORESIZE:
	  /* handle resize event */
	  surface = SDL_SetVideoMode( event.resize.w,
				      event.resize.h,
				      16, videoFlags );
	  if ( !surface ) {
	    std::cerr<<"Could not get a surface after resize: "<<SDL_GetError( )<<std::endl;
	    quit( 1 );
	  }
	  screen.resize_window(event.resize.w, event.resize.h );

	  break;

	case SDL_MOUSEBUTTONDOWN:
	  SDL_GetMouseState(&iMouse._x, &iMouse._y);
	  dMouse = pix_to_gl(screen,
			     iMouse,
			     camera._z);
	  bMouseButtonDown = true;
	  cameraDragStart = camera;
	  drag_timer.reset();
	  break;

	case SDL_MOUSEBUTTONUP:
	  //todo: check timer, if the button's been held too long and mouse hasn't moved, do nothing
	  if(!bDragging) {
	    dMouse = pix_to_gl(screen
			       , iMouse
			       , camera._z);

	    click_projection =  dMouse + static_cast<dPair>(camera);
	    grid.click( static_cast<int>((click_projection._x + 1) / 2)
			, static_cast<int>((click_projection._y + 1) / 2) );
	  }
	  bMouseButtonDown = false;
	  bDragging = false;
	  break;

	case SDL_KEY_DOWN:
	  /* handle key presses */
	  handleKeyPress( &event.key.keysym );
	  break;

	case SDL_QUIT:
	  /* handle quit requests */
	  done = true;
	  break;

	default:
	  break;
	}
      }

      /* draw the scene */
      if ( isActive ) {
	/* Clear The Screen And The Depth Buffer */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for(int i = 0; i < board._rows; ++i) {
	  for(int j = 0; j < board._columns; ++j) {
	    glLoadIdentity( );
	    glTranslatef( ((double)2 * i) - camera._x, ((double)2 * j) - camera._y, camera._z );

	    if( clicked._x == i && clicked._y == j ) {
	      color(simple_colors::blue);

	    else if( board.is_red(i, j) ) {
	      color(simple_colors::red);

	    else
	      color(simple_colors::green);

	    draw_square();
	  }
	}
      
	play.draw();

	/* Draw it to the screen */
	SDL_GL_SwapBuffers( );
      }

      /* I suppose we'd ask for an AI update here. */
    }

    /* clean ourselves up and exit */
    Quit( 0 );

    /* Should never get here */
    return( 0 );
}

