// main.cpp - process entry point for the PS3 clone skeleton.
//
// Boots the whole system (Cell + RSX + modules) and offers a couple of
// diagnostic modes so you can see "where everything is" at runtime:
//
//   ps3clone --help            show usage
//   ps3clone --list-components walk the Cell and print every component
//   ps3clone --boot-demo       boot the system and run a few instructions
#include "common/Config.h"
#include "common/Logging.h"
#include "common/Types.h"
#include "Cell/PPU/PPU.h"
#include "Cell/SPU/SPU.h"
#include "Cell/EIB/EIB.h"
#include "Cell/Memory/XdrMemory.h"
#include "RSX/RSX.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace ps3clone;

namespace
{

    void PrintUsage(const char *argv0)
    {
        std::cout
            << "PS3 clone skeleton\n\n"
            << "Usage: " << argv0 << " [options]\n\n"
            << "Options:\n"
            << "  --help              Show this help text\n"
            << "  --list-components   Walk the Cell and print every component\n"
            << "  --boot-demo         Boot the system and run a few instructions\n";
    }

    int ListComponents()
    {
        LogInfo("main", "Walking the Cell Broadband Engine...");

        PPU ppu;
        ppu.Boot();
        std::cout << ppu.Describe() << "\n";

        for (int i = 0; i < kPhysicalSPUCount; ++i)
        {
            SPU spu(i);
            spu.Boot();
            std::cout << spu.Describe() << "\n";
        }

        EIB eib;
        eib.Initialize();
        std::cout << eib.Describe() << "\n";

        XdrMemory memory;
        memory.Initialize();
        std::cout << memory.Describe() << "\n";

        RSX rsx;
        rsx.Initialize();
        std::cout << rsx.Describe() << "\n";

        return 0;
    }

    int BootDemo()
    {
        LogInfo("main", "Booting PS3 clone skeleton (demo)...");

        PPU ppu;
        if (ppu.Boot() != CellResult::CELL_OK)
        {
            LogError("main", "PPU failed to boot");
            return 1;
        }

        EIB eib;
        eib.Initialize();

        std::vector<std::unique_ptr<SPU>> spus;
        for (int i = 0; i < kUsableSPUCount; ++i)
        {
            auto spu = std::make_unique<SPU>(i);
            spu->Boot();
            spus.push_back(std::move(spu));
        }

        // Run a handful of steps on each hardware thread.
        ppu.Step(0);
        ppu.Step(1);

        // Simulate one DMA between an SPU and main memory across the EIB.
        eib.Transfer(EIBNode::SPU0, EIBNode::MIC, 16 * 1024);

        LogInfo("main", "Demo complete.");
        return 0;
    }

} // namespace

int main(int argc, char **argv)
{
    Config &config = Config::Get();
    (void)config;

    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "-h") == 0)
        {
            PrintUsage(argv[0]);
            return 0;
        }
        if (std::strcmp(arg, "--list-components") == 0)
        {
            return ListComponents();
        }
        if (std::strcmp(arg, "--boot-demo") == 0)
        {
            return BootDemo();
        }
        std::cerr << "Unknown option: " << arg << "\n";
        PrintUsage(argv[0]);
        return 2;
    }

    PrintUsage(argv[0]);
    return 0;
}
