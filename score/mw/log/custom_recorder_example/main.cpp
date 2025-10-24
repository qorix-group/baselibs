#include "score/mw/log/logging.h"

#include <iostream>

int main(void)
{
    score::mw::log::LogInfo("TEST") << "welcome to use mw::log";
}
