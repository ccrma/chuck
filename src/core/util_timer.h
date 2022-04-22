#ifndef util_timer_h
#define util_timer_h

#include <chrono>

class Timer 
{
public:
    using t_Duration = std::chrono::duration<double, std::nano>;

    Timer() :
        m_elapsed(0)
    {
    }

    Timer operator+(const Timer &rhs)
    {
        Timer temp;
        return temp;
    }

    void Reset() 
    {
        m_elapsed = t_Duration(0);
    }

    void Start() 
    { 
    }

    void Stop() 
    {
    }

    t_Duration GetElapsed() const;

private:
    t_Duration m_elapsed;
};

#endif 
