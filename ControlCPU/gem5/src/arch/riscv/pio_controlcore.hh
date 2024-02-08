#ifndef __ARCH_RISCV_PIO_CONTROLCORE_HH__
#define __ARCH_RISCV_PIO_CONTROLCORE_HH__

#include "sim/sim_object.hh"
#include "mem/port.hh"
#include "mem/packet.hh"
#include "sim/sim_object.hh"
#include "dev/io_device.hh"
#include "params/PIO_ControlCore.hh"
#include "cpu/simple/atomic.hh"
#include "cpu/o3/cpu.hh"


namespace gem5
{
namespace RiscvISA{
    class ISA;
}

// The PIO device which switches back from the Control Core
class PIO_ControlCore : public BasicPioDevice {
    public:
        PARAMS(PIO_ControlCore);
        PIO_ControlCore(const Params &p);
        ~PIO_ControlCore() override;
        Port &getPort(const std::string &if_name, PortID idx) override;
        o3::CPU* mcpu;
        o3::CPU* ccpu;
        int delay;

    private:
        class CPUPort : public RequestPort{
            public :
                CPUPort(const std::string &_name, PIO_ControlCore  &_owner);
                ~CPUPort() override;

            protected :
                bool recvTimingResp(PacketPtr pkt) override;
                void recvReqRetry() override;

            private :
                PIO_ControlCore &pio_controlcore;
                PacketPtr blockedPacket;
            };
        CPUPort port;

    protected:
        Tick read(PacketPtr pkt) override;
        Tick write(PacketPtr pkt) override;
};
} //Namespace gem5
#endif