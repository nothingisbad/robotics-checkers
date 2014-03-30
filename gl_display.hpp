#ifndef GL_DISPLAY_HPP
#define GL_DISPLAY_HPP

/**
 * @file /home/ryan/uml/robotics/checkers/gl_display.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 29, 2014
 */

#include <iostream>
#include "./points.hpp"

class Camera;


class Camera : public dTriplet {
public:
  bool _is_absolute;
  Camera() : dTriplet(0,0,0), _is_absolute(false) {}
};

struct screen_data {
  int pixWidth, pixHeight;
  double glWidth, glHeight;
  double aspectRatio;
  constexpr static double dFOV = 0.275; //field of view angle factor

  /* so, to project a mouse click onto the game-board plane I can use the center of the screen in terms of pixles and
     center in terms of the OpenGL coordinates to calibrate my offsets */
  double dHalf_x, dHalf_y;
  int iHalf_x, iHalf_y;

  void resize_window(int width, int height) {
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

  screen_data(int width = 640, int height = 480) {
    resize_window(width, height);
  }
};

dPair pix_to_gl(const screen_data &screen
		, const iPair &pix
		, const double &depth) {
  double scale_factor = screen.dFOV * depth;
  /* I could add the arithmetic operator to the Pair objects and clean this up, but for my little project, I don't think
     it's worth the time.  */
  return dPair( (screen.dHalf_x - (((double)pix._x / (double)screen.pixWidth) * screen.glWidth) ) * scale_factor ,
		((((double)pix._y / (double)screen.pixHeight) * screen.glHeight) - screen.dHalf_y) * scale_factor );
}

iPair gl_to_pix(const screen_data &screen, const double &x, const double &y, const double &z) {
  double scale_factor = -(screen.dFOV * z);
  return iPair( (int)(x / (screen.dHalf_x * scale_factor)) * screen.iHalf_x + screen.iHalf_x
		, (int)(y / (screen.dHalf_y * scale_factor)) * screen.iHalf_y + screen.iHalf_y
		);
}

iPair gl_to_pix(const screen_data &screen
		,const dPair& point
		,const double &_z) {
  return gl_to_pix(screen, point._x, point._y, _z);
}

iPair gl_to_pix(const screen_data &screen
		, const dTriplet &point) {
  return gl_to_pix(screen, point._x, point._y, point._z);
}

#endif


