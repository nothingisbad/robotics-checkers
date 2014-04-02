
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

#include <SDL.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "./gl_display.hpp"
#include "./checkers.hpp"

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

/* function to handle key press events */
void handleKeyPress( SDL_Keysym *keysym ) {
  switch ( keysym->sym ) {
  case SDLK_ESCAPE:
    /* ESC key was pressed */
    exit( 0 );
    break;
  case SDLK_F1:
    /* F1 key was pressed
     * this toggles fullscreen mode
     */
    break;
  default:
    break;
  }
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

/* select and move a checker piece */
class MoveSelection {
  dPair _src, _dst;
  bool _selected, _move;
  Camera &_camera;

  /* Given a point in the scene (on the game-board plane) pick the
     corrosponding square on the checker's board. */
  iPair to_int(const dPair& p) const {
    double offset;
    int i, j;

    i = (p._y + _camera._y + 1) / 2;

    if(i % 2 == 1)
      offset = 2.0;
    else
      offset = 0.0;

    j = (p._x - offset + _camera._x + 1) / 4.0;

    return iPair(i,j); 
  }

public:
  MoveSelection(Camera &camera) : _selected(false) , _move(false) , _camera(camera) {}

  dPair& clicked() {
    if(_selected) {
      _selected = false;
      _move = true;
      return _dst;

    } else {
      _selected = true;
      _move = false;
      return _src;
    }
  }

  iPair src() const { return to_int(_src); }
  iPair dst() const { return to_int(_dst); }

  bool do_move() { return _move; }
  bool selected() { return _selected; }
  void reset() { _move = false; }
};

struct PlaybackControl {
  dTriplet Pos;
  bool bPlaying;
  dPair glBBox;

  PlaybackControl() : glBBox(0.01, 0.01) {
    bPlaying = true;
    Pos._z = -0.1;
  }

  bool playP() {
    return bPlaying;
  }

  void resize(const ScreenData& screen) {
    Pos = dTriplet(pix_to_gl(screen
			     , iPair(20,20)
			     , Pos._z)
		   , Pos._z);
  }

  bool point_on(dPair point) {
    dPair pos(Pos._x, Pos._y);
    std::cout << "Point " << point << " in " << pos << " +/-" << glBBox << "?" << std::endl;
    return (point > (pos - glBBox)) && (point < (pos + glBBox));
  }

  void clicked() {
    bPlaying ^= true;
  }

  void draw() {
    glLoadIdentity( );
    glTranslatef(Pos._x, Pos._y, Pos._z);

    if(bPlaying) {
      glColor3f( 0.1, 0.9, 0.2 );
    } else {
      glColor3f( 0.8, 0.2, 0.1 );
    }
    glBegin( GL_TRIANGLES ); {
      glVertex3f( -0.001, 0.001, 0 );
      glVertex3f( -0.001, -0.001 , 0);
      glVertex3f( 0.001, 0.0, 0);
    }
    glEnd();
  }
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

    /* main loop variable */
    bool done = false;

    /* used to collect events */
    SDL_Event event;


    /* whether or not the window is active */
    bool isActive = true;

    ScreenData screen;

    //mouse:
    dPair dMouse, mouseOffset;
    iPair iMouse;
    bool bMouseButtonDown = false;
    bool bDragging = false;

    SimpleTimer drag_timer( milliseconds(250) );

    /* convinient, squares are 2 units wide, so I don't have to divide to get a half way point */
    Camera camera(board._columns + 3.0
		  , board._columns + 3.0
		  , -20);
    dTriplet cameraDragStart;

    //selection
    MoveSelection selector(camera);

    //playback
    PlaybackControl play_button;
    screen.resize_callback( [&play_button](ScreenData &s) { play_button.resize(s); } );

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
	case SDL_WINDOWEVENT:
	  switch( event.window.event ) {
	    case SDL_WINDOWEVENT_HIDDEN:
	      isActive = false;
	      break;

	  case SDL_WINDOWEVENT_EXPOSED:
	    isActive = true;
	    break; 

	  case SDL_WINDOWEVENT_RESIZED:
	    screen.resize_window(event.window.data1, event.window.data2);
	    break;

	  case SDL_WINDOWEVENT_CLOSE:
	    exit(0);
	    break;
	  }


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
	    if( play_button.point_on( pix_to_gl(screen, iMouse, play_button.Pos._z) ) )
	      play_button.clicked();

	    else {
	      selector.clicked() = pix_to_gl(screen,
					     iMouse,
					     camera._z);
	    }
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
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	play_button.draw();

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
	      
	    else if( board.is(Board::Red,i, j) )
	      color(red);

	    else if( board.is(Board::Black,i, j) )
	      color(green);

	    else
	      color(black);

	    draw_square();
	  }
	}

	SDL_GL_SwapWindow(screen.window);
      }

      if( selector.do_move() ) {
	board.move( selector.src()
		    , selector.dst() );
	selector.reset();
      }

      /* I suppose we'd ask for an AI update here. */
    }

    return( 0 );
}

