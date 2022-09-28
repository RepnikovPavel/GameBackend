#include "Connection.h"
#include <iostream>
int main()
{
    ConnectionsManagerInTheCurrentThread connections_manager;
    connections_manager.StartListenServer();
    std::vector<double> x = { 1,2,3,4,5 };
    std::vector<double> y = { 6,7,8,9,10 };
    for (size_t i = 0; i < 1000; i++)
    {
        connections_manager.PushDrawRequest(x, y);
    }
    connections_manager.WaitUntilAllRenderingRequestsAreSent();
}