#include "level_system.h"

namespace Meow
{
    void LevelSystem::Start()
    {
        m_levels.insert({"Default", std::make_shared<Level>()});
        m_current_active_level = m_levels["Default"];
    }

    void LevelSystem::Tick(float dt)
    {
        if (std::shared_ptr<Level> level = m_current_active_level.lock())
        {
            level->Tick(dt);
        }
    }
} // namespace Meow