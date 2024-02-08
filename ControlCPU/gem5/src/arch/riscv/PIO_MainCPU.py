from m5.SimObject import SimObject
from m5.params import *
from m5.objects.Device import BasicPioDevice
from m5.objects.BaseO3CPU import BaseO3CPU

class PIO_MainCPU(BasicPioDevice):
    type = "PIO_MainCPU"
    cxx_header = 'arch/riscv/pio_maincpu.hh'
    cxx_class = 'gem5::PIO_MainCPU'

    pio_size = Param.Addr(0xC000, "PIO Size")
    mcpu = Param.BaseO3CPU("Main Core")
    ccpu = Param.BaseO3CPU("Control Core")
    delay = Param.Int("Delay")
    port = RequestPort("MemoryPort")
