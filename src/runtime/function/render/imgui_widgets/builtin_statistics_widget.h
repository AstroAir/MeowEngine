#pragma once

#include "function/render/structs/builtin_render_stat.h"

#include <cstdint>
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Meow
{
    class BuiltinStatisticsWidget
    {
    public:
        BuiltinStatisticsWidget();

        void Draw(const std::unordered_map<std::string, BuiltinRenderStat>& stat);

    private:
        static void DrawFrameStat(const BuiltinRenderStat& stat);
        static void DrawVertexAttributesStat(const BuiltinRenderStat& stat);
        static void DrawBufferStat(const BuiltinRenderStat& stat);
        static void DrawImageStat(const BuiltinRenderStat& stat);
        void        DrawRingBufferStat(const std::string& pass_name, const RingUniformBufferStat& stat);

        static constexpr int    k_gredint_count     = 20;
        static constexpr double k_gredint_partition = 1.0 / 20.0;

        std::vector<ImU32> m_col_base_table;
        std::vector<ImU32> m_col_hovered_table;

        std::unordered_map<std::string, bool>                  m_is_ringbuf_stat_shapshot_enabled;
        std::unordered_map<std::string, RingUniformBufferStat> m_ringbuf_stat_snapshot;

        static constexpr ImU32 col_outline = 0xFFFFFFFF;
    };
} // namespace Meow