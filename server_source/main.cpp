//server
#include <iostream>
#include <vector>
#include "PlotLib.h"
int main()
{

    plt::Manager drawing_manager;
    drawing_manager.Init();
    std::cout << "some user calculations"<<std::endl;
    drawing_manager.Plot(std::vector<double>(5), std::vector<double>(5));
    drawing_manager.StopCurrentThread();
    std::cout << "another user calculations" << std::endl;
    std::cout << "end of master thread" << std::endl;
    
}