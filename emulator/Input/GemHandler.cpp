// GemHandler.cpp - implementation of the PlayStation Move handler.
#include "Input/GemHandler.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    CellResult GemHandler::Initialize()
    {
        for (auto &gem : m_gems)
            gem = GemState{};
        m_camera_index = -1;
        LogInfo("GemHandler", "initialised");
        return CellResult::CELL_OK;
    }

    CellResult GemHandler::BindCamera(int camera_index)
    {
        m_camera_index = camera_index;
        return CellResult::CELL_OK;
    }

    void GemHandler::Poll()
    {
        // TODO: capture a frame, track the sphere, update m_gems.
    }

    const GemState &GemHandler::GetState(int gem_index) const
    {
        static const GemState empty{};
        if (gem_index < 0 || gem_index >= kMaxGems)
            return empty;
        return m_gems[gem_index];
    }

    std::string GemHandler::Describe() const
    {
        int tracked = 0;
        for (const auto &gem : m_gems)
            tracked += gem.tracked ? 1 : 0;

        std::ostringstream os;
        os << "cellGem: " << tracked << " controller(s) tracked";
        if (m_camera_index >= 0)
            os << ", camera " << m_camera_index;
        return os.str();
    }

} // namespace ps3clone
