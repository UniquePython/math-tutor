#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <pcgrnglib.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>


#define CENTERY (LINES/2)
#define CENTERX(msg) ((COLS - strlen(msg))/2)


void init_ncurses(void);
void greet(const char *name);
const char *get_system_username(void);
const char *random_name(void);


int main(void)
{
    init_ncurses();

    const char *name = get_system_username();
    if (!name) name = random_name();

    greet(name);

    const char *msg = "Welcome to your MATH CLASS!";
    mvprintw(CENTERY, CENTERX(msg), "%s", msg);

	const char *msg1 = "Press any key to continue...";
	mvprintw((CENTERY) + 2, CENTERX(msg1), "%s", msg1);

    refresh();
    getch();

	RNG rng;
	pcgrng_auto(&rng);

	int n = 1;
	int ne = 0;
	int nes = 0;
	int nm = 0;
	int nms = 0;
	int nh = 0;
	int nhs = 0;

	while (1)
	{
		erase();

		const char *format = "EASY = %d/%d, MEDIUM = %d/%d, HARD = %d/%d";
		int length = snprintf(NULL, 0, format, nes, ne, nms, nm, nhs, nh);
		char *str0 = malloc(length + 1);
		snprintf(str0, length + 1, format, nes, ne, nms, nm, nhs, nh);
		attron(COLOR_PAIR(4));
		mvprintw(LINES * 3 / 4, CENTERX(str0), "%s", str0);
		attroff(COLOR_PAIR(4));
		refresh();
		free(str0);

		int op = pcgrng_randrange64(&rng, 0, 2);
		int diff = pcgrng_randrange64(&rng, 0, 2);

		uint64_t left, right, res;

		if (diff == 0) {
			left = pcgrng_randrange64(&rng, 0, 100);
			right = pcgrng_randrange64(&rng, 0, 100-1);
			ne++;
		} else if (diff == 1) {
			left = pcgrng_randrange64(&rng, 100, 1000);
			right = pcgrng_randrange64(&rng, 100, 1000-1);
			nm++;
		} else {
			left = pcgrng_randrange64(&rng, 1000, 10000);
			right = pcgrng_randrange64(&rng, 1000, 10000-1);
			nh++;
		}

		char out[64];

		if (op == 0)
		{
			res = left + right;
			snprintf(out, sizeof(out),  "%" PRIu64 " + " "%" PRIu64 " = ", left, right);
		}
		else if (op == 1)
		{
			if (right > left)
			{
				uint64_t tmp = left;
				left = right;
				right = tmp;
			}
			res = left - right;
			snprintf(out, sizeof(out),  "%" PRIu64 " - " "%" PRIu64 " = ", left, right);
		}
		else
		{
			if (diff == 0) {
				right = pcgrng_randrange64(&rng, 0, 10);
			}
			else if (diff == 1) {
				uint64_t factors[] = {2, 3, 5, 10};
				right = factors[pcgrng_randrange64(&rng, 0, 3)];
			}
			else {
				uint64_t factors[] = {2, 3, 5, 7, 11, 13};
				right = factors[pcgrng_randrange64(&rng, 0, 5)];
			}

			res = left * right;

			snprintf(out, sizeof(out),  "%" PRIu64 " * " "%" PRIu64 " = ", left, right);
		}

		const char *fmt = "=== Question %d (%s) ===";
		const char *lvl;
		if (diff == 0)      lvl = "EASY";
		else if (diff == 1) lvl = "MEDIUM";
		else                lvl = "HARD";
		int len = snprintf(NULL, 0, fmt, n, lvl);
    	char *str = malloc(len + 1);
		snprintf(str, len + 1, fmt, n++, lvl);
		attron(COLOR_PAIR(3));
		mvprintw(CENTERY - 2, CENTERX(str), "%s", str);
		attroff(COLOR_PAIR(3));
		free(str);

		attron(COLOR_PAIR(1));
		mvprintw(CENTERY, CENTERX(out), "%s", out);
		time_t start = time(NULL);
		attroff(COLOR_PAIR(1));
		move(CENTERY, CENTERX(out) + strlen(out));
		refresh();

		char buf[32];

		echo();
		getnstr(buf, sizeof(buf) - 1);
		noecho();

		if (buf[0] == 'q') break;
		if (buf[0] == '\0')
		{
			const char *fmt = "WRONG! Correct answer is %" PRIu64;
			int len = snprintf(NULL, 0, fmt, res);
			char *str = malloc(len + 1);
			snprintf(str, len + 1, fmt, res);
			attron(COLOR_PAIR(2));
			mvprintw(CENTERY + 2, CENTERX(str), "%s", str);
			attroff(COLOR_PAIR(2));
			refresh();
			getch();
			free(str);
			continue;
		}

		char *end;
		uint64_t answer = strtoull(buf, &end, 10);

		if (*end != '\0') continue;
		if (answer == res)
		{
			time_t now = time(NULL);

			if (diff == 0)      nes++;
			else if (diff == 1) nms++;
			else                nhs++;

			int seconds = now - start;
			const char *fmt2 = "Time taken: %02d:%02d";
			int len2 = snprintf(NULL, 0, fmt2, seconds / 60, seconds % 60);
			char *str2 = malloc(len2 + 1);
			snprintf(str2, len2 + 1, fmt2, seconds / 60, seconds % 60);
			attron(COLOR_PAIR(4));
			mvprintw(LINES * 5 / 8, CENTERX(str2), "%s", str2);
			attroff(COLOR_PAIR(4));
			refresh();
			napms(1000);
			free(str2);
		}
		else
		{
			const char *fmt = "WRONG! Correct answer is %" PRIu64;
			int len = snprintf(NULL, 0, fmt, res);
			char *str = malloc(len + 1);
			snprintf(str, len + 1, fmt, res);
			attron(COLOR_PAIR(2));
			mvprintw(CENTERY + 2, CENTERX(str), "%s", str);
			attroff(COLOR_PAIR(2));
			refresh();
			getch();
			free(str);
			continue;
		}

		refresh();
	}

    endwin();

    return 0;
}


void init_ncurses(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
	start_color();
	use_default_colors();
	init_pair(1, COLOR_GREEN, -1);
	init_pair(2, COLOR_RED, -1);
	init_pair(3, COLOR_YELLOW, -1);
	init_pair(4, COLOR_CYAN, -1);
}


void greet(const char *name)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    const char *fmt;

    if      (t->tm_hour >= 5  && t->tm_hour < 11) fmt = "Good Morning %s!";
    else if (t->tm_hour >= 11 && t->tm_hour < 16) fmt = "Good Afternoon %s!";
    else if (t->tm_hour >= 16 && t->tm_hour < 20) fmt = "Good Evening %s!";
    else                                          fmt = "Hey there %s!";

    int len = snprintf(NULL, 0, fmt, name);
    char *str = malloc(len + 1);

    snprintf(str, len + 1, fmt, name);
    mvprintw((CENTERY) - 2, CENTERX(str), "%s", str);

    free(str);
}


const char *get_system_username(void)
{
    const char *username = getlogin();
    if (username) return username;

    struct passwd *pw = getpwuid(getuid());
    if (pw) return pw->pw_name;

    return NULL;
}


const char *random_name(void)
{
    static const char *names[] = {
        "Friend",
        "Scholar",
        "Genius",
        "Maverick",
        "Einstein"
    };

    RNG rng;
    pcgrng_auto(&rng);

    return pcgrng_choice(&rng, names, sizeof(names) / sizeof(names[0]), sizeof(char *));
}
