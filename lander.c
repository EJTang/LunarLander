/*name:			Eddy J. Tang
 *ONE Card number:	1430371
 *Unix id: 		ejtang
 *lecture section:	A1
 *instructor's name:	Neil Burch
 *lab section: 		D04
 *TA's name:		Chao Gao
 */

#define _XOPEN_SOURCE
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/time.h>
#include "lander.h"
#include "ship.h"
#include <string.h>

#define MAXBUF 512

double gravity;
double thrust;
FILE* executable;
int running, landed, crashed,
  underLandscape = 0,
  improvement = 0,
  stablize = 0,
  delay = 10;

const char exec_name[] = "java -jar Sketchpad.jar";

struct ship ship; // our ship

struct landscape gameLandscape;

int main ( int argc, char *argv[])
{

  char *landscape = NULL;
  int c;
  FILE* landscapeFile;
  int keypress; // for ncurses
  int hasThrust = 0,
    hasGravity = 0,
    hasLandscapeFile = 0;

  struct sigaction handler; // for signal handling
  opterr = 0;
  while ((c = getopt (argc, argv, "g:f:t:i")) != -1)
    {
    switch(c)
      {
      case 'g':
	hasGravity = 1;
	gravity = atol(optarg);
	break;
      case 't':
	hasThrust = 1;
	thrust = atol(optarg);
	break;
      case 'f':
	hasLandscapeFile = 1;
	landscape = optarg;
	break;
      case 'i':
	improvement = 1;
	break;
      case '?':
	printf("bad '-%c', or good option but no value.\n",optopt);
	exit(EXIT_FAILURE);
	break;
	//default:
	//abort();
      }
    }
  if ( !hasGravity || !hasThrust || !hasLandscapeFile)
    {
      printf("need flags -g gravity, -f landscape.txt, -t thrust\n");
      exit(EXIT_FAILURE);
    }
  if ( gravity <0 || gravity >20 )
    {
      printf("gravity < 0, > 20 not allowed\n");
      exit(EXIT_FAILURE);
    }
  if ( thrust >0 || thrust <-20)
    {
      printf("thrust > 0, < -20 not allowed\n");
      exit(EXIT_FAILURE);
    }
  
  landscapeFile = fopen(landscape, "r");
  if (landscapeFile == NULL)
    {
      printf("invalid file\n");
      exit(EXIT_FAILURE);
    }
  executable = popen(exec_name, "w");
  if (executable == NULL)
    {
      printf("could not open Sketchpad.jar\n");
      exit(EXIT_FAILURE);
    }
  processLandscapeFile(landscapeFile);
  drawLandscape(executable);
  
  makeShip(&ship); // sets the points for our ship here
  // ncurses for key presses
  init_ncurses();
  
  mvprintw( 5, 10, "press any key to start.");
  refresh(); // printed output only displays if refreshed
  keypress = getch();

  nodelay( stdscr, TRUE ); // have getch() return ERR if input isn't ready

  /****************************************/
  // signal handling
  handler.sa_handler = handle_timeout;
  sigemptyset( &handler.sa_mask );
  handler.sa_flags = 0;
  if( sigaction( SIGALRM, &handler, NULL) < 0 ) 
    { exit( EXIT_FAILURE ); }
  
  struct itimerval timer;

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 50000;

  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 50000;

  if( setitimer( ITIMER_REAL, &timer, NULL) < 0 )
    { exit(EXIT_FAILURE); }


  // change what is on the screen
  erase();
  mvprintw( 5, 10, "Press left arrow key to rotate counter clockwise,");
  mvprintw( 6, 10, "press right arrow key to rotate clockwise,");
  mvprintw( 7, 10, "press space to thrust,");
  if (improvement)
    {mvprintw( 8, 10, "press 's' to activate or deactivate stablizer");
      mvprintw( 9, 10, "or press 'q' to quit, or 'r' to restart at any time." );}
  else 
    {mvprintw( 8, 10, "or press 'q' to quit." );}
  refresh();

  running = 1;
  landed = 0;
  crashed = 0;
  do
    {
      if (landed)
	{
	  ship.thrusting = 0;
	  if (improvement && ship.wasthrusting)
	    {
	      eraseShip(executable,&ship);
	      drawShip(executable,&ship);
	    }
	  if ((ship.shipAngle == 90 || ship.shipAngle == -270) &&
	      ship.velocityy < 20 && !underLandscape && !crashed &&
	      onHorizontalLine())
	    {
	      mvprintw(10, 11,"Landed!!!");
	    }
	  else
	    {
	      mvprintw(10, 11,"Crashed!!!");
	    }
	}
      keypress = getch();
      if( keypress == ERR ) { continue; } // keep looping until we get input

    // in asn3, won't need to do any printing to screen.
    // instead, will rotate figure on left or right arrow keys, and
    // initiate thrust when space bar is pressed.
      erase();
      mvprintw( 5, 10, "Press left arrow key to rotate counter clockwise,");
      mvprintw( 6, 10, "press right arrow key to rotate clockwise,");
      mvprintw( 7, 10, "press space to thrust,");
      if (improvement)
	{mvprintw( 8, 10, "press 's' to activate or deactivate stablizer");
	  mvprintw( 9, 10, "or press 'q' to quit, or 'r' to restart at any time." );}
      else 
	{mvprintw( 8, 10, "or press 'q' to quit." );}
      move( 6, 10 );
      switch ( keypress )
	{
	case KEY_LEFT:
	  eraseShip(executable, &ship);
	  rotateShip(-10, &ship);
	  break;
	case KEY_RIGHT:
	  eraseShip(executable, &ship);
	  rotateShip(10, &ship);
	  break;
	case ' ':
	  ship.thrusting = 1;
	  break;
	case 'q':
	  running = 0;
	  fprintf(executable, "end");
	  pclose(executable);
	  fclose(landscapeFile);
	  free(gameLandscape.landscapexPoints);
	  free(gameLandscape.landscapeyPoints);
	  break;
	case 'r':
	  if (improvement)
	    {
	      eraseShip(executable, &ship);
	      makeShip(&ship);
	      running = 1;
	      landed = 0;
	      crashed = 0;
	    }
	  break;
	case 's':
	  if (improvement)
	    {
	      if (stablize)
		{stablize = 0;}
	      else
		{stablize = 1;}
	    }
	default:
	  break;
	}
      refresh();
    } while( running );

  shutdown_ncurses();

  /**************************************/

  

  exit(EXIT_SUCCESS);
}

