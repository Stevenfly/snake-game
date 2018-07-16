/*
CS 349 A1 Skeleton Code - Snake

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o snake snake.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./snake

Note: the -L option and -lstdc++ may not be needed on some machines.
*/

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <deque>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
const int width = 800;
const int height = 600;

const int grid_size = 10;

const int initial_x = 100;
const int initial_y = 80;

const int defaultFPS = 30;
const int defaultSpeed = 5;

const int dir_up = 1;
const int dir_down = -1;
const int dir_left = -2;
const int dir_right = 2;

const int gameover_obstacle = 1;
const int gameover_eat = 2;

const int move_distance = 1;
const int growth_multiplier = 3;

const string str_game_name = "SNAKE";
const string str_name = "Yufei Yang";
const string str_userid = "20610147";
const string str_description_1 = "How to play:";
const string str_description_2 = "Use \"WASD\" to control direction.";
const string str_description_3 = "Enhancements: Creative level design";
const string str_description_4 = "The snake can wrap around to other side of borders; There are obstacles in the shape of \"CS\".";
const string str_description_5 = "Press any key to start.";

const string str_score = "Score: ";
const string str_fps = "FPS: ";
const string str_speed = "Speed: ";

const string str_gameover = "GAME OVER";
const string str_gameover_obstacle = "You hit an obstacle! Press \"r\" to restart the game; Press \"q\" to quit.";
const string str_gameover_eat = "You ate yourself! Press \"r\" to restart the game; Press \"q\" to quit.";
const string str_paused = "Game Paused";
const string str_paused_status = "Press \"p\" again to resume.";
const string str_moving = "Go snake!";
const string str_moving_status = "Press \"p\" to pause the game; Press \"r\" to restart the level.";

int FPS = defaultFPS;
int snakeSpeed = defaultSpeed;

bool isGameStarted = false;

bool debug = false;
bool cheat = false;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
	int		 width;		// size of window
	int		 height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}

/*
 * Function reads a int direction outputs string direction
 */
string printDirection(int direction) {
	if (direction == dir_up) return "Up";
	else if (direction == dir_down) return "Down";
	else if (direction == dir_left) return "Left";
	else if (direction == dir_right) return "Right";
	else return "N/A";
}

int translateSpeed(int speed) {
	return 80 + speed * 10;
}

/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};       

class Fruit : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			if (isGameStarted) XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x + 1, y + 1, blockSize, blockSize);
        }

        Fruit() {
            // ** ADD YOUR LOGIC **
            // generate the x and y value for the fruit 
            generate();
            blockSize = 8;
        }

        int getX() {
        	return x;
        }

        int getY() {
        	return y;
        }

        void generate(int snake_x, int snake_y) {
        	// prevent re-generate on the snake
        	do {
        		generate();
        	} while (x == snake_x && y == snake_y);
        }

        void generate(deque< pair<int, int> > &tail) {
        	// prevent re-generate on the snake
        	while (true) {
        		generate();
	        	for (pair<int, int> p: tail) {
	        		if (x == p.first && y == p.second) {
	        			continue;
	        		}
				}
				break;
			}
        }

        void generate() {
        	x = ( rand() % 79 + 1 ) * 10;
        	y = ( rand() % 39 + 1 ) * 10;

        	// prevent re-generate on the obstacles
        	if (x > 190 && x < 360 && ( (y > 90 && y < 110) || (y > 290 && y < 310))) {
        		generate();
        	}
        	if (y > 90 && y < 310 && x > 190 && x < 210) {
        		generate();
        	}

        	if (x > 440 && x < 590 && ( (y > 90 && y < 110) || (y > 190 && y < 210) || (y > 290 && y < 310))) {
        		generate();
        	}
        	if (y > 90 && y < 210 && x > 440 && x < 460) {
        		generate();
        	}
        	if (y > 190 && y < 310 && x > 590 && x < 610) {
        		generate();
        	}
        }

    private:
        int x;
        int y;
        int blockSize;
};

// class Obstacle : public Displayable {

// }

class Snake : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			if (isGameStarted) {
				XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x + 1, y + 1, blockSize, blockSize);

				for (pair<int, int> p: tail) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], p.first + 1, p.second + 1, blockSize, blockSize);
				}

				// Obstacles

				for (int i = 200; i <= 350; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], i, 100, 10, 10);
				}
				for (int i = 200; i <= 350; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], i, 300, 10, 10);
				}
				for (int i = 100; i <= 300; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 200, i, 10, 10);
				}

				for (int i = 450; i <= 600; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], i, 100, 10, 10);
				}
				for (int i = 450; i <= 600; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], i, 200, 10, 10);
				}
				for (int i = 450; i <= 600; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], i, 300, 10, 10);
				}
				for (int i = 100; i <= 200; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 450, i, 10, 10);
				}
				for (int i = 200; i <= 300; i+=10) {
					XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 600, i, 10, 10);
				}
			}
		}
		
		void move(Fruit &fruit) {
			if (curDirection != newDirection && x % grid_size == 0 && y % grid_size == 0) {
				curDirection = newDirection;
			}

			switch( curDirection ) {
				case dir_up:
					y -= move_distance;
					break;
				case dir_down:
					y += move_distance;
					break;
				case dir_left:
					x -= move_distance;
					break;
				case dir_right:
					x += move_distance;
					break;
			}

			if (x == fruit.getX() && y == fruit.getY()) {
        		didEatFruit();
        		fruit.generate(tail);
        	}

        	if (x > 190 && x < 360 && ( (y > 90 && y < 110) || (y > 290 && y < 310))) {
        		didHitObstacle();
        	}
        	if (y > 90 && y < 310 && x > 190 && x < 210) {
        		didHitObstacle();
        	}

        	if (x > 440 && x < 590 && ( (y > 90 && y < 110) || (y > 190 && y < 210) || (y > 290 && y < 310))) {
        		didHitObstacle();
        	}
        	if (y > 90 && y < 210 && x > 440 && x < 460) {
        		didHitObstacle();
        	}
        	if (y > 190 && y < 310 && x > 590 && x < 610) {
        		didHitObstacle();
        	}

			// Enhancments:
			if (x < 9) x = width - 19;
			if (y < 9) y = height - 219;
			if (x > width - 19) x = 9;
			if (y > height - 219) y = 9;

            // This solves a bug that shows snake eat itdelf when adding tail
			if (selfEatDisabled){
				selfEatDisabled = false;
			} else {
				for (pair<int, int> p: tail) {
					if (p.first == x && p.second == y) didEatItself();
				}
			}

			tail.push_back(make_pair(x, y));
			if (tail.size() > 0) tail.pop_front();

            // This controls the speed of the snake
            usleep(1000000/translateSpeed(snakeSpeed));
		}
		
		int getX() {
			return x;
		}
		
		int getY() {
			return y;
		}

		int getDirection() {
			return curDirection;
		}

		int getLength() {
			return length;
		}

		int getScore() {
			return score;
		}

		bool isPaused() {
			return paused;
		}

		bool isGameOver() {
			return (gameover > 0);
		}

		int getGameOverType() {
			return gameover;
		}

		deque< pair<int, int> > getTail() {
			return tail;
		}

        void didEatFruit() {
        	addTail();
        }

        void didEatItself() {
        	gameover = gameover_eat;
        }

        void didHitObstacle() {
        	gameover = gameover_obstacle;
        }

        void changeDirection(int direction) {
        	if (!paused && (length == 1 || abs(curDirection) != abs(direction))) {
        		newDirection = direction;
        	}
        }

        void addTail() {
        	selfEatDisabled = true;
        	for (int i = 0; i < grid_size * growth_multiplier; i++) tail.push_back(make_pair(x, y));
        	length += growth_multiplier;
        	score += 10;
        }

        void pause() {
        	paused = true;
        }

        void resume() {
        	paused = false;
        }

        void initialize() {
        	x = initial_x;
        	y = initial_y;
        	curDirection = dir_right;
			newDirection = dir_right;
			length = 1;
			score = 0;
            paused = !isGameStarted;
            selfEatDisabled = false;
            gameover = 0;
            tail.clear();
        }
		
		Snake() {
            blockSize = 8;
            initialize();
		}
	
	private:
		int x;
		int y;
		int blockSize;

		int curDirection;
		int newDirection;

		int length;
		int score;

		bool paused;
		bool selfEatDisabled;

		int gameover;

		deque< pair<int, int> > tail;
};


class Information : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			if (isGameStarted) {
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 700, 440,
					str_score.c_str(), str_score.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 750, 440,
					to_string(score).c_str(), to_string(score).length());

				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 440,
					status1.c_str(), status1.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 460,
					status2.c_str(), status2.length());
			} else {
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 385, 120,
				str_game_name.c_str(), str_game_name.length());

				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 370, 200,
				str_name.c_str(), str_name.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 376, 220,
				str_userid.c_str(), str_userid.length());

				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 368, 240,
				str_description_1.c_str(), str_description_1.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 310, 260,
				str_description_2.c_str(), str_description_2.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 300, 280,
				str_description_3.c_str(), str_description_3.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 140, 300,
				str_description_4.c_str(), str_description_4.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 338, 320,
				str_description_5.c_str(), str_description_5.length());
			}

			XDrawRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 9, 9, width - 19, height - 219);

			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 700, 540,
			str_fps.c_str(), str_fps.length());
			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 750, 540,
				to_string(FPS).c_str(), to_string(FPS).length());

			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 700, 560,
				str_speed.c_str(), str_speed.length());
			XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 750, 560,
				to_string(snakeSpeed).c_str(), to_string(snakeSpeed).length());

			if (debug) {
				string str_direction = "Direction: " + printDirection(direction);
				string str_length = "Length: " + to_string(length);
				string str_snake_position = "Snake Position: " + to_string(snake_x) + ", " + to_string(snake_y);
				string str_fruit_position = "Fruit Position: " + to_string(fruit_x) + ", " + to_string(fruit_y);

				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 500,
					str_direction.c_str(), str_direction.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 520,
					str_length.c_str(), str_length.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 540,
					str_snake_position.c_str(), str_snake_position.length());
				XDrawImageString(xinfo.display, xinfo.window, xinfo.gc[0], 40, 560,
					str_fruit_position.c_str(), str_fruit_position.length());
			}
        }

		Information() {
			score = 0;

			direction = 0;
			length = 0;
			snake_x = 0;
			snake_y = 0;
			fruit_x = 0;
			fruit_y = 0;
		}

		void updateInfo(Snake &snake, Fruit &fruit) {
			score = snake.getScore();

			if (snake.isGameOver()) {
				status1 = str_gameover;
				if (snake.getGameOverType() == gameover_obstacle) {
					status2 = str_gameover_obstacle;
				} else if (snake.getGameOverType() == gameover_eat) {
					status2 = str_gameover_eat;
				}
			} else if (snake.isPaused()) {
				status1 = str_paused;
				status2 = str_paused_status;
			} else {
				status1 = str_moving;
				status2 = str_moving_status;
			}

			direction = snake.getDirection();
			length = snake.getLength();
			snake_x = snake.getX();
			snake_y = snake.getY();
			fruit_x = fruit.getX();
			fruit_y = fruit.getY();
		}

	private:
		int score;
		string status1;
		string status2;

		int direction;
		int length;
		int snake_x;
		int snake_y;
		int fruit_x;
		int fruit_y;
};


list<Displayable *> dList;           // list of Displayables
Snake snake;
Fruit fruit;
Information info;

void restartGame() {
	snake.initialize();
	fruit.generate(snake.getX(), snake.getY());
}

/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"snake",			// window's title
		"Snake",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	// XSelectInput(xInfo.display, xInfo.window, 
	// 	ButtonPressMask | KeyPressMask | 
	// 	PointerMotionMask | 
	// 	EnterWindowMask | LeaveWindowMask |
	// 	StructureNotifyMask);  // for resize events

    XSelectInput(xInfo.display, xInfo.window,
        ButtonPressMask | KeyPressMask );

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	XClearWindow( xinfo.display, xinfo.window );
	
	// get height and width of window (might have changed since last repaint)

	// XWindowAttributes windowInfo;
	// XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	// unsigned int height = windowInfo.height;
	// unsigned int width = windowInfo.width;

	// big black rectangle to clear background
    
	// draw display list
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}

	XFlush( xinfo.display );
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		// printf("Got key press -- %c\n", text[0]);

		if (text[0] == 'q') {
			error("Terminating normally.");
		} else if (text[0] == 'w') {
			snake.changeDirection(dir_up);
		} else if (text[0] == 's') {
			snake.changeDirection(dir_down);
		} else if (text[0] == 'a') {
			snake.changeDirection(dir_left);
		} else if (text[0] == 'd') {
			snake.changeDirection(dir_right);
		} else if (text[0] == 'r') {
			restartGame();
		} else if (text[0] == 't') {
			if (cheat) snake.addTail();
		} else if (text[0] == 'g') {
			if (cheat) fruit.generate();
		} else if (text[0] == 'p') {
			if (!snake.isGameOver()) {
				if (snake.isPaused()) snake.resume();
				else snake.pause();
			}
		}

		if (!isGameStarted) {
			isGameStarted = true;
			snake.resume();
		}
	}
}

void handleAnimation(XInfo &xinfo, int inside) {
	if (!snake.isPaused() && !snake.isGameOver()) snake.move(fruit);
	info.updateInfo(snake, fruit);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
    dList.push_front(&info);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );

			// cout << "event.type=" << event.type << "\n";

			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		}

		handleAnimation(xinfo, inside);

		unsigned long end = now();	// get time in microsecond

		if (end - lastRepaint > 1000000 / FPS) {
			repaint(xinfo);

			lastRepaint = now(); // remember when the paint happened
		}
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

	if (argc == 3) {
		istringstream ss1(argv[1]);
		istringstream ss2(argv[2]);

		ss1 >> FPS;
		ss2 >> snakeSpeed;
	}

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
