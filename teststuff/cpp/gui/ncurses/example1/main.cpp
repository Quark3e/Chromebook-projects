
#include <curses.h>
#include <ncurses.h>
#include <menu.h>
#include <cstdlib>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = {
	"Choice 1",
	"Choice 2",
	"Choice 3",
	"Choice 4",
	"Exit",
};

char *descriptions[sizeof(choices)/sizeof(choices[0])] = {
	"  desc 1 ",
	"  desc 2 ",
	"  desc 3 ",
	"  desc 4 ",
	""
};

int main()
{	ITEM **my_items;
	int c;				
	MENU *my_menu;
	int n_choices, i;
	ITEM *cur_item;
	
	
	initscr();
	cbreak();
	noecho(); //prevents letter from appearing on screen
	keypad(stdscr, TRUE);
	
	n_choices = ARRAY_SIZE(choices);
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

	for(i = 0; i < n_choices; ++i)
	        my_items[i] = new_item(choices[i], descriptions[i]);
	my_items[n_choices] = (ITEM *)NULL;

	my_menu = new_menu((ITEM **)my_items);
	mvprintw(LINES - 2, 0, "\"a\" to Exit");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != 'a')
	{   switch(c)
	    {	case KEY_DOWN:
		        menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
		}
	}	

	free_item(my_items[0]);
	free_item(my_items[1]);
	free_menu(my_menu);
	endwin();
}