#include "AppContext.h"

#include "system/Log.h"

#include <cstdlib>
#include <ctime>

constexpr auto LOG_TAG = "init";


bool AppContext::init()
{
    try {
        Log::info(LOG_TAG) << "Initializing video...\n";
        m_window = Window::create();

        std::srand(std::time(nullptr));
    }
    catch (const std::exception& err) {
        Window::showErrorMessage(err.what());
        return false;
    }

    return true;
}
