/*
 * Copyright (c) 2009 The Regents of The University of Michigan
 * Copyright (c) 2009 The University of Edinburgh
 * Copyright (c) 2014 Sven Karlsson
 * Copyright (c) 2016 RISC-V Foundation
 * Copyright (c) 2016 The University of Virginia
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

#ifndef __ARCH_RISCV_ISA_HH__
#define __ARCH_RISCV_ISA_HH__

#include <unordered_map>
#include <vector>

#include "arch/generic/isa.hh"
#include "arch/riscv/faults.hh"
#include "arch/riscv/pcstate.hh"
#include "arch/riscv/regs/misc.hh"
#include "arch/riscv/types.hh"
#include "base/types.hh"
#include "arch/riscv/mlb.hh"
#include "arch/riscv/plb.hh"
#include "sim/stat_control.hh"
#include "sim/sim_exit.hh"
#include "cpu/o3/cpu.hh"

namespace gem5
{

struct RiscvISAParams;
class Checkpoint;

namespace RiscvISA
{

enum FPUStatus
{
    OFF = 0,
    INITIAL = 1,
    CLEAN = 2,
    DIRTY = 3,
};

class ISA : public BaseISA
{
  protected:
    std::vector<RegVal> miscRegFile;
    std::vector<uint64_t> privReg;
    bool checkAlignment;

    bool hpmCounterEnabled(int counter) const;
    std::shared_ptr<RiscvFault> saved_fault;
    bool has_fault = false;

  public:
    using Params = RiscvISAParams;

    void clear() override;

    PCStateBase *
    newPCState(Addr new_inst_addr=0) const override
    {
        return new PCState(new_inst_addr);
    }

    MLB *mlb;
    PLB *plb;

  public:
    RegVal readMiscRegNoEffect(RegIndex idx) const override;
    RegVal readMiscReg(RegIndex idx) override;
    void setMiscRegNoEffect(RegIndex idx, RegVal val) override;
    void setMiscReg(RegIndex idx, RegVal val) override;

    void setRegs(int index, RegVal value);
    RegVal getRegs(int index);

    void statdump(int var){
        if(var == 1){
            statistics::schedStatEvent(true,true,curTick(),0);
            exitSimLoop("End of Testcase");
        }
        statistics::schedStatEvent(true,true,curTick(),0);
    };

    void setPrivlevel(uint64_t idx, uint64_t privLevel);
    void resetPrivlevel(uint64_t idx, uint64_t Mode);
    bool isModeAllowed(uint64_t reg, uint64_t checkMode);

    // Derived class could provide knowledge of non-standard CSRs to other
    // components by overriding the two getCSRxxxMap here and properly
    // implementing the corresponding read/set function. However, customized
    // maps should always be compatible with the standard maps.
    virtual const std::unordered_map<int, CSRMetadata>&
    getCSRDataMap() const
    {
        return CSRData;
    }
    virtual const std::unordered_map<int, RegVal>&
    getCSRMaskMap() const
    {
        return CSRMasks;
    }

    bool alignmentCheckEnabled() const { return checkAlignment; }

    bool inUserMode() const override;
    void copyRegsFrom(ThreadContext *src) override;

    void serialize(CheckpointOut &cp) const override;
    void unserialize(CheckpointIn &cp) override;

    ISA(const Params &p);

    void handleLockedRead(const RequestPtr &req) override;

    bool handleLockedWrite(const RequestPtr &req,
            Addr cacheBlockMask) override;

    void handleLockedSnoop(PacketPtr pkt, Addr cacheBlockMask) override;

    void globalClearExclusive() override;

    MLB *getMLB()
    {
        return mlb;
    }

    PLB *getPLB()
    {
        return plb;
    }
    bool has_saved_fault();
    std::shared_ptr<RiscvFault> get_saved_fault();
    void set_saved_fault(std::shared_ptr<RiscvFault> fault, Addr exception_addr, uint64_t exception_modeid);
};

} // namespace RiscvISA
} // namespace gem5

std::ostream &operator<<(std::ostream &os, uint64_t pm);

#endif // __ARCH_RISCV_ISA_HH__