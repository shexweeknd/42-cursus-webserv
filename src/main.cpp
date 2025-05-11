#include "webserv.hpp"

int main(void)
{
    std::cout << "\e[32mMaster process \e[0m(" << getpid() << ")\e[32m is starting...\e[0m" << std::endl;

    Server  server;

    if (server.serverFd == -1) {
        std::cerr << "\e[31mError creating server socket: \e[0m" << strerror(errno) << std::endl;
        return (1);
    }
    for (int i = 0; i < WORKERS; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "\e[31mError forking process: \e[0m" << strerror(errno) << std::endl;
            return (1);
        } else if (pid == 0) {
            work(server);
            exit(0);
        }
    }
    while (waitpid(-1, NULL, 0) > 0 || errno != ECHILD);

    std::cout << "\e[32mMaster process \e[0m(" << getpid() << ")\e[32m is terminating...\e[0m" << std::endl;
    return (0);
}