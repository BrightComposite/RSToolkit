//---------------------------------------------------------------------------

#include <application/starter.h>
#include <flow/timer.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
    class Printer
    {
    public:
        Printer(flow::context & flow) : timer(flow, 1s) {
            timer.bind(make_method(this, print));
            timer.start();
        }

        void print() {
            std::cout << count << std::endl;
            ++count;
        }

    private:
        flow::timer timer;
        int count = 0;
    };

    static entrance open([]() {
        flow::context flow; 

        Printer printer(flow);

        flow.run_io();
    });
}

//---------------------------------------------------------------------------
