#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <random>
#include <vector>

using namespace std;

void clear(winsize& w) {
    system("clear");
    for (int i = 0; i < w.ws_row - 1; i++) {
        printf("%s", string(w.ws_col, ' ').c_str());
    }    
}

int main(int argc, char* argv[]) {
    // setting up input
    system("stty raw");
    struct termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    cout << "\x1b[?25l" << flush; // hide cursor

    // terminal sizing
    struct winsize w;
    struct winsize wl;
    
    ioctl(0, TIOCGWINSZ, &w);

    clear(w);

    // speed
    int speed = 30;
    if (argc >= 2) {
        speed = stoi(argv[1]);
    }

    // trails stuff
    int length = 0;
    vector<pair<int, int> > trails;
    vector<char> lastchars;

    // rng
    random_device dev;
    mt19937 rng(dev());

    printf("\033[38;2;%d;%d;%dm", 255, 255, 255); // color to white
    
    while (true) {
        ioctl(0, TIOCGWINSZ, &w);

        // size changed
        if (w.ws_col != wl.ws_col || w.ws_row != wl.ws_row) {
            clear(w);

            trails.clear();
            lastchars.clear();

            wl = w;
            ioctl(0, TIOCGWINSZ, &w);

            length = w.ws_row / 2;
            continue;
        }

        if (getchar() == 'q') break;

        // gen new numbers
        uniform_int_distribution<mt19937::result_type> xrandom(0, w.ws_col);
        uniform_int_distribution<mt19937::result_type> yrandom(-length + 2, 0);

        // push first random trail
        trails.push_back(make_pair(xrandom(rng), yrandom(rng)));
        lastchars.push_back(' ');
        //push second random trail
        trails.push_back(make_pair(xrandom(rng), yrandom(rng)));
        lastchars.push_back(' ');

        for (int i = 0; i < trails.size(); i++) {
            // if the trail is out of bounds, delete it
            if (trails[i].second - length > w.ws_row) {
                trails.erase(trails.begin() + i);
                continue;
            }

            // clear last char if it was inbounds
            if (trails[i].second - length - 1 > 0)
                printf("\033[%d;%dH%c\n", trails[i].second - length - 1, trails[i].first, ' ');

            // change the color of the last char to green if its inbounds
            if (trails[i].second - 1 > 0 && trails[i].second - 1 < w.ws_row) {
                printf("\033[38;2;%d;%d;%dm", 30, 180, 60); // color to green
                printf("\033[%d;%dH%c\n", trails[i].second - 1, trails[i].first, lastchars[i]); // we need to print it again
            }

            // print new char if its inbounds
            if (trails[i].second < w.ws_row && trails[i].second > 0) {
                char current = (char)rand() % 26 + 97; // gen random char
                printf("\033[38;2;%d;%d;%dm", 255, 255, 255); // color to white
                printf("\033[%d;%dH%c\n", trails[i].second, trails[i].first, current);
                lastchars[i] = current;
            }

            trails[i].second++;
        }

        wl = w;

        usleep(100000 - speed * 1000); // frame timing
    }

    // restoring input
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    system("stty cooked");

    system("clear");

    cout << "\033[0m" << flush; // reset colors
    cout << "\x1b[?25h" << flush; // show cursor
    return 0;
}
