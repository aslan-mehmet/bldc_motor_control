choosable JP5
VIN -->  E5V or
USB -->  U5V
JP5 on +5V=U5V

uc power VDD
JP6 on VDD=+3V3
AVDD=VDD

max cpu freq 84MHz

main regulator output voltage
(PWR_CR->VOS) regulator voltage scaling output selection
Scale3 HCLK<=60M
Scale2 60M<=HCLK<=84M
data pg 65

flash wait state
flash latency depends on power supply
data pg 61 table 15
84MHz with 2 wait states

en.STM32L4_Memory_Flash.pdf pg 4
adaptive real-time memory accel
ART accelerator (Instruction cache, Data cache and prefetch buffer)
allowing linear performance in relation to frequency

in their classic def
dont know how works
turn it on, why not
prefetch buffer
instruction cache
data cache
