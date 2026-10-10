// PadHandler.cpp - implementation of the DualShock / SIXAXIS handler.
#include "Input/PadHandler.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    CellResult PadHandler::Initialize()
    {
        m_pads.fill(PadState{});
        m_connected.fill(false);
        m_connected[0] = true; // pad 1 connected by default
        LogInfo("PadHandler", "initialised");
        return CellResult::CELL_OK;
    }

    void PadHandler::Poll()
    {
        // TODO: read the host keyboard / gamepad and fill m_pads.
    }

    const PadState &PadHandler::GetState(int pad) const
    {
        static const PadState empty{};
        if (pad < 0 || pad >= kMaxPads)
            return empty;
        return m_pads[static_cast<size_t>(pad)];
    }

    bool PadHandler::IsPressed(int pad, PadButton button) const
    {
        return (GetState(pad).buttons & static_cast<u32>(button)) != 0;
    }

    std::string PadHandler::Describe() const
    {
        int connected = 0;
        for (bool c : m_connected)
            connected += c ? 1 : 0;

        std::ostringstream os;
        os << "cellPad: " << connected << "/" << kMaxPads << " pads connected";
        return os.str();
    }

} // namespace ps3clone
