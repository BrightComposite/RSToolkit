//---------------------------------------------------------------------------

#pragma once

#ifndef XPACK_H
#define XPACK_H

//---------------------------------------------------------------------------

#include <meta/types.h>

//---------------------------------------------------------------------------

namespace asd
{
#define xpack(... /*props*/) [](){ struct XPack{__VA_ARGS__;} props; return props; }()

#define xinit1(...) xpack(__VA_ARGS__)}
#define xinit(...) handle<__VA_ARGS__>{xinit1
}

//---------------------------------------------------------------------------
#endif
