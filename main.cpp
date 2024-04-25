#include <iostream>
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

int main() {
    cout << "\x1b[?25l" << flush; // hide cursor
    color(true, 255, 255, 255);

    system("clear");

    struct winsize w;

    // int pos = 0;
    int length = 0;
    vector<coordinates*> matrixTrails;
    matrixTrails.push_back(new coordinates(1, 0));

    while (true) {
        system("clear");
        ioctl(0, TIOCGWINSZ, &w);

        length = w.ws_row / 2;

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

            if (matrixTrails[i]->y < w.ws_row) {
                color(true, 255, 255, 255);
                printf("\033[%d;%dH%s\n", matrixTrails[i]->y, matrixTrails[i]->x, "a");
            }

            for (int j = 0; j < length; j++) {
                if (matrixTrails[i]->y - j < 0) break;
                if (matrixTrails[i]->y - j >= w.ws_row) continue;

                color(true, 30, 180, 60);
                printf("\033[%d;%dH%s\n", matrixTrails[i]->y - j - 1, matrixTrails[i]->x, "a");
            }
        }

        usleep(90000);
    }

    system("clear");

    cout << "\033[0m"; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
