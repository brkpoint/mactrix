#include <iostream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"      
#define BOLDBLUE    "\033[1m\033[34m"    
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

int main() {
    cout << GREEN;

    system("clear");

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    while (true) {
        system("clear");

        for (int j = 0; j < w.ws_row; j++) {
            for (int i = 0; i < w.ws_col; i++) {
                cout << "a";
            }
            cout << "\n";
        }
    }

    system("clear");

    cout << RESET;
    return 0;
}
