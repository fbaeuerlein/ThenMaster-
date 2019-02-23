
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <atomic>
#include <iostream>
#include <thread>
#include "include/Task.hxx"
#include <algorithm>
#include <numeric>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace ThenMaster;
int main(void)
{
    {
        auto a = Task<int const &, int const &>([](int const & a) { std::cout << "a = " << a << std::endl; return a;});
        int y = 9;
        int & x = y;
        a.launch(x);

        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

    {
        auto a = Task<int, int>([](int && a) -> int { return a;});
        a
            .then<void>([](int && a) -> void {std::cout << "a = " << a << std::endl; })
            .then<void>([](){ std::cout << "finished" << std::endl; })
            .then<int>([](){return 234; })
            .then<void>([](int && x){ std::cout << "really finished" << std::endl; });
        
        a.launch<AsyncTaskExecutor>(5);
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

    {

        auto pi = [](int n) ->double 
        {
            double sum = 0.0;
            int sign = 1;
            for (int i = 0; i < n; ++i) 
            {           
                sum += sign/(2.0*i+1.0);
                sign *= -1;
            }
            return 4.0*sum;
        };
        std::cout << std::setprecision(20);

        auto a = Task<double, int>(pi);
             a.then<void>([](double pi) {std::cout << "a: Pi: " << pi << std::endl; });
        
        auto b = Task<double, int>(pi);
             b.then<void>([](double pi) {std::cout << "b: Pi: " << pi << std::endl; });
        
        auto c = Task<void, void>([]() { std::this_thread::sleep_for(std::chrono::seconds(5)); std::cout << "c: finished\n" <<std::endl; });
             

        a.launch<AsyncTaskExecutor>(10000000);
        b.launch<AsyncTaskExecutor>(1000);
        c.launch<AsyncTaskExecutor>();

        a.get_future().get();
        b.get_future().get();
        c.get_future().get();
    }    

    // {
    //     auto t = Task<void, void>([](){ throw std::exception(); });
    //     t.then<void>([](){});
    //     t.launch();
    //     try
    //     {
    //         t.get_future().get();
    //     }
    //     catch(const std::exception& e)
    //     {
    //         std::cout << e.what() << std::endl;
    //     }
        
    // }
    return 0;
}