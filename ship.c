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
#include "ship.h"

void drawShip (FILE* executable, struct ship* ship)
{
  int i;
  fprintf(executable, "drawSegment %ld %ld %ld %ld\n",
	  lround(ship->shipxPoints[0] + ship->shipx),
	  lround(ship->shipyPoints[0] + ship->shipy),
	  lround(ship->shipxPoints[3] + ship->shipx),
	  lround(ship->shipyPoints[3] + ship->shipy));
  for ( i = 0; i < 3; i++)
    {
      fprintf(executable, "drawSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[i] + ship->shipx),
	      lround(ship->shipyPoints[i] + ship->shipy),
	      lround(ship->shipxPoints[i+1] + ship->shipx),
	      lround(ship->shipyPoints[i+1] + ship->shipy));
    }
  if ( ship->thrusting )
    {
      fprintf(executable, "drawSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[2] + ship->shipx),
	      lround(ship->shipyPoints[2] + ship->shipy),
	      lround(ship->shipThrusterPoint[0] + ship->shipx),
	      lround(ship->shipThrusterPoint[1] + ship->shipy));
      fprintf(executable, "drawSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[3] + ship->shipx),
	      lround(ship->shipyPoints[3] + ship->shipy),
	      lround(ship->shipThrusterPoint[0] + ship->shipx),
	      lround(ship->shipThrusterPoint[1] + ship->shipy));
      ship->wasthrusting = 1;
    }
  fflush(executable);
  return;
}


void eraseShip (FILE* executable, struct ship* ship)
{
  int i;
  fprintf(executable, "eraseSegment %ld %ld %ld %ld\n",
	  lround(ship->shipxPoints[0] + ship->shipx),
	  lround(ship->shipyPoints[0] + ship->shipy),
	  lround(ship->shipxPoints[3] + ship->shipx),
	  lround(ship->shipyPoints[3] + ship->shipy));
  for ( i = 0; i < 3; i++)
    {
      fprintf(executable, "eraseSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[i] + ship->shipx),
	      lround(ship->shipyPoints[i] + ship->shipy),
	      lround(ship->shipxPoints[i+1] + ship->shipx),
	      lround(ship->shipyPoints[i+1] + ship->shipy));
    }
  if ( ship->wasthrusting )
    {
      fprintf(executable, "eraseSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[2] + ship->shipx),
	      lround(ship->shipyPoints[2] + ship->shipy),
	      lround(ship->shipThrusterPoint[0] + ship->shipx),
	      lround(ship->shipThrusterPoint[1] + ship->shipy));
      fprintf(executable, "eraseSegment %ld %ld %ld %ld\n",
	      lround(ship->shipxPoints[3] + ship->shipx),
	      lround(ship->shipyPoints[3] + ship->shipy),
	      lround(ship->shipThrusterPoint[0] + ship->shipx),
	      lround(ship->shipThrusterPoint[1] + ship->shipy));
      ship->wasthrusting = 0;
    }
  fflush(executable);
  return;
}

void rotateShip (double angle_degrees, struct ship* ship)
{
  int i;
  double tempx, tempy;
  double angle_radians;

  angle_radians = angle_degrees * M_PI / 180.0;
  for (i = 0; i < 4; i ++)
    {
      tempx = ship->shipxPoints[i];
      tempy = ship->shipyPoints[i];
      ship->shipxPoints[i] = tempx*cos(angle_radians) - 
	tempy*sin(angle_radians);
      ship->shipyPoints[i] = tempx*sin(angle_radians) + 
	tempy*cos(angle_radians);
    }
  // might have to change because points on 0 dont rotate properly
  tempx = ship->shipThrusterPoint[0];
  tempy = ship->shipThrusterPoint[1];
  ship->shipThrusterPoint[0] = tempx*cos(angle_radians) - 
    tempy*sin(angle_radians);
  ship->shipThrusterPoint[1] = tempx*sin(angle_radians) + 
    tempy*cos(angle_radians);
  ship->shipAngle = (ship->shipAngle + (int)angle_degrees) % 360;
  return;
}

void makeShip (struct ship* ship)
{
    /************************************/
  // ship to be made here
  ship->shipAngle = 90;
  ship->shipx = 300;
  ship->shipy = 50;
  ship->shipxPoints[0] = -5;
  ship->shipyPoints[0] = -10;
  ship->shipxPoints[1] = 5;
  ship->shipyPoints[1] = -10;
  ship->shipxPoints[2] = 10;
  ship->shipyPoints[2] = 10;
  ship->shipxPoints[3] = -10;
  ship->shipyPoints[3] = 10;
  ship->shipThrusterPoint[0] = 0; // x
  ship->shipThrusterPoint[1] = 15; // y
  ship->velocityy = 0;
  ship->velocityx = 0;
  return;
/***************************************/
}

void stablizeShip(struct ship*ship)
{
  if (ship->shipAngle > 90 && ship->shipAngle <= 270)
    {rotateShip(-10,ship);}
  else if (ship->shipAngle > 270 && ship->shipAngle < 360)
    {rotateShip(10,ship);}
  else if (ship->shipAngle >= 0 && ship->shipAngle <90)
    {rotateShip(10,ship);}
  else if (ship->shipAngle < 0 && ship->shipAngle >= -90)
    {rotateShip(10,ship);}
  else if (ship->shipAngle < -90 && ship->shipAngle > -270)
    {rotateShip(-10,ship);}
  else if (ship->shipAngle < -270 && ship->shipAngle > -360)
    {rotateShip(10,ship);}
}
