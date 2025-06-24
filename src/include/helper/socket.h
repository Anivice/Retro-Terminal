#ifndef SOCKET_H
#define SOCKET_H

#include "WorkerThread.h"

class stable_socket_server {
private:
    void worker_thread();

public:
    void init(int port);
    void start();
    void stop();
};

#endif //SOCKET_H
