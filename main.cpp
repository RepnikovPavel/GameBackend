// master branch

#include <string>
#include <iostream>
#include <format>
#include <uwebsockets/App.h>
#include "DataStructures.h"
#include "GlobalVariables.h"
#include "Serv.h"

int main() {
    
    //"ws://localhost:8080" 
    //"/*"
    std::vector<std::thread*> Threads(std::thread::hardware_concurrency());
    std::transform(Threads.begin(), Threads.end(), Threads.begin(),
        [](auto* thr)
        {
            return new std::thread(
                []()
                {
                    uWS::App().ws<UserFromUEConnectionInfo>
                        (
                            "/*",
                            {
                                .open = [](auto* ws)
                                {
                                    UserFromUEConnectionInfo* data = ws->getUserData();
                                    data->user_id = gv::latest_user_id++;

                                },
                                .message = [](auto* ws, std::string_view message,uWS::OpCode OpCode)
                                {


                                }


                            }
                    ).listen(8080, [](auto* token)
                        {
                            if (token)
                            {
                                std::cout << std::format("server start and listening on port {}\n", 8080);
                            }
                            else
                            {
                                std::cout << std::format("server failed to start");

                            }

                        }
                    ).run();

                }
            );
        }
    );

    std::for_each(Threads.begin(), Threads.end(), [](auto* thr) {thr->join(); });
}
