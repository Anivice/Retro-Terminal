#ifndef SOCKET_H
#define SOCKET_H

class stable_socket_server {
public:
    void init(int port);
    void start();
    void stop();
};

#endif //SOCKET_H
