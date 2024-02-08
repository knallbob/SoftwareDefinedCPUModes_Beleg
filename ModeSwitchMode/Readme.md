## How to run
Inside the gem5 folder execute :
```shell
    build/RISCV/gem5.opt test_config.py
```
with the desired benchmark and parameter. 

For Example the modeswitchtest :

```shell
    build/RISCV/gem5.opt test_config.py modeswitch_test
```
List of all benchmarks
- reg3_test (modeswitch test with amount of registers saved)
- reg12_test
- reg24_test
- reg31_test
- syscall_test
- modeswitch_test