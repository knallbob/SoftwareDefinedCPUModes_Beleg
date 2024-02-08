#include "arch/riscv/faults.hh"
#include "arch/riscv/mlb.hh"

#include <cstring>

#define MLB_SI 0x2
#define MLB_FI 0x1

namespace gem5
{

MLB::MLB(const Params &params)
            : SimObject(params),
            entries(params.size)
        {
        }
        
        Fault MLB::store_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> data, uint32_t machInst)
        {

            if(cur_modeid != 1)
            {
                return std::make_shared<RiscvISA::IllegalInstFault>("mode!=1 trying to change other mode", machInst);
            }

            struct mlb_entry *entry = (struct mlb_entry*)(data.data());
            entries.at(line) = *entry;
            return NoFault;
        }

        Fault MLB::load_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> *data, uint32_t machInst)
        {
            if(cur_modeid != 1)
            {
                return std::make_shared<RiscvISA::IllegalInstFault>("mode!=1 trying to load other mode", machInst);
            }
            
            memcpy(data->data(), &entries.at(line), sizeof(mlb_entry));

            return NoFault;
        }

        bool MLB::is_mode_allowed(uint16_t modeid, uint64_t csr_id){

            if(modeid == 1){
                return true;
            }

            if(csr_id == 0){
                return false;
            }

            int line = get_line_for_modeid(modeid);
            uint64_t rights = entries.at(line).combined_csr;

            if (csr_id < 12) {

                int position = static_cast<int>(csr_id) + 1;
                return (rights & (static_cast<uint64_t>(1) << position)) != 0;
            } 
            if(csr_id > 11 && csr_id < 70){
                int position = 1;
                return (rights & (static_cast<uint64_t>(1) << position)) != 0;
            }
            if(csr_id > 69 && csr_id < 85){
                int position = static_cast<int>(csr_id) - 57;
                return (rights & (static_cast<uint64_t>(1) << position)) != 0;
            }
            if(csr_id > 84 && csr_id < 103){
                int position = 0;
                return (rights & (static_cast<uint64_t>(1) << position)) != 0;
            }
            if(csr_id > 102 && csr_id < 121){
                int position = static_cast<int>(csr_id) - 73;
                return (rights & (static_cast<uint64_t>(1) << position)) != 0;
            }
            return false;
        }
        
        Fault MLB::get_entry_point(uint16_t modeid, uint64_t* entry_point)
        {
            int line = get_line_for_modeid(modeid);
            if(line == -1)
            {
                warn("Couldn't get ep for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            *entry_point = entries.at(line).entry_point;
            return NoFault;
        }

        Fault MLB::get_epc(uint16_t modeid, uint64_t *epc)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {

                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            *epc = entries.at(line).epc;

            return NoFault;
        }

        Fault MLB::set_epc(uint16_t modeid, uint64_t epc)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {
                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            entries.at(line).epc = epc;

            return NoFault;
        }

        int MLB::get_line_for_modeid(uint16_t modeid)
        {
            for(int i = 0; i < entries.size(); i++)
            {
                if(entries.at(i).modeid == modeid)
                {
                    return i;
                }
            }
            return -1;
        }
}
