#pragma once

#include <cstdint>

namespace Devices {

class PCIEnumerator {
    public:
        void enumerate();
    private:
        struct PCIAddress
        {
            std::uint8_t bus;
            std::uint8_t device;
            std::uint8_t function;
        };

        void enumerateBus(std::uint8_t bus);
        void writeConfigAddress(PCIAddress& pciAddress, std::uint8_t offset);
        std::uint16_t pciConfigReadWord(PCIAddress& pciAddress, std::uint8_t offset);
        std::uint8_t pciConfigReadByte(PCIAddress& pciAddress, std::uint8_t offset);
        void checkFunction(PCIAddress& pciAddress);
        std::uint8_t getSecondaryBus(PCIAddress& address);
        std::uint8_t getHeaderType(PCIAddress& address);

        std::uint16_t getVendorID(PCIAddress& address);
        std::uint8_t getBaseClass(PCIAddress& address);
        std::uint8_t getSubClass(PCIAddress& address);
};

}