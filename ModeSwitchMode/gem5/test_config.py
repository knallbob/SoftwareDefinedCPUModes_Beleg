import m5
from m5.objects import *

import argparse

parser = argparse.ArgumentParser(description="Test Bench for Control CPU")

parser.add_argument(
    "testcase",
    type=str,
    help="The testcase file name",
)

args = parser.parse_args()

core = System()
core.clk_domain = SrcClockDomain()
core.clk_domain.clock = '1GHz'
core.clk_domain.voltage_domain = VoltageDomain()
core.xbar = SystemXBar()
core.cpu = RiscvO3CPU()
core.cpu.cpu_id = 1
core.cpu.createThreads()
core.cpu.createInterruptController()

memory_range = AddrRange("0x2", "0x8FFFFFFF")
core.mem_ctrl = MemCtrl()
core.mem_ctrl.dram = DDR3_1600_8x8()
core.mem_ctrl.dram.device_size = "8192000000"
core.mem_ctrl.dram.range = memory_range
core.mem_ctrl.port = core.xbar.mem_side_ports


core.xbar.cpu_side_ports = core.cpu.icache_port
core.xbar.cpu_side_ports = core.cpu.dcache_port

core.system_port = core.xbar.cpu_side_ports
core.workload = RiscvBareMetal(bootloader = args.testcase)

core.mem_mode = 'timing'

root = Root(full_system=True, system=core)
m5.instantiate()
m5.stats.reset()
print("Beginning simulation!")
exit_event = m5.simulate()
print("Exiting @ tick {} because {}.".format(m5.curTick(), exit_event.getCause()))
m5.stats.dump()