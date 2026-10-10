// RSXCommandBuffer.h - the GCM command stream the RSX decodes.
//
// Games never talk to the GPU directly: they build a buffer of GCM commands
// (every cellGcm* call lands here) and then kick the RSX. The command
// processor walks that stream one method at a time, exactly like the real
// hardware walks its FIFO.
//
// 2026 design note: the stream is modelled as a class / subclass / branch /
// node tree of *methods*. A command is a node, a register range is a branch,
// and a whole engine block (surface, texture, draw, ...) is a subclass.
#pragma once

#include "common/Types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ps3clone
{

    // One decoded GCM command ("node"): a method id plus its argument.
    struct GCMCommand
    {
        u32 method = 0;   // register / method id, e.g. 0x1810 = surface pitch
        u32 value = 0;    // the argument the game wrote
        u32 count = 1;    // > 1 => array form (count additional args)
        std::string name; // human-readable method name
    };

    // A branch of methods that belong to one engine sub-block.
    struct GCMBranch
    {
        std::string name;         // e.g. "surface", "texture", "draw"
        std::vector<u32> methods; // method ids owned by this branch
    };

    class RSXCommandBuffer
    {
    public:
        // Methods are grouped into these subclasses (mirrors the RSX FIFO).
        enum class Subclass
        {
            Surface,
            Texture,
            Vertex,
            Draw,
            Shader,
            Blend,
            DepthStencil,
            Report,
            Unknown
        };

        RSXCommandBuffer();
        ~RSXCommandBuffer();

        // Append a raw method/value pair to the stream (a leaf node).
        void Push(u32 method, u32 value, const std::string &name = {});

        // Decode and execute every queued command. CELL_OK, or an error if a
        // command is malformed.
        CellResult ExecuteAll();

        // Execute at most `max_commands`; returns how many actually ran.
        size_t ExecuteSome(size_t max_commands);

        // The branch (subclass) a method id belongs to.
        static Subclass ClassifyMethod(u32 method);
        static const char *SubclassName(Subclass subclass);

        void Reset();

        size_t PendingCount() const { return m_commands.size(); }
        size_t ExecutedCount() const { return m_executed; }

        const std::vector<GCMCommand> &Commands() const { return m_commands; }
        const std::vector<GCMBranch> &Branches() const { return m_branches; }

        std::string Describe() const;

    private:
        void BuildBranches();
        CellResult Execute(const GCMCommand &command);

        std::vector<GCMCommand> m_commands;
        std::vector<GCMBranch> m_branches;
        size_t m_cursor = 0;
        size_t m_executed = 0;
        bool m_dirty = true; // rebuilt when the buffer changes
    };

} // namespace ps3clone
