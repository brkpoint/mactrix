#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

unsigned int microsecond = 1000000;

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return max(lower, min(n, upper));
}

/*
\033[38;2;<r>;<g>;<b>m   Select RGB foreground color
\033[48;2;<r>;<g>;<b>m   Select RGB background color
*/

class coordinates {
    public:
        int x;
        int y;
        
        coordinates(int x, int y) {
            this->x = x;
            this->y = y;
        }
};

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

void printToCoords(coordinates* coords, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("\033[%d;%dH", coords->x, coords->y);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

int main() {
    cout << "\x1b[?25l" << flush; // hide cursor
    color(true, 255, 255, 255);

    system("clear");

    struct winsize w;
    int frame = 0;

    int length = 0;
    vector<coordinates*> matrixTrails;

    random_device dev;
    mt19937 rng(dev());
    
    while (true) {
        system("clear");
        ioctl(0, TIOCGWINSZ, &w);

        frame++;
        length = w.ws_row / 2;

        for (int i = 0; i < w.ws_row - 1; i++) {
            printf(string(w.ws_col, ' ').c_str());
        }

        uniform_int_distribution<mt19937::result_type> xrandom(0, w.ws_col);
        uniform_int_distribution<mt19937::result_type> yrandom(-5, 0);

        matrixTrails.push_back(new coordinates(xrandom(rng), yrandom(rng)));
        matrixTrails.push_back(new coordinates(xrandom(rng), yrandom(rng)));

        for (int i = 0; i < matrixTrails.size(); i++) {
            matrixTrails[i]->y++;

            if (matrixTrails[i]->x > w.ws_col) {
                matrixTrails.erase(matrixTrails.begin() + i);
                continue;
            }

            if (matrixTrails[i]->y - length > w.ws_row) {
                matrixTrails.erase(matrixTrails.begin() + i);
                continue;
            }

            for (int j = 0; j < length; j++) {
                coordinates* tcoords = new coordinates(matrixTrails[i]->x, matrixTrails[i]->y - j - 1);

                if (tcoords->y - 1 < 0) break;
                if (tcoords->y >= w.ws_row) continue;

                color(true, 30, 180, 60);
                //printToCoords(tcoords, "%s", "a");
                printf("\033[%d;%dH%s\n", tcoords->y, tcoords->x, "a");
            }

            if (matrixTrails[i]->y < w.ws_row) {
                color(true, 255, 255, 255);
                //printToCoords(matrixTrails[i], "%s", "a");
                printf("\033[%d;%dH%s\n", matrixTrails[i]->y, matrixTrails[i]->x, "a");
            }
        }

        usleep(90000);
    }

    system("clear");

    cout << "\033[0m"; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
