#include "./Grid.hpp"
#include "./Board.hpp"

using namespace std;

bool color_equal(Board::State color, Board b1, Board b2) {
  int color_num = 0;

  if ( color == 1 ) {
    color_num = 1; /* black */
  } else {
    color_num = 2; /* red */
  }

  cout << "COLOR NUM: " << color_num << endl; 

  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++) {
      iPair a(i,j);
      if (b1.at(a) == color_num) {
	if (!(b2.at(a) == color_num)) {  
	  return false;
	}
      }
    }
  }
  return true;
}

int main() {
  Board b1, b2;
  Grid g1, g2;
  iPair a(0,2);
  iPair b(2,2);

  /* -- black
   ++ red */

  --g1.at(a);
  --g1.at(b);

  --g2.at(a);
  --g2.at(b);

  b1=g1;
  b2=g2;

  cout << g1 << endl;
  cout << g2 << endl << endl;

  cout << b1 << endl;
  cout << b2 << endl << endl;

  cout << color_equal(Board::State::black, b1, b2) << endl;

  return 0;
}
