#include "arch/riscv/pio_maincpu.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"
#include "mem/port.hh"
#include "mem/packet.hh"
#include "sim/sim_object.hh"
#include "dev/io_device.hh"
#include "arch/riscv/faults.hh"
#include <iostream>
#include <unistd.h>
#include "base/bitunion.hh"
#include "arch/riscv/regs/misc.hh"

namespace gem5{
PIO_MainCPU::PIO_MainCPU(const Params &p) : BasicPioDevice(p, sizeof(uint64_t)), mcpu(p.mcpu), ccpu(p.ccpu), delay(p.delay),
    port("port", *this)
{
}

PIO_MainCPU::~PIO_MainCPU()
{    
}

Tick PIO_MainCPU::read(PacketPtr pkt){
    pkt->makeResponse();
    return pioDelay;
}

Tick PIO_MainCPU::write(PacketPtr pkt){
    ccpu->suspendContext(0);
    mcpu->suspendContext(0); // suspend the CPUs to avoid Out-of-Order issues

    auto startfunc = [this](){ // defining an event which starts the CPUs
        ccpu->f_isa_ptr = mcpu->getContext(0)->getIsaPtr();
        ccpu->mcpu = mcpu;
        ccpu->activateContext(0); // activate the Control Core
    };

    EventFunctionWrapper *event1 = new EventFunctionWrapper(startfunc, "ActivateEvent");
    ccpu->schedule(event1, curTick() + delay); // schedule the event with user defined delay
    pkt->makeResponse();
    return pioDelay;

}

Port& PIO_MainCPU::getPort(const std::string &if_name, PortID idx){
    if (if_name == "port")
        return port;
    return PioDevice::getPort(if_name, idx);
}

PIO_MainCPU::CPUPort::CPUPort(const std::string &_name, PIO_MainCPU &_owner)
        : RequestPort(_name, &_owner), pio_maincpu(_owner), blockedPacket(nullptr)
{
}

PIO_MainCPU::CPUPort::~CPUPort()
{
}

bool PIO_MainCPU::CPUPort::recvTimingResp(PacketPtr pkt){
    return true;
}

void PIO_MainCPU::CPUPort::recvReqRetry(){

}
}//Namespace gem5