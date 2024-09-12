#include <iostream>
#include <windows.h>
#include <vector>


namespace CpuCacheInfoHelper
{
    static void PrintCacheType(PROCESSOR_CACHE_TYPE cacheType)
    {
        switch (cacheType)
        {
            case CacheUnified: std::cout << "Unified"; break;
            case CacheInstruction: std::cout << "Instruction"; break;
            case CacheData: std::cout << "Data"; break;
            case CacheTrace: std::cout << "Trace"; break;
            default: std::cout << "Unknown"; break;
        }
    }

    static void PrintLogicalProcessorInformation()
    {
        DWORD bufferSize = 0;
        GetLogicalProcessorInformation(nullptr, &bufferSize);
        
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (!GetLogicalProcessorInformation(buffer.data(), &bufferSize))
        {
            std::cerr << "Error retrieving processor information.\n";
            return;
        }

        for (const auto& info : buffer)
        {
            std::cout << "Processor Mask: " << info.ProcessorMask << "\n";
            
            switch (info.Relationship) {
                case RelationProcessorCore:
                    std::cout << "Relationship: Processor Core\n";
                    std::cout << "Hyperthreading: " << ((info.ProcessorCore.Flags) ? "Enabled" : "Disabled") << "\n";
                    break;

                case RelationNumaNode:
                    std::cout << "Relationship: NUMA Node\n";
                    std::cout << "NUMA Node: " << info.NumaNode.NodeNumber << "\n";
                    break;

                case RelationCache:
                    std::cout << "Relationship: Cache\n";
                    std::cout << "Level: " << uint8_t(info.Cache.Level) << "\n";
                    std::cout << "Associativity: " << uint8_t(info.Cache.Associativity) << "\n";
                    std::cout << "Line Size: " << info.Cache.LineSize << " bytes\n";
                    std::cout << "Cache Size: " << info.Cache.Size << " bytes\n";
                    std::cout << "Cache Type: ";
                    PrintCacheType(info.Cache.Type);
                    std::cout << "\n";
                    break;

                case RelationProcessorPackage:
                    std::cout << "Relationship: Processor Package\n";
                    break;

                default:
                    std::cout << "Relationship: Unknown\n";
                    break;
            }

            std::cout << "----------------------------------------\n";
        }
    }
}