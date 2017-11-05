![alt text](scope/CCxE_disable.png)

Image 1, when used main_CCxE_toggle file

using CCxE register to pull down pwm pin, create problems
when OCx active(high) if CCxE disabled as in Image 1 CH2
it does not pull down immediately, rather discharges

![alt text](scope/CCR_0_duty.png)

Image 2

setting CCRx to zero
it contain high frequency content. dont know why?
