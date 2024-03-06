#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

    int signal = atoi(argv[1]);
    int pid = atoi(argv[2]);

    kill(pid, signal);

    return 0;

}