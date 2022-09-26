//server
#include "PlotLib.h"
#include <iostream>

int main()
{

    plt::Manager drawing_manager;
    drawing_manager.Init();
    std::cout << "some user calculations"<<std::endl;
    drawing_manager.StopMasterThread();
    std::cout << "another user calculations" << std::endl;
    std::cout << "end of master thread" << std::endl;
}