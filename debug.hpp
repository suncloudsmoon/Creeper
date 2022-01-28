#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <cassert>

#ifdef NDEBUG
#define RELEASE
#else
#define DEBUG
#include <iostream>
#endif

#endif /* DEBUG_HPP */