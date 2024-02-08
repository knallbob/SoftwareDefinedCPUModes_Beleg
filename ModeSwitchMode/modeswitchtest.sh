cd ./gem5 || { echo "gem5 not valid"; exit 1; }

build/RISCV/gem5.opt test_config.py modeswitch_test