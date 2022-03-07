@echo off
rem TODO: should probably try and detect whether 32/64 bit OS and execute appropriate version of ansicon.
start "uCsim (STM8S208)" ansicon\x64\ansicon.exe sstm8.exe -t STM8S208 -X 16M -I if=rom[0x5800] -C "sim_cmds.txt" "bin\Test\test"
