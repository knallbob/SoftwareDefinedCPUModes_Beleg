## How to run
Inside the gem5 folder execute :
```shell
    build/RISCV/gem5.opt test_config.py
```
with the desired benchmark and parameter. 

For Example the modeswitchtest with a delay of 1000 :

```shell
    build/RISCV/gem5.opt test_config.py modeswitch_test --delay=1000
```
List of all benchmarks
- syscall_test
- modeswitch_test