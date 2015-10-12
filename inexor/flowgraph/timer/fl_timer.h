/// @file fl_timer.h
/// @author Johannes Schneider
/// @brief Timers will execute code in a very certain interval of {n} miliseconds/seconds/minutes/hours.
/// 
/// .-------------------------------------------------------------------------------------------------.
/// | Timer                                                                                           |
/// |_________________________________________________________________________________________________|
/// |                                                                                                 |
/// | A timer is a type of nodes which (beside events) can actually be the entry point of execution.  |
/// | Timers will be called every [clock_interval] miliseconds/seconds/minutes/hours and run code.    |
/// | Timers have the following members and methods will will be displayed by the node renderer:      |
/// |                                                                                                 |
/// | interval     the frequency with which this node will execute code                               |
/// | startdelay   the amount of time which has to pass until the first call will be done.            |
/// |              the start delay begins with the end of the script's loading process.               |
/// | counter      how many times has this node already been called (write protected).                |
/// | limiter      the maximum amount of executions until the cooldown lock will be activated.        |
/// | cooldown     the amount of time which has to pass when counter exceeds the limiter.             |
/// ._________________________________________________________________________________________________.
/// 

// Include guard
#ifndef INEXOR_VSCRIPT_TIMER_HEADER
#define INEXOR_VSCRIPT_TIMER_HEADER

#include "inexor/engine/engine.h"

// Inexor namespace protection
namespace inexor {
namespace vscript {

/// TODO: undefine once debugging is finished
#define INEXOR_VSCRIPT_DEBUG_TIMERS

/// minimum time interval (in miliseconds)
#define INEXOR_VSCRIPT_MIN_TIMER_INTERVAL 10

/// one whole day is the maximum delay (in miliseconds)
#define INEXOR_VSCRIPT_MAX_TIMER_INTERVAL 1000 * 60 * 60 * 24

/// time format enumerations
enum INEXOR_VSCRIPT_TIMER_FORMAT
{
    TIMER_FORMAT_MILISECONDS,
    TIMER_FORMAT_SECONDS,
    TIMER_FORMAT_MINUTES,
    TIMER_FORMAT_HOURS
};


/// @brief Implementation of timer nodes.
class timer_node : public script_node
{
    public:
    // please note: there is no standard constructor!

    /// TODO: pass name as parameter
    /// overloaded constructor for dynamic allocation
    timer_node(vec pos, unsigned int interval, unsigned int startdelay,    unsigned int cooldown = 0, INEXOR_VSCRIPT_TIMER_FORMAT format = TIMER_FORMAT_MILISECONDS)
    {
        /// convert the interval into miliseconds
        switch(format)
        {
            case TIMER_FORMAT_HOURS:
                interval *= 1000 * 60 *60;
                break;
            case TIMER_FORMAT_MINUTES:
                interval *= 1000 * 60;
                break;
            case TIMER_FORMAT_SECONDS:
                interval *= 1000;
                break;
            case TIMER_FORMAT_MILISECONDS: 
                /// this is the format we want: convert nothing!
                break;
        }

        position = pos;

        // cut the timer interval
        clamp(interval, INEXOR_VSCRIPT_MIN_TIMER_INTERVAL, INEXOR_VSCRIPT_MAX_TIMER_INTERVAL);
        timer_interval = interval;
        timer_startdelay = startdelay;
        timer_cooldown = cooldown;
        last_time = SDL_GetTicks();
        timer_counter = 0;
        node_name = "timer";
        type = NODE_TYPE_TIMER;
    }

    /// TODO: is a destructor required?
    ~timer_node() {}

    /// timer related data
    /// time data in miliseconds
    unsigned int timer_startdelay;
    unsigned int timer_counter;
    unsigned int timer_interval;
    unsigned int timer_cooldown;
    unsigned int last_time;

    /// incoming node signal
    void in()
    {
        run();
    }

    /// implementation of the run method
    /// which was declared as virtual in script_node
    void run()
    {
        /// set color to light blue during execution
        gle::color(vec::hexcolor(0xFFBA00));
        check_if_execution_is_due();
    }
    

    private:

    /// check the timer status
    /// do we need to call the trigger?
    void check_if_execution_is_due() 
    {
        #ifdef INEXOR_VSCRIPT_DEBUG_TIMERS
            conoutf(CON_DEBUG, "checking if code execution is due..");
        #endif

        /// calculate the amount of time that passed away since last check
        unsigned int this_time = SDL_GetTicks();
        if(this_time - last_time >= timer_interval) 
        {
            /// trigger event
            out();
            /// save the timestamp
            last_time = SDL_GetTicks();
            /// increment activation counter
            timer_counter++;
        }

        /// Display a yellow color effect that is 200 ms long
        if(this_time - last_time < 200) 
            gle::color(vec::hexcolor(0xFFBA00));
    }

    /// notify child nodes
    void out()
    {
        #ifdef INEXOR_VSCRIPT_DEBUG_TIMERS
            conoutf(CON_DEBUG, "trigger # %d", timer_counter);
        #endif

        for(unsigned int i = 0; i < outgoing.size(); i++) 
        {
            /// run child node's code
            outgoing[i]->in();
        }
    }
};

// end of namespace
};
};

#endif
