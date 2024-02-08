import m5
from m5.objects import *

import argparse

parser = argparse.ArgumentParser(description="Test Bench for Control CPU")

parser.add_argument(
    "testcase",
    type=str,
    help="The testcase file name",
)

parser.add_argument(
    "--delay",
    type=int,
    required=False,
    default=0,
    help="Delay for activating the controlcpu",
)

args = parser.parse_args()

def createConfig():
    core = System()
    core.clk_domain = SrcClockDomain()
    core.clk_domain.clock = '1GHz'
    core.clk_domain.voltage_domain = VoltageDomain()

    core.xbar = SystemXBar()

    
    core.cpu = RiscvO3CPU()
    core.cpu.cpu_id = 1
    core.cpu.createThreads()
    core.cpu.createInterruptController()

    core.control_cpu = RiscvO3CPU()
    core.control_cpu.cpu_id = 2
    core.control_cpu.createThreads()
    core.control_cpu.createInterruptController()
    core.control_cpu.power_state.default_state = "OFF"

    core.main_CPU = PIO_MainCPU(pio_addr=0x3000000, pio_latency='0', mcpu = core.cpu, ccpu = core.control_cpu, delay = args.delay)
    core.cc_PIO = PIO_ControlCore(pio_addr=0xFFFFFFFF, pio_latency='0', mcpu = core.cpu, ccpu = core.control_cpu, delay = args.delay)

    #size_bytes = MemorySize("8192000000").value
    memory_range = AddrRange("0x4000000", "0xFFFFFFFF")
    core.mem_ctrl = MemCtrl()
    core.mem_ctrl.dram = DDR3_1600_8x8()
    core.mem_ctrl.dram.device_size = "8192000000"
    core.mem_ctrl.dram.range = memory_range
    core.mem_ctrl.port = core.xbar.mem_side_ports

    core.main_CPU.pio = core.xbar.mem_side_ports
    core.cc_PIO.pio = core.xbar.mem_side_ports

    core.xbar.cpu_side_ports = core.cpu.icache_port
    core.xbar.cpu_side_ports = core.cpu.dcache_port

    core.xbar.cpu_side_ports = core.control_cpu.icache_port
    core.xbar.cpu_side_ports = core.control_cpu.dcache_port

    core.system_port = core.xbar.cpu_side_ports

    core.workload = RiscvBareMetal(bootloader = args.testcase)

    core.mem_mode = 'timing'

    return core

core = createConfig()
root = Root(full_system=True, system=core)
m5.instantiate()
m5.stats.reset()
print("Beginning simulation!")
exit_event = m5.simulate()
print("Exiting @ tick {} because {}.".format(m5.curTick(), exit_event.getCause()))
m5.stats.dump()
    