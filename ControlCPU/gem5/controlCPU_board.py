import m5
from gem5.components.boards.abstract_system_board import AbstractSystemBoard
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.components.processors.cpu_types import CPUTypes
from gem5.components.cachehierarchies.classic.private_l1_private_l2_cache_hierarchy import PrivateL1PrivateL2CacheHierarchy
from gem5.components.memory import SingleChannelDDR4_2400
from gem5.utils.override import overrides
from gem5.isas import ISA
from m5.objects import *

system =System()

system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]
system.membus = SystemXBar()

cpu1 = RiscvTimingSimpleCPU()

cpu1.createInterruptController()

system.cpu = cpu1
system.cpu.createThreads()

system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

addr = 0x3000000
system.test_PIO = Test_PIO(pio_addr=addr, pio_latency='0')
Test_PIO.pio = system.membus.mem_side_ports

root = Root(full_system=True, system=system)
m5.instantiate(None)
print("Beginning simulation!")
exit_code = m5.simulate()
print('Simulation exited with code %d' % exit_code)