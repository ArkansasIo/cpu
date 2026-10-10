// GunConHandler.cpp - implementation of the gun / wheel handler.
#include "Input/GunConHandler.h"
#include "common/Logging.h"

#include <sstream>

namespace ps3clone
{

    CellResult GunConHandler::Initialize()
    {
        for (auto &device : m_devices)
            device = GunConState{};
        for (bool &attached : m_attached)
            attached = false;
        LogInfo("GunConHandler", "initialised");
        return CellResult::CELL_OK;
    }

    CellResult GunConHandler::Attach(int index, GunConMode mode)
    {
        if (index < 0 || index >= kMaxDevices)
            return CellResult::CELL_EINVAL;
        m_modes[index] = mode;
        m_attached[index] = true;
        return CellResult::CELL_OK;
    }

    void GunConHandler::Poll()
    {
        // TODO: read aim point (IR) or wheel axes from the host.
    }

    const GunConState &GunConHandler::GetState(int index) const
    {
        static const GunConState empty{};
        if (index < 0 || index >= kMaxDevices)
            return empty;
        return m_devices[index];
    }

    std::string GunConHandler::Describe() const
    {
        int attached = 0;
        for (bool a : m_attached)
            attached += a ? 1 : 0;

        std::ostringstream os;
        os << "cellGunCon/cellWheel: " << attached << "/" << kMaxDevices << " device(s) attached";
        return os.str();
    }

} // namespace ps3clone
