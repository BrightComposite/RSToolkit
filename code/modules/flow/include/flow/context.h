//---------------------------------------------------------------------------

#pragma once

#ifndef FLOW_CONTEXT_H
#define FLOW_CONTEXT_H

//---------------------------------------------------------------------------

#include <boost/asio/io_context.hpp>

//---------------------------------------------------------------------------

namespace asd
{
    namespace flow
    {
        class context
        {
        public:
            boost::asio::io_context & io() {
                return _io;
            }

            size_t run_io() {
                return _io.run();
            }

        private:
            boost::asio::io_context _io;
        };
    }
}

//---------------------------------------------------------------------------
#endif
