Repository for school project using CBCv2 controller to play checkers.

Try make test_checkers

Includes some primitives for representing and playing a checkers game, including a rudementary (3-move lookahead) AI and GUI interface.


# openGL GUI:

To build the gui (requires sdl2 and openGL):
```make gl_checkers```

There is a small red square in the upper left hand corner; that is the 'play' button.
When first launched, the gui is 'stopped' meaning the human can move any peice to any position and the AI will not respond.

Click on a piece then click on it's destination to move.

To 'play', click on upper left red square.  It should turn into a green triangle.
The human can move red pieces (the human goes first) and the machine will respond.
To capture, click on the piece that should move, then click on the peice that will be jumped.


# text UI:

To build
```make test_checkers```

The bottom left square is (0,0), there are 8 rows [0,7].  Columns count only playable squares [0,3].

* to unconditionally move a piece use the command:

  ```move <row_begin> <column_begin> <row_end> <column_begin>```

* to make a legal checkers move:
 
  ```exec <row_begin> <column_begin> <row_end> <column_begin>```

* to have the AI make one move (alternates red and black)

  ```play```

* to have the computer pick one move (from an ordered set of legal moves)

  ```next <color> <n>```

  where color is red or black and n is <= the number of moves it is possible for that side to make.

* to go back a board state 

  ``` undo ```

* to reset the board (note: reset clears the undo history)

  ``` reset ```


