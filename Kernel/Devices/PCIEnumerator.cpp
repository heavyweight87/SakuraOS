#include "PCIEnumerator.h"
#include "Libk.h"
#include "Arch.h"

namespace Devices { 

#define MAX_NUM_PCI_DEVICES 32
#define MAX_NUM_PCI_FUNCTIONS 32
#define INVALID_VENDOR_ID 0xFFFF

enum PCIOffset
{
    VendorID = 0x00,
    BaseClass = 0x0B,
    SubClass = 0x0C,
    HeaderType = 0x0E,
    SecondaryBus = 0x19
};

void PCIEnumerator::writeConfigAddress(PCIAddress& pciAddress, std::uint8_t offset)
{
    std::uint32_t address = 0x80000000u | (pciAddress.bus << 16u) | 
        (pciAddress.device << 11u) | (pciAddress.function << 8u) | (offset & 0xfc);
    outl(0xCF8, address);
}

uint16_t PCIEnumerator::pciConfigReadWord(PCIAddress& pciAddress, std::uint8_t offset) 
{
    writeConfigAddress(pciAddress, offset);
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    return (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

uint8_t PCIEnumerator::pciConfigReadByte(PCIAddress& pciAddress, std::uint8_t offset) 
{
    writeConfigAddress(pciAddress, offset);
    /* (offset & 3) * 8) = 0 will choose the first byte of the 32 bits register */
    return  (uint8_t)((inl(0xCFC) >> ((offset & 3) * 8)) & 0xffff);
}

 void PCIEnumerator::checkFunction(PCIAddress& pciAddress) 
 {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;

    baseClass = getBaseClass(pciAddress);
    subClass = getSubClass(pciAddress);
    if( (baseClass == 0x06) && (subClass == 0x04)) 
    {
        secondaryBus = getSecondaryBus(pciAddress);
        enumerateBus(secondaryBus);
    }
    else
    {
        uint16_t vendor = pciConfigReadWord(pciAddress, 0);
        uint16_t device = pciConfigReadWord(pciAddress, 2);
        uint8_t deviceClass = pciConfigReadByte(pciAddress, 0x0b);
        Libk::printk("device = %X\r", pciAddress.device);
        Libk::printk("function = %X\r", pciAddress.function);
        Libk::printk("vendor = %X\r", vendor);
        Libk::printk("device id = %X\r", device);
        Libk::printk("class = %X\r\n", deviceClass);
    }
 }

void PCIEnumerator::enumerateBus(std::uint8_t bus)
{
    PCIAddress pciAddress = {};
    pciAddress.bus = bus;
    for(int device = 0; device < MAX_NUM_PCI_DEVICES; device++)
    {
        pciAddress.function = 0;
        pciAddress.device = device;
        if(getVendorID(pciAddress) != INVALID_VENDOR_ID) 
        {
            checkFunction(pciAddress);
            uint16_t headerType = getHeaderType(pciAddress);
            if((headerType & 0x80) != 0) 
            {
                for(int function = 1; function < 8; function++) 
                {
                    pciAddress.function = function;
                    if(getVendorID(pciAddress) != 0xFFFF) 
                    {
                        checkFunction(pciAddress);
                    }
                }
            }
        }
    }
}

void PCIEnumerator::enumerate()
{
    PCIAddress pciAddress = {};
    uint8_t headerType = getHeaderType(pciAddress);
    if(headerType&0x80) 
    {
        //single PCI controller
        enumerateBus(0); //enumerate first and only bus
    }
    else
    {
        for(int function = 0; function < MAX_NUM_PCI_FUNCTIONS; function++) 
        {
            pciAddress.function = function;
            enumerateBus(function);
        }
    }
}

std::uint16_t PCIEnumerator::getVendorID(PCIAddress& address)
{
    return pciConfigReadWord(address, PCIOffset::VendorID);
}

std::uint8_t PCIEnumerator::getBaseClass(PCIAddress& address)
{
    return pciConfigReadWord(address, PCIOffset::BaseClass);
}

std::uint8_t PCIEnumerator::getSubClass(PCIAddress& address)
{
    return pciConfigReadWord(address, PCIOffset::SubClass);
}

std::uint8_t PCIEnumerator::getSecondaryBus(PCIAddress& address)
{
    return pciConfigReadWord(address, PCIOffset::SecondaryBus);
}

std::uint8_t PCIEnumerator::getHeaderType(PCIAddress& address)
{
    return pciConfigReadWord(address, PCIOffset::HeaderType);
}




}