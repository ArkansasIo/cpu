// types.cpp - result-code stringification.
#include "common/Types.h"

namespace ps3clone
{

    const char *ToString(CellResult r)
    {
        switch (r)
        {
        case CellResult::CELL_OK:
            return "CELL_OK";
        case CellResult::CELL_EINVAL:
            return "CELL_EINVAL";
        case CellResult::CELL_ENOMEM:
            return "CELL_ENOMEM";
        case CellResult::CELL_EFAULT:
            return "CELL_EFAULT";
        case CellResult::CELL_ENOSYS:
            return "CELL_ENOSYS";
        case CellResult::NOT_IMPLEMENTED:
            return "NOT_IMPLEMENTED";
        }
        return "UNKNOWN";
    }

} // namespace ps3clone
