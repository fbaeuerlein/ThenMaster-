
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
int main(void)
{

    {
        auto a = Task<int, int>([](int && a) -> int { return a;});
        auto b = a.then<void>([](int a) -> void {std::cout << "a = " << a << std::endl; });
        auto x = a.get_future();
        a.execute(5);
        //std::cout << x.get() << std::endl;
    }

    {
        auto a = Task<float, float>([](float const & a) -> float { return a; });
        // auto x = a.get_future();
        // a(5.);
        // x.get();
    }    
    // a.then<double>([](float const & a) -> double { return double(a); })
    //  .then<int>([](double const & a) -> int { std::cout << a << std::endl; return 0; });

    // a(4, 3, 17);
    // auto a = Task<float, int, int, int>([](int const & a, int b, int const c) -> float { return float(a * b + c); });
    
    // a.then<double>([](float const & a) -> double { return double(a); })
    //  .then<int>([](double const & a) -> int { std::cout << a << std::endl; return 0; });

    // a(4, 3, 17);

    // typedef std::vector<float> vec_t;

    // auto b = Task<vec_t, int>([](int x){ return vec_t(x, 1); });
    // auto c = b.then<vec_t>([](vec_t v) -> vec_t { std::transform(v.begin(), v.end(), v.begin(), [](float d) -> float { return d * 2;}); return v;})
    //  .then<float>([](vec_t v) -> float { return std::accumulate(v.begin(), v.end(), 0.); })
    //  .then<int>([](float x) { std::cout << "x = " << x << std::endl; return x;});

    // b(1000);

    // std::cout << "c = " << c.get_future().get() << std::endl;

    // auto x = Task<void, void>([](){});
    // auto y = x.then<int>([]() -> int{ return 0; });
    // auto z = y.then<void>([](int x) { std::cout << "YAY!" << x << std::endl;});
    // x();

    return 0;
}