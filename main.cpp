#include <iostream>
#include <cstdlib>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <vector>

using namespace std;

unsigned int microsecond = 1000000;

/*
\033[38;2;<r>;<g>;<b>m   Select RGB foreground color
\033[48;2;<r>;<g>;<b>m   Select RGB background color
*/

class trail {
    public:
        int x;
        int y;
        vector<char> chl;
        
        trail(int x, int y) {
            this->x = x;
            this->y = y;
        }
};

template <typename T> T clip(const T& n, const T& lower, const T& upper) {
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

void clear(winsize& w) {
    system("clear");
    for (int i = 0; i < w.ws_row - 1; i++) {
        printf("%s", string(w.ws_col, ' ').c_str());
    }    
}

int main() {
    cout << "\x1b[?25l" << flush; // hide cursor
    color(true, 255, 255, 255);

    struct winsize w;
    struct winsize wl;
    int frame = 0;

    clear(w);

    int length = 0;
    vector<trail*> trails;

    random_device dev;
    mt19937 rng(dev());
    
    while (true) {
        ioctl(0, TIOCGWINSZ, &w);

        if (w.ws_col != wl.ws_col || w.ws_row != wl.ws_row) {
            clear(w);
            trails.clear();
            wl = w;
            continue;
        }

        frame++;
        length = w.ws_row / 2;

        uniform_int_distribution<mt19937::result_type> xrandom(0, w.ws_col);
        uniform_int_distribution<mt19937::result_type> yrandom(-length + 2, 0);

        trails.push_back(new trail(xrandom(rng), yrandom(rng)));
        trails.push_back(new trail(xrandom(rng), yrandom(rng)));

        for (int i = 0; i < trails.size(); i++) {
            char current = (char)rand() % 26 + 97;

            if (trails[i]->chl.size() > length)
                trails[i]->chl.erase(trails[i]->chl.begin() + length, trails[i]->chl.end());

            if (trails[i]->x > w.ws_col) {
                trails.erase(trails.begin() + i);
                continue;
            }

            if (trails[i]->y - length - 1 > 0 && trails[i]->y - length - 1 < w.ws_row)
                printf("\033[%d;%dH%c\n", trails[i]->y - length - 1, trails[i]->x, ' ');

            if (trails[i]->y - length > w.ws_row) {
                trails.erase(trails.begin() + i);
                continue;
            }

            color(true, 30, 180, 60);
            for (int j = 0; j < length; j++) {
                if (trails[i]->y - j - 1 < 0) break;
                if (trails[i]->y - j - 1 >= w.ws_row) continue;

                printf("\033[%d;%dH%c\n", trails[i]->y - j - 1, trails[i]->x, trails[i]->chl.at(j));
            }
            
            if (trails[i]->y < w.ws_row && trails[i]->y > 0) {
                color(true, 255, 255, 255);
                printf("\033[%d;%dH%c\n", trails[i]->y, trails[i]->x, current);
            }

            trails[i]->y++;
            trails[i]->chl.insert(trails[i]->chl.begin(), current);
        }

        wl = w;
        usleep(70000);
    }

    system("clear");

    cout << "\033[0m"; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
