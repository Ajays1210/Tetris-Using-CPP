#include "app/Application.h"

#include "core/Constants.h"
#include "platform/Console.h"
#include "platform/Keyboard.h"

namespace tetris {

Application::Application() {
    platform::SetupConsole();
    platform::ClearScreen();  // One-time wipe at startup.
}

void Application::Run() {
    while (true) {
        game_.HandleCommand(platform::ReadCommand());
        game_.Update();
        renderer_.Draw(game_);

        platform::SleepMs(kFrameDelayMs);
    }
}

}  // namespace tetris
