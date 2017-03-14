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

#define MAXBUF 512

struct ship{
  int shipAngle,
    thrusting,
    wasthrusting;
  double velocityx, velocityy, shipx, shipy,
    shipxPoints[4],
    shipyPoints[4],
    shipThrusterPoint[2];
};

void drawShip (FILE* executable, struct ship* ship);

void eraseShip (FILE* executable, struct ship* ship);

void rotateShip (double angle_degrees, struct ship* ship);

void makeShip (struct ship* ship);

void stablizeShip(struct ship*ship);
