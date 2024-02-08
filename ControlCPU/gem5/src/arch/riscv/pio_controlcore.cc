#include "arch/riscv/pio_controlcore.hh"
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
PIO_ControlCore::PIO_ControlCore(const Params &p) : BasicPioDevice(p, sizeof(uint64_t)), mcpu(p.mcpu), ccpu(p.ccpu), delay(p.delay),
    port("port", *this)
{
}

PIO_ControlCore::~PIO_ControlCore()
{    
}

Tick PIO_ControlCore::read(PacketPtr pkt){
    pkt->makeResponse();
    return pioDelay;
}

Tick PIO_ControlCore::write(PacketPtr pkt){
    
    ccpu->suspendContext(0); // deactivate the Control Core
    auto startfunc = [this](){
        mcpu->activateContext(0); // activate the main CPU
    };
    EventFunctionWrapper *event = new EventFunctionWrapper(startfunc, "ActivateEvent");

    mcpu->schedule(event, curTick() + delay);
    pkt->makeResponse();
    return pioDelay;
}

Port& PIO_ControlCore::getPort(const std::string &if_name, PortID idx){
    if (if_name == "port")
        return port;
    return PioDevice::getPort(if_name, idx);
}

PIO_ControlCore::CPUPort::CPUPort(const std::string &_name, PIO_ControlCore &_owner)
        : RequestPort(_name, &_owner), pio_controlcore(_owner), blockedPacket(nullptr)
{
}

PIO_ControlCore::CPUPort::~CPUPort()
{
}

bool PIO_ControlCore::CPUPort::recvTimingResp(PacketPtr pkt){
    return true;
}

void PIO_ControlCore::CPUPort::recvReqRetry(){

}
}//Namespace gem5