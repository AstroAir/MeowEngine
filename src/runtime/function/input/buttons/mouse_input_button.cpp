#include "mouse_input_button.h"

#include "function/global/runtime_global_context.h"

namespace Meow
{
    MouseInputButton::MouseInputButton(MouseButtonCode button)
        : m_button(button)
    {
        g_runtime_global_context.window_system->GetCurrentFocusWindow()->OnMouseButton().connect(
            [this](MouseButtonCode button, InputAction action, uint8_t mods) {
                if (m_button == button)
                {
                    m_on_button(action, mods);
                }
            });
    }

    InputAction MouseInputButton::GetAction() const
    {
        return g_runtime_global_context.window_system->GetCurrentFocusWindow()->GetMouseButtonAction(m_button);
    }
} // namespace Meow
