#pragma once

#include "core/base/macro.h"
#include "core/base/non_copyable.h"
#include "function/ecs/system.h"

#include <memory>
#include <vector>

namespace Meow
{
    /**
     * @brief Engine entry.
     */
    class LIBRARY_API MeowEngine : NonCopyable
    {
    public:
        bool Init(); /**< Init engine */
        void Run();
        void ShutDown();

        static MeowEngine& GetEngine()
        {
            static MeowEngine instance;
            return instance;
        }

        void SetRunning(bool running) { m_running = running; }

    private:
        bool m_running = true;

        double m_last_time = 0.0;
        // double m_accumulator          = 0.0;
        // double m_phyics_time           = 0.0;
        // double m_phyics_fixed_delta_time = 1.0 / 60.0;
    };
} // namespace Meow