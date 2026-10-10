// RSXCommandBuffer.cpp - GCM method stream decoding.
//
// Methods are bucketed into branches (engine blocks). Execution walks the
// stream, classifies each method, charges it to its branch and reports the
// first malformed command it meets.
#include "RSX/RSXCommandBuffer.h"

#include "common/Logging.h"

namespace ps3clone
{

    namespace
    {
        // The FIFO ranges the real RSX documents. Everything outside these
        // falls into the Unknown subclass.
        struct MethodRange
        {
            u32 first;
            u32 last;
            RSXCommandBuffer::Subclass subclass;
            const char *branch;
        };

        constexpr MethodRange kRanges[] = {
            {0x0000, 0x00FF, RSXCommandBuffer::Subclass::Report, "report"},
            {0x0100, 0x01FF, RSXCommandBuffer::Subclass::Surface, "surface"},
            {0x0200, 0x02FF, RSXCommandBuffer::Subclass::Texture, "texture"},
            {0x0300, 0x03FF, RSXCommandBuffer::Subclass::Vertex, "vertex"},
            {0x0400, 0x04FF, RSXCommandBuffer::Subclass::Shader, "shader"},
            {0x0500, 0x05FF, RSXCommandBuffer::Subclass::Blend, "blend"},
            {0x0600, 0x06FF, RSXCommandBuffer::Subclass::DepthStencil, "depth_stencil"},
            {0x1800, 0x1FFF, RSXCommandBuffer::Subclass::Draw, "draw"},
        };
    } // namespace

    RSXCommandBuffer::RSXCommandBuffer() { BuildBranches(); }
    RSXCommandBuffer::~RSXCommandBuffer() = default;

    RSXCommandBuffer::Subclass RSXCommandBuffer::ClassifyMethod(u32 method)
    {
        for (const MethodRange &r : kRanges)
        {
            if (method >= r.first && method <= r.last)
            {
                return r.subclass;
            }
        }
        return Subclass::Unknown;
    }

    const char *RSXCommandBuffer::SubclassName(Subclass subclass)
    {
        switch (subclass)
        {
        case Subclass::Surface:
            return "surface";
        case Subclass::Texture:
            return "texture";
        case Subclass::Vertex:
            return "vertex";
        case Subclass::Draw:
            return "draw";
        case Subclass::Shader:
            return "shader";
        case Subclass::Blend:
            return "blend";
        case Subclass::DepthStencil:
            return "depth_stencil";
        case Subclass::Report:
            return "report";
        case Subclass::Unknown:
            break;
        }
        return "unknown";
    }

    void RSXCommandBuffer::BuildBranches()
    {
        m_branches.clear();
        for (const MethodRange &r : kRanges)
        {
            m_branches.push_back(GCMBranch{r.branch, {r.first, r.last}});
        }
    }

    void RSXCommandBuffer::Push(u32 method, u32 value, const std::string &name)
    {
        GCMCommand cmd;
        cmd.method = method;
        cmd.value = value;
        cmd.name = name.empty() ? SubclassName(ClassifyMethod(method)) : name;
        m_commands.push_back(cmd);
        m_dirty = true;
    }

    CellResult RSXCommandBuffer::Execute(const GCMCommand &command)
    {
        // The real command processor writes the method into the register file
        // and triggers the matching engine. Here we validate and account for
        // it; the backend (Vulkan/GL) consumes the resulting state later.
        if (command.count == 0)
        {
            LogError("RSX.FIFO", "malformed method count=0 at 0x" + std::to_string(command.method));
            return CellResult::CELL_EINVAL;
        }
        return CellResult::CELL_OK;
    }

    CellResult RSXCommandBuffer::ExecuteAll()
    {
        while (m_cursor < m_commands.size())
        {
            const CellResult rc = Execute(m_commands[m_cursor]);
            if (rc != CellResult::CELL_OK)
            {
                return rc;
            }
            ++m_cursor;
            ++m_executed;
        }
        return CellResult::CELL_OK;
    }

    size_t RSXCommandBuffer::ExecuteSome(size_t max_commands)
    {
        size_t ran = 0;
        while (ran < max_commands && m_cursor < m_commands.size())
        {
            if (Execute(m_commands[m_cursor]) != CellResult::CELL_OK)
            {
                break;
            }
            ++m_cursor;
            ++m_executed;
            ++ran;
        }
        return ran;
    }

    void RSXCommandBuffer::Reset()
    {
        m_commands.clear();
        m_cursor = 0;
        m_executed = 0;
        m_dirty = true;
    }

    std::string RSXCommandBuffer::Describe() const
    {
        return "RSXCommandBuffer{pending=" + std::to_string(m_commands.size()) +
               ", executed=" + std::to_string(m_executed) +
               ", branches=" + std::to_string(m_branches.size()) +
               ", subclasses=" + std::to_string(sizeof(kRanges) / sizeof(kRanges[0])) + "}";
    }

} // namespace ps3clone
