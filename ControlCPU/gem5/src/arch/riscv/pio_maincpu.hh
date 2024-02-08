#ifndef __ARCH_RISCV_PIO_MAINCPU_HH__
#define __ARCH_RISCV_PIO_MAINCPU_HH__

#include "sim/sim_object.hh"
#include "mem/port.hh"
#include "mem/packet.hh"
#include "sim/sim_object.hh"
#include "dev/io_device.hh"
#include "params/PIO_MainCPU.hh"
#include "cpu/simple/atomic.hh"
#include "cpu/o3/cpu.hh"


namespace gem5
{
namespace RiscvISA{
    class ISA;
}

// The PIO device which switches to the Control Core
class PIO_MainCPU : public BasicPioDevice {
    public:
        PARAMS(PIO_MainCPU);
        PIO_MainCPU(const Params &p);
        ~PIO_MainCPU() override;
        Port &getPort(const std::string &if_name, PortID idx) override;
        o3::CPU* mcpu;
        o3::CPU* ccpu;
        int delay;

    private:
        class CPUPort : public RequestPort{
            public :
                CPUPort(const std::string &_name, PIO_MainCPU  &_owner);
                ~CPUPort() override;

            protected :
                bool recvTimingResp(PacketPtr pkt) override;
                void recvReqRetry() override;

            private :
                PIO_MainCPU &pio_maincpu;
                PacketPtr blockedPacket;
            };
        CPUPort port;

    protected:
        Tick read(PacketPtr pkt) override;
        Tick write(PacketPtr pkt) override;
};
} //Namespace gem5
#endif