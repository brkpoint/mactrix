#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
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

char inpchar() {
    char ch;
    struct termios oldt, newt;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch;
}

void color(bool fob, int r, int g, int b) {
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

int main(int argc, char* argv[]) {
    system("stty raw");
    cout << "\x1b[?25l" << flush; // hide cursor
    color(true, 255, 255, 255);

    struct winsize w;
    struct winsize wl;
    clear(w);
    int frame = 0;

    int speed = 30;

    if (argc >= 2) {
        speed = stoi(argv[1]);
    }

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

        if (inpchar() == 'q') break;

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

            if (trails[i]->y - length > w.ws_row) {
                trails.erase(trails.begin() + i);
                continue;
            }

            if (trails[i]->y - length - 1 > 0)
                printf("\033[%d;%dH%c\n", trails[i]->y - length - 1, trails[i]->x, ' ');

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
        usleep(100000 - speed * 1000);
    }

    system("clear");
    system("stty cooked");
    cout << "\033[0m" << flush; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
