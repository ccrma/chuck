#ifndef util_timer_h
#define util_timer_h

#include <chrono>

class Timer 
{
public:
    using t_Duration = std::chrono::duration<double, std::nano>;
    using t_Instant = std::chrono::time_point<std::chrono::steady_clock>;

    Timer() :
        m_elapsed(0),
        m_running(false)
    {
    }

    Timer operator+(const Timer &rhs) const
    {
        Timer t;
        t.m_elapsed = this->m_elapsed + rhs.m_elapsed;
        return t;
    }

    bool Running() { return m_running; }

    /**
     * @brief Resets the presumed-running timer. 
     * 
     * @param andStart optional convenience param to restart the timer.
     */
    void Reset(bool andStart=false) 
    {
        m_elapsed = t_Duration(0);
        if(andStart)
            this->Start();
    }

    /**
     * @brief Starts the timer.  Resumes count from last place so make
     * sure to call Reset if you'd like non-resuming behavior.
     */
    void Start() 
    { 
        m_startTime = std::chrono::steady_clock::now();
        m_running = true;
    }

    /**
     * @brief Stops the timer and updates the current elapsed value.
     *  No reset occurs so that you at resume the timer at will.
     * 
     * @return t_Duration 
     */
    t_Duration Stop() 
    {
        auto end = std::chrono::steady_clock::now();
        m_elapsed += end - m_startTime;
        m_running = false;
        return m_elapsed;
    }

    t_Duration GetElapsed() const { return m_elapsed; }

private:
    t_Duration m_elapsed;
    t_Instant m_startTime;
    bool m_running;
};

#endif 
