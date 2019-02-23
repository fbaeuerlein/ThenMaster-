
#include <queue>
#include <vector>
#include <map>

#include <gtest/gtest.h>

#include <Task.hxx>

using namespace ThenMaster;

class Identity
{
    public:
    template<typename V>
    static bool check(V && v) 
    {
        typedef typename std::decay<V>::type value_t;
        auto l = [](V && v) -> value_t { return v;};
        Task<value_t, V> task(l);
        task.launch(std::forward<V>(v));
        return task.future().get() == v;        
    }

};

TEST(Types, pod)
{
    // copy
    EXPECT_TRUE(Identity::check<bool>(true));

    EXPECT_TRUE(Identity::check<int>(5));
    EXPECT_TRUE(Identity::check<short int>(5));
    EXPECT_TRUE(Identity::check<unsigned short int>(5));
    EXPECT_TRUE(Identity::check<unsigned int>(5));
    EXPECT_TRUE(Identity::check<unsigned long int>(5));
    EXPECT_TRUE(Identity::check<long long int>(5));
    EXPECT_TRUE(Identity::check<unsigned long long int>(5));

    EXPECT_TRUE(Identity::check<wchar_t>(0xAA));
    EXPECT_TRUE(Identity::check<char>(0xAA));
    EXPECT_TRUE(Identity::check<unsigned char>(0xAA));

    EXPECT_TRUE(Identity::check<float>(0.125));
    EXPECT_TRUE(Identity::check<double>(0.125));
    EXPECT_TRUE(Identity::check<long double>(0.125));

    // cref
    EXPECT_TRUE(Identity::check<bool const &>(true));
    EXPECT_TRUE(Identity::check<int const &>(5));
    EXPECT_TRUE(Identity::check<short int const &>(5));
    EXPECT_TRUE(Identity::check<unsigned short int const &>(5));
    EXPECT_TRUE(Identity::check<unsigned int const &>(5));
    EXPECT_TRUE(Identity::check<unsigned long int const &>(5));
    EXPECT_TRUE(Identity::check<long long int const &>(5));
    EXPECT_TRUE(Identity::check<unsigned long long int const &>(5));

    EXPECT_TRUE(Identity::check<wchar_t const &>(0xAA));
    EXPECT_TRUE(Identity::check<char const &>(0xAA));
    EXPECT_TRUE(Identity::check<unsigned char const &>(0xAA));

    EXPECT_TRUE(Identity::check<float const &>(0.125));
    EXPECT_TRUE(Identity::check<double const &>(0.125));
    EXPECT_TRUE(Identity::check<long double const &>(0.125));    
}

TEST(Types, complex)
{
    EXPECT_TRUE(Identity::check<std::string>("abcd"));
    EXPECT_TRUE(Identity::check<std::vector<int>>({1, 2, 3, 4, 5, 6}));
    EXPECT_TRUE((Identity::check<std::map<std::string, int>>({ {"a", 1},{"b", 2},{"c", 3}})));
    EXPECT_TRUE(Identity::check<std::vector<int>>({1, 2, 3, 4, 5, 6}));

}

TEST(Then, concatenation)
{
    size_t thens = 10000;
    
    {
        size_t counter = 0;

        Task<void, void> a([&counter]() { counter++; });
        auto t = &a;

        for ( size_t i = 0; i < thens - 1; ++i )
        {
            t = &(t->then<void>([&counter]() { counter++; }));
        }

        a.launch();

        t->future().get(); // wait for the last one to be finished task
        EXPECT_EQ(counter, thens);
    }

    {
        Task<size_t, size_t> a([](size_t x) { return x; });
        auto t = &a;

        for ( size_t i = 0; i < thens; ++i )
        {
            t = &(t->then<size_t>([](size_t x) { return x + 1; }));
        }

        a.launch(0);

        EXPECT_EQ(t->future().get(), thens);
    }    
}

TEST(Then, Exception)
{
    // single exception
    {
        char const * ex_msg = "thrown at get";
        Task<void, void> a([ex_msg]() { throw std::runtime_error(ex_msg); });
        a.launch();

        bool caught = false;
        try
        {
            a.future().get();
        }
        catch(const std::runtime_error & e)
        {
            EXPECT_STREQ(ex_msg, e.what());
            caught = true;
        }
        
        EXPECT_TRUE(caught);
    }


}    

// check if the first thrown exception is propagated
TEST(Then, ExceptionConcatenation)
{
    size_t thens = 10000;

    char const * ex_msg = "thrown at get";
    Task<void, void> a([ex_msg]() { throw std::runtime_error(ex_msg); });
    auto t = &a;

    for ( size_t i = 0; i < thens; ++i )
    {
        t = &(t->then<void>([]() { throw std::runtime_error("other"); }));
    }

    a.launch();

    bool caught = false;
    try
    {
        t->future().get();
    }
    catch(const std::runtime_error & e)
    {
        EXPECT_STREQ(ex_msg, e.what()) << e.what();
        caught = true;
    }
    
    EXPECT_TRUE(caught);
}

