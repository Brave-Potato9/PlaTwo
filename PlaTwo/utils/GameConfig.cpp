#include "GameConfig.h"

//------------------------------------ constructor ------------------------------------
GameConfig::GameConfig()
    : gameType("Unknown")
    , hasTimeLimit(false)
    , timeLimit(60)
    , serverPort(12345)
    , serverIP("127.0.0.1")
    , dotsAndBoxesRows(5)
    , dotsAndBoxesColumns(5)
    , useFlyingPhase(true)
{
    validateAndFix();
}
