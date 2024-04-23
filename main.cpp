#include <iostream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

unsigned int microsecond = 1000000;

/*
\033[38;2;<r>;<g>;<b>m   Select RGB foreground color
\033[48;2;<r>;<g>;<b>m   Select RGB background color
*/

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return max(lower, min(n, upper));
}

void color(bool fob, int r, int g, int b) {
    clip(r, 0, 255);
    clip(g, 0, 255);
    clip(b, 0, 255);

    if (fob) {
        printf("\033[38;2;%d;%d;%dm", r, g, b);
        return;
    }

    printf("\033[48;2;%d;%d;%dm", r, g, b);
}

int main() {
    cout << "\x1b[?25l" << flush; // hide cursor
    color(true, 20, 180, 60);

    system("clear");

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    int pos = 0;
    int length = 5;

    while (true) {
        system("clear");
        ioctl(0, TIOCGWINSZ, &w);

        pos++;
        if (pos > 0) {
            for (int i = 0; i < length; i++) {
                if (pos - i < 0) break;
                if (pos - i >= w.ws_row - 1) continue;

                color(true, 30 - i, 255 - i, 60 - i);
                printf("\033[%d;%dH%s\n", pos - i, 1, "a");
            }

            if (pos < w.ws_row - 1) {
                color(true, 255, 255, 255);
                printf("\033[%d;%dH%s\n", pos, 1, "a");
            }
        }

        if (pos - length >= w.ws_row - 1) {
            pos = 0;
        }

        usleep(150000);
    }

    system("clear");

    cout << "\033[0m"; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