void processLandscapeFile(FILE* landscapeFile)
{
  double x,y;
  char line[MAXBUF];

  gameLandscape.maxPoints = 20;
  gameLandscape.numberOfPoints = 0;
  gameLandscape.landscapexPoints = malloc(gameLandscape.maxPoints * sizeof(double));
  gameLandscape.landscapeyPoints = malloc(gameLandscape.maxPoints * sizeof(double));

  while (fgets(line, MAXBUF, landscapeFile) != NULL)
  {
    sscanf(line, "%lf %lf", &x, &y);
    gameLandscape.landscapexPoints[gameLandscape.numberOfPoints] = x;
    gameLandscape.landscapeyPoints[gameLandscape.numberOfPoints] = y;
    gameLandscape.numberOfPoints++;
    if (gameLandscape.numberOfPoints >= gameLandscape.maxPoints)
      {
	gameLandscape.maxPoints = gameLandscape.maxPoints * 2;
	gameLandscape.landscapexPoints = realloc(gameLandscape.landscapexPoints,
					     gameLandscape.maxPoints * 
						 sizeof(double));
	gameLandscape.landscapeyPoints = realloc(gameLandscape.landscapeyPoints,
					     gameLandscape.maxPoints * 
					     sizeof(double));
      }
  }
}

void drawLandscape(FILE* executable)
{
  int i;

  for (i = 0; i < (gameLandscape.numberOfPoints-1); i++)
    {
      fprintf(executable,"drawSegment %ld %ld %ld %ld\n",
	      lround(gameLandscape.landscapexPoints[i]), 
	      lround(gameLandscape.landscapeyPoints[i]), 
	      lround(gameLandscape.landscapexPoints[i+1]), 
	      lround(gameLandscape.landscapeyPoints[i+1]));
    }
  return;
}

void handle_timeout(int signal)
{
  int i, j;
  // code for redrawing the ship
  //printf("hello\n");
  if( signal != SIGALRM ) { return; }
  double t = 0.05,
    angle_radians,
    xAcceleration = 0,
    yAcceleration = 0;
  if (!landed)
    {
  eraseShip(executable,&ship);
  ship.shipy = (ship.shipy + ship.velocityy * t + 1/2 * gravity * t*t);
  ship.velocityy = (ship.velocityy + gravity * t);
 
  for (i = 0; i < gameLandscape.numberOfPoints-1; i++)
    {
      for (j = 0; j < 3; j++)
	{
	  if (lineSegmentIntersection(lround(ship.shipxPoints[j] + ship.shipx),
				      lround(ship.shipyPoints[j] + ship.shipy),
				      lround(ship.shipxPoints[j+1] + ship.shipx), 
				      lround(ship.shipyPoints[j+1] + ship.shipy),
				      lround(gameLandscape.landscapexPoints[i]), 
				      lround(gameLandscape.landscapeyPoints[i]),
				      lround(gameLandscape.landscapexPoints[i+1]), 
				      lround(gameLandscape.landscapeyPoints[i+1])))
	    {
	      landed = 1;
	      crashed = 1;
	    }
	}
      if (lineSegmentIntersection(lround(ship.shipxPoints[0] + ship.shipx), 
				  lround(ship.shipyPoints[0] + ship.shipy),
				  lround(ship.shipxPoints[3] + ship.shipx), 
				  lround(ship.shipyPoints[3] + ship.shipy),
				  lround(gameLandscape.landscapexPoints[i]),
				  lround(gameLandscape.landscapeyPoints[i]),
				  lround(gameLandscape.landscapexPoints[i+1]),
				  lround(gameLandscape.landscapeyPoints[i+1])))
	{
	  landed = 1;
	  crashed = 1;
	}
      if (lineSegmentIntersection(lround(ship.shipThrusterPoint[0] + ship.shipx),
				  lround(ship.shipThrusterPoint[0] + ship.shipy),
				  lround(ship.shipxPoints[3] + ship.shipx),
				  lround(ship.shipyPoints[3] + ship.shipy),
				  lround(gameLandscape.landscapexPoints[i]),
				  lround(gameLandscape.landscapeyPoints[i]),
				  lround(gameLandscape.landscapexPoints[i+1]),
				  lround(gameLandscape.landscapeyPoints[i+1])))
	{
	  landed = 1;
	}
      if (lineSegmentIntersection(lround(ship.shipThrusterPoint[1] + ship.shipx),
				  lround(ship.shipThrusterPoint[1] + ship.shipy),
				  lround(ship.shipxPoints[4] + ship.shipx),
				  lround(ship.shipyPoints[4] + ship.shipy),
				  lround(gameLandscape.landscapexPoints[i]),
				  lround(gameLandscape.landscapeyPoints[i]),
				  lround(gameLandscape.landscapexPoints[i+1]),
				  lround(gameLandscape.landscapeyPoints[i+1])))
	{
	  landed = 1;
	}
    }
 
  
  if (ship.thrusting)
    {
      angle_radians = ship.shipAngle * M_PI / 180.0;
      xAcceleration = thrust * cos(angle_radians);
      yAcceleration = gravity + thrust * sin(angle_radians);
  
      ship.shipy = (ship.shipy + ship.velocityy * t + 
		    1/2 * yAcceleration * t*t);
      if (ship.shipy < 0)
	{ship.shipy = 0;}
      ship.velocityy = (ship.velocityy + yAcceleration * t);
      if (improvement)
	{
	  ship.shipx = (ship.shipx + ship.velocityx * t + 
			1/2 * xAcceleration * t*t);
	  if (ship.shipx >= 640)
	    {
	      ship.shipx -= 640;
	      if (shipUnderLandscape(1))
		{landed = 1; underLandscape = 1;}
	    }
	  else if (ship.shipx <= 0)
	    {
	      ship.shipx += 640;
	      if (shipUnderLandscape(0))
		{landed = 1; underLandscape = 1;}
	    }
	}
      else 
	{
	  ship.shipx = (ship.shipx + ship.velocityx * t + 
			1/2 * xAcceleration * t*t);
	  if (ship.shipx > 640)
	    {ship.shipx = 640;}
	  else if (ship.shipx < 0)
	    {ship.shipx = 0;}
	}
      ship.velocityx = (ship.velocityx + xAcceleration * t);
    }
  else // ship it not thrusting
    {
      ship.shipy = (ship.shipy + ship.velocityy * t + 
		    1/2 * yAcceleration * t*t);
      if (ship.shipy < 0)
	{ship.shipy = 0;}
      if (improvement)
	{
	  ship.shipx = (ship.shipx + ship.velocityx * t + 
			1/2 * xAcceleration * t*t);
	  if (ship.shipx >= 640)
	    {
	      ship.shipx -= 640;
	      if (shipUnderLandscape(1))
		{landed = 1; underLandscape = 1;}
	    }
	  else if (ship.shipx <= 0)
	    {
	      ship.shipx += 640;
	      if (shipUnderLandscape(0))
		{landed = 1; underLandscape = 1;}
	    }
	}
      else
	{
	  ship.shipx = (ship.shipx + ship.velocityx * t + 
			1/2 * xAcceleration * t*t);
	  if (ship.shipx > 640)
	    {ship.shipx = 640;}
	  else if (ship.shipx < 0)
	    {ship.shipx = 0;}
	}
    }
  if (improvement)
    {
      if (stablize)
	{
	  if (delay == 0)
	    {
	      eraseShip(executable,&ship);
	      stablizeShip(&ship); 
	      delay = 10;
	    }
	  delay -= 1;
	}
    }
 
  drawShip(executable,&ship);

  fflush(NULL);
  ship.thrusting = 0;
    }
}

// init_ncurses and shutdown_ncurses from example files
void init_ncurses()
{
  int r;

  // start up the ncurses environment
  initscr(); // nothing to check, initscr exits on error

  // don't wait for enter for keyboard input
  r = cbreak(); assert( r != ERR );

  // don't echo keypresses to screen
  r = noecho(); assert( r != ERR );

  r = nonl(); assert( r != ERR );

  // turn cursor off
  r = curs_set( 0 ); assert( r != ERR );

  // don't worry about cursor position
  r = leaveok( stdscr, TRUE ); assert( r != ERR );

  // slower handling of ^C but saner output
  r = intrflush( stdscr, FALSE ); assert( r != ERR );

  // translate movement espace codes into single keys
  r = keypad( stdscr, TRUE ); assert( r != ERR );
}

void shutdown_ncurses()
{
  endwin();
}


/**********************************/
// code for line intersect. Retrieved from 
//http://alienryderflex.com/intersect/

//  public domain function by Darel Rex Finley, 2006



//  Determines the intersection point of the line defined by points A and B with the
//  line defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.

//  public domain function by Darel Rex Finley, 2006



//  Determines the intersection point of the line segment defined by points A and B
//  with the line segment defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.

int lineSegmentIntersection(
double Ax, double Ay,
double Bx, double By,
double Cx, double Cy,
double Dx, double Dy) {

  double  distAB, theCos, theSin, newX, ABpos ;

  //  Fail if either line segment is zero-length.
  if ((Ax==Bx && Ay==By) || (Cx==Dx && Cy==Dy)) return 0;

  //  Fail if the segments share an end-point.
  if ((Ax==Cx && Ay==Cy) || (Bx==Cx && By==Cy)
      ||  (Ax==Dx && Ay==Dy) || (Bx==Dx && By==Dy)) {
    return 0; }

  //  (1) Translate the system so that point A is on the origin.
  Bx-=Ax; By-=Ay;
  Cx-=Ax; Cy-=Ay;
  Dx-=Ax; Dy-=Ay;

  //  Discover the length of segment A-B.
  distAB=sqrt(Bx*Bx+By*By);

  //  (2) Rotate the system so that point B is on the positive X axis.
  theCos=Bx/distAB;
  theSin=By/distAB;
  newX=Cx*theCos+Cy*theSin;
  Cy  =Cy*theCos-Cx*theSin; Cx=newX;
  newX=Dx*theCos+Dy*theSin;
  Dy  =Dy*theCos-Dx*theSin; Dx=newX;

  //  Fail if segment C-D doesn't cross line A-B.
  if ((Cy<0. && Dy<0.) || (Cy>=0. && Dy>=0.)) return 0;

  //  (3) Discover the position of the intersection point along line A-B.
  ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

  //  Fail if segment C-D crosses line A-B outside of segment A-B.
  if (ABpos<0. || ABpos>distAB) return 0;

  // unneeded section of he code because we do no need he point of intersection.
  //  (4) Apply the discovered position to line A-B in the original coordinate system.
  //*X=Ax+ABpos*theCos;
  //*Y=Ay+ABpos*theSin;

  //  Success.
  return 1; }

// replaced YES with 1 and NO with 0

int shipUnderLandscape(int side) // 0 will be from the left going to the right wall
{
  int i;
  if (side) // right side to left wall
    {
      for (i = 0; i < 4; i++)
	{
	  if ((ship.shipyPoints[i] + ship.shipy) >= 
	      gameLandscape.landscapeyPoints[0])
	    {return 1;}
	}
      if ((ship.shipThrusterPoint[1] + ship.shipy) >=
	  gameLandscape.landscapeyPoints[0])
	{return 1;}
    }
  else
    {
      for (i = 0; i < 4; i++)
	{
	  if ((ship.shipyPoints[i] + ship.shipy) >= 
	      gameLandscape.landscapeyPoints[gameLandscape.numberOfPoints-1])
	    {return 1;}
	}
      if ((ship.shipThrusterPoint[1] + ship.shipy) >=
	  gameLandscape.landscapeyPoints[gameLandscape.numberOfPoints-1])
	{return 1;}
	}
  return 0;
}

int onHorizontalLine(void)
{
  int i;
  for (i = 0; i < gameLandscape.numberOfPoints-1; i++)
    {
      if (ship.shipx > gameLandscape.landscapexPoints[i] &&
	  ship.shipx < gameLandscape.landscapexPoints[i+1])
	{
	  if (gameLandscape.landscapeyPoints[i] - 
	      gameLandscape.landscapeyPoints[i+1] == 0)
	    {return 1;}
	  else
	    {return 0;}
	}
    }
  return 0;
}
