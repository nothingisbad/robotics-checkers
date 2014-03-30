
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
inline void draw_triangle(const dTriplet& tri_color, const dTriplet& pos, const dPair& bbox) {
  glLoadIdentity( );

  glTranslatef(pos._x, pos._y, pos._z);
  color(tri_color);

  // draw_square();
  glBegin( GL_TRIANGLES ); {
    glVertex3f( -bbox._x, bbox._y, 0.0f );
    glVertex3f( -bbox._x, -bbox._y , 0.0f);
    glVertex3f( bbox._x, 0.0, 0.0f);
  }
  glEnd( );
} 

class MoveSelection {
  dPair _src, _dst;
  bool _selected, _move;
  Camera &_camera;


  iPair to_int(const dPair& p) const {
    double x, y, offset = 2.0;
    cout << "to_int from " << p << endl;
    cout << "  camera: " << _camera << endl;

    /* this is stupid, but I'm tired of thinking.
       I should be able to just divide, but it's not working so great. */
    for(int i = 0; i < Board::_rows; ++i) {
      for(int j = 0; j < Board::_columns; ++j) {

	if(i % 2 == 0)
	  offset = 0.0;
	else
	  offset = 2.0;

	x = ((double)4 * j) + offset - _camera._x;
	y = ((double)2 * i) - _camera._y;

	if((p > dPair(x - 1, y -1)) && (p < dPair(x + 1, y + 1))) {
	  cout << "to_int: " << iPair(i,j) << endl;
	  return iPair(i,j);
	}
      }
    }
    cout << "to_int can't find an in range match: " << endl;
    return iPair(-1,-1);
  }
public:
  MoveSelection(Camera &camera) : _selected(false) , _move(false) , _camera(camera) {}

  dPair& clicked() {
    if(_selected) {
      cout << "Moving" << endl;
      _selected = false;
      _move = true;
      return _dst;

    } else {
      cout << "Selecting" << endl;
      _selected = true;
      _move = false;
      return _src;
    }
  }

  iPair src() const { return to_int(_src); }
  iPair dst() const { return to_int(_dst); }
  /* dPair src() const { return _src; } */
  /* dPair dst() const { return _dst; } */

  bool do_move() { return _move; }
  bool selected() { return _selected; }
  void reset() { _move = false; }
};

/**************************/
/*  __  __       _        */
/* |  \/  | __ _(_)_ __   */
/* | |\/| |/ _` | | '_ \  */
/* | |  | | (_| | | | | | */
/* |_|  |_|\__,_|_|_| |_| */
/**************************/
int main( int argc, char **argv ) {
    //game grid and logic
    Board board;

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


    dTriplet grid_center;

    Camera camera;
    dTriplet cameraDragStart;

    //selection
    MoveSelection selector(camera);


    initSDL( videoInfo, screen, videoFlags );

    /* initialize OpenGL */
    initGL( );

    /* resize the initial window */
    screen.resize_window(screen.pixWidth, screen.pixHeight );

    //setup the camera
    grid_center._x = (double)(board._columns / 2) - 1.0;
    grid_center._y = (double)(board._columns / 2) - 1.0;
    grid_center._z = 0;

    camera._x = grid_center._x;
    camera._y = grid_center._y;
    camera._z = -20;

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
	    selector.clicked()
	      = pix_to_gl(screen
			  , iMouse
			  , camera._z);
	    
	  }
	  bMouseButtonDown = false;
	  bDragging = false;
	  break;

	case SDL_KEYDOWN:
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

      /*******************/
      /* Draw the scence */
      /*******************/
      if ( isActive ) {
	double offset, x, y;
	dPair p;

	/* Clear The Screen And The Depth Buffer */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for(int i = 0; i < board._rows; ++i) {

	  /* offset the squares to make the checkerboard pattern */
	  if( i % 2 == 0)
	    offset = 0.0;
	  else
	    offset = 2.0;

	  for(int j = 0; j < board._columns; ++j) {
	    using namespace simple_colors;

	    glLoadIdentity( );

	    x = ((double)4 * j) + offset - camera._x;
	    y = ((double)2 * i) - camera._y;

	    glTranslatef( x , y, camera._z );

	    if( selector.selected() && (iPair(i,j) == selector.src() ) )
	      color(blue);
	      
	    else if( board.is_red(i, j) )
	      color(red);

	    else if( board.is_black(i, j) )
	      color(green);

	    else
	      color(black);

	    draw_square();
	  }
	}
	
	/* Draw it to the screen */
	SDL_GL_SwapBuffers( );
      }

      if( selector.do_move() ) {
	board.move( selector.src()
		    , selector.dst() );
	selector.reset();
      }
	

      /* I suppose we'd ask for an AI update here. */
    }

    /* clean ourselves up and exit */
    quit( 0 );

    /* Should never get here */
    return( 0 );
}

