@echo off
ucsim_stm8.exe -t STM8S208 -X 16M -I if=rom[0x5800] -C "sim_cmds.txt" "bin\Test\test"
