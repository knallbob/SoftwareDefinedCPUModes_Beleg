/*
 * Copyright (c) 2016 RISC-V Foundation
 * Copyright (c) 2016 The University of Virginia
 * Copyright (c) 2018 TU Dresden
 * Copyright (c) 2020 Barkhausen Institut
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "arch/riscv/faults.hh"
#include "arch/riscv/insts/static_inst.hh"
#include "arch/riscv/isa.hh"
#include "arch/riscv/regs/misc.hh"
#include "arch/riscv/utility.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/Faults.hh"
#include "sim/debug.hh"
#include "sim/full_system.hh"
#include "sim/workload.hh"
#include "cpu/simple/atomic.hh"
#include "mem/translating_port_proxy.hh"

namespace gem5
{

namespace RiscvISA
{

void
RiscvFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Fault %s encountered at pc %s.", name(), tc->pcState());
}

void RiscvFault::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    invoke_inner(tc, inst, tc->pcState().instAddr(), tc->readMiscReg(MISCREG_MODE));
}

void RiscvFault::invoke_inner(ThreadContext *tc, const StaticInstPtr &inst, Addr exception_addr, uint64_t exception_modeid)
{
    tc->setMiscRegNoEffect(MISCREG_MODE, tc->readMiscRegNoEffect(MISCREG_PRV));
    tc->setMiscRegNoEffect(MISCREG_PRV, 1);
    auto pc_state = tc->pcState().as<PCState>();
    auto c_id = tc->getCpuPtr()->cpuId();

    // If the main cpu has a fault, it just writes to the PIO device on location 0x3000000 to trigger
    // the change to the control core
    if(c_id == 1){
        tc->setMiscReg(MISCREG_MEPC, tc->pcState().instAddr()); // current PC is saved to the MEPC register
        Addr memoryAddress = 0x3000000;
        uint8_t dataToWrite = 1;
        uint8_t *dataptr = &dataToWrite;
        Request::Flags flags = 0;
        TranslatingPortProxy proxy(tc);
        proxy.writeBlob(memoryAddress, &dataptr, sizeof(uint8_t)); // write to memory to activate PIO device

        Addr addr =  mbits((tc->readMiscRegNoEffect(82)), 63, 2);
        pc_state.set(addr);
        tc->pcState(pc_state);
        return;
    }

    // If the control core receives a fault it jumps to the start of the handler address
    if(c_id == 2){
        Addr addr = 0x9FFFFFF0;
        pc_state.set(addr);
        tc->pcState(pc_state);
        return;
    }
}

void
Reset::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    tc->setMiscReg(MISCREG_PRV, PRV_M);
    STATUS status = tc->readMiscReg(MISCREG_STATUS);
    status.mie = 0;
    status.mprv = 0;
    tc->setMiscReg(MISCREG_STATUS, status);
    tc->setMiscReg(MISCREG_MCAUSE, 0);

    // Advance the PC to the implementation-defined reset vector
    auto workload = dynamic_cast<Workload *>(tc->getSystemPtr()->workload);
    PCState pc(workload->getEntry());
    tc->pcState(pc);
}

void
UnknownInstFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    auto *rsi = static_cast<RiscvStaticInst *>(inst.get());
    panic("Unknown instruction 0x%08x at pc %s", rsi->machInst,
        tc->pcState());
}

void
IllegalInstFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    auto *rsi = static_cast<RiscvStaticInst *>(inst.get());
    panic("Illegal instruction 0x%08x at pc %s: %s", rsi->machInst,
        tc->pcState(), reason.c_str());
}

void
UnimplementedFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Unimplemented instruction %s at pc %s", instName, tc->pcState());
}

void
IllegalFrmFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Illegal floating-point rounding mode 0x%x at pc %s.",
            frm, tc->pcState());
}

void
BreakpointFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    schedRelBreak(0);
}

void
SyscallFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    tc->getSystemPtr()->workload->syscall(tc);
}

} // namespace RiscvISA
} // namespace gem5
