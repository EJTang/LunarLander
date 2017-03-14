/*name:			Eddy J. Tang
 *ONE Card number:	1430371
 *Unix id: 		ejtang
 *lecture section:	A1
 *instructor's name:	Neil Burch
 *lab section: 		D04
 *TA's name:		Chao Gao
 */

struct landscape {
  double *landscapexPoints;
  double *landscapeyPoints;
  int numberOfPoints,
    maxPoints;
};

void drawLandscape(FILE* executable);
// for signal
void handle_timeout(int signal);
// for ncurses
void init_ncurses();
void shutdown_ncurses();

void processLandscapeFile(FILE* landscapeFile);

// function obtained from http://alienryderflex.com/intersect/
int lineSegmentIntersection(
double Ax, double Ay,
double Bx, double By,
double Cx, double Cy,
double Dx, double Dy);

int shipUnderLandscape(int side);

int onHorizontalLine(void);
