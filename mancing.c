#include <curses.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>

const int optrow = 96;
const int optcol = 355;
int bkgcolor = 2;

void mvprtch(int row, int col, int color, char c){
	attron(COLOR_PAIR(color));
	mvaddch(row, col, c);
	attroff(COLOR_PAIR(color));
}

void mvprtstr(int row, int col, int color, const char *c){	
	attron(COLOR_PAIR(color));
	mvprintw(row, col, "%s", c);
	attroff(COLOR_PAIR(color));
}

int findcenterpos(const char *s){
	int x;
	for(x = 0; s[x] != '\0'; x++);
	return x;
}

void forcefullscreen(int *irow, int *icol){
	int midx = (*icol)/2 , midy = (*irow)/2;
	
	while(*irow < optrow || *icol < optcol){
		clear();
		char str[50] = "Please full screen this window !!";
		mvprtstr(midy-1, midx-findcenterpos(str)/2, 2, str);
		strcpy(str, "and then press ctrl and \"+\" to adjust the minimum size.");
		mvprtstr(midy, midx-findcenterpos(str)/2, 2, str);
		sprintf(str, "Row: %d, Col: %d", *irow, *icol);
		mvprtstr(midy+1, midx-findcenterpos(str)/2, 1, str);		
		refresh();
	//	napms(500); // napms is delay function that takes in arguments in millisecond
		getmaxyx(stdscr, *irow, *icol);
		midx = (*icol)/2;
		midy = (*irow)/2;
		resize_term(0, 0);
	}
	resize_term(0, 0);
	*irow = optrow;
	*icol = optcol;
	erase();
	refresh();
}

void drawgraphic(const char *fname, int cordy, int cordx, int color){
	FILE *f = fopen(fname, "r");
	char c;
	int x = cordx, y = cordy, toggle = 0;
	int oob = 0;
	
	while((c = fgetc(f)) != EOF){
		if (c == 'ë' && toggle == 0 && !oob) {
            attroff(COLOR_PAIR(bkgcolor));
            attron(COLOR_PAIR(color));
            toggle = 1;
        } else if (c == 'ë' && toggle == 1 && !oob) {
            attroff(COLOR_PAIR(color));
            attron(COLOR_PAIR(bkgcolor));
            toggle = 0;
        } else if (c == '\n') {
            y++;
            x = cordx;
            oob = 0;
        } else if (!oob){
        	if(x > optcol-2 || x < 1 || y > optrow-2 || y < 1){
        		if(toggle){
	        		toggle = 0;
				}
				oob = 1;
			} else if(c != ' '){
        		mvaddch(y, x, c);
        		x++;
			} else {
        		x++;
			}
		}
		refresh();
	} 
	
	attroff(COLOR_PAIR(color));
	attron(COLOR_PAIR(bkgcolor));
	
	fclose(f);
}

void upres(char *buf, int *maxr, int *maxc){
	getmaxyx(stdscr, maxr, maxc);
	sprintf(buf, "Row = %d, Col = %d", maxr, maxc);
	mvprtstr(0, 0, 1, buf);
}

void enclear(){
	erase();
	refresh();
}

void strgame(int irow, int icol){
//	refresh();
	char buf[30];
	int maxr, maxc;
	char chr = '\0';
	
	int sim1 = 330, sim2 =330; 

	do {
		// drawing the map
		for(int i =0; i < irow; i++){
			for(int j = 0; j< icol; j++){
				if(i == 0 || i == irow-1){
					mvprtch(i,j, 1, '#');
				} else if(j == 0 || j == icol-1){
					mvprtch(i,j, 1, '#');
				} else {
					mvprtch(i,j, 2, ' ');
				}
			}
		}
		
		upres(buf, &maxr, &maxc);
		drawgraphic("Simulation.txt", 4, sim1, 2);
		drawgraphic("Sim2.txt", 24, sim2, 1);
		sim1+=1; sim2+=1;
		if(sim1 > optcol-1){
			sim1 = 330;
		}
		
		if(sim2 > optcol-1){
			sim2 = 330;
		}
		
		refresh();
		napms(50);
		chr = wgetch(stdscr);
		// clear entire screen (artifact left behind)
		enclear();
	} while(chr != 'q');

	
	
}


int main(int argc, char *argv[]){
	
	int irow = 0, icol = 0;
	//	int widths = GetSystemMetrics(SM_CXSCREEN); // this is how to get the screen size in pixel unit
	//  int heights = GetSystemMetrics(SM_CYSCREEN);

	
// initialization of window state
	initscr(); // initialize the window screen
	start_color(); // start the color function 
	keypad(stdscr, TRUE); // allows the use of other characters such as key_up and key_down
	noecho(); //hide the echo (hide the character that is getting inputed form the console)
	curs_set(0); // set the cursor invisible
	getmaxyx(stdscr, irow, icol); // get the screensize
	
// setting the pair color
	init_pair(1, COLOR_RED, COLOR_BLUE); // initialize the color pair (letter and background)
	init_pair(2, 0, COLOR_WHITE);

// force full screen
	forcefullscreen(&irow, &icol);
	
	strgame(irow, icol);
	
	
	endwin();
	return 0;
}
