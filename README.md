# DRV8701P driver abstraction
Repository containing the assignment for the company Dometic

### Questions:
1.	What is the purpose of slewing the command before outputting it to the predriver? Why might we want to do this?
    ----> By doing this it is possible to avoid big current spikes. We might want to do this on during the start for example. By creating a soft start, we ease the load on the power supply.
2.	How does the brake work? When the brake is applied, what happens to the h-bridge and why does that cause the motor to brake?
    ----> The brake works by creating a freewheeling circuit with the motor and the switches. At the moment where the low side switches are closed, the motor starts acting as a generator, and it`s kinectic energy will turn into electric energy, which will then be dissipated as heat on the switches.
3.	What are the advantages / disadvantages of using analog control vs. digital (eg SPI)?
    ----> One of the advantages of using the analog control is the lower latency, where there is no communication overhead and no time needed to transmit the information from the main IC to the motor driver. It also provides flexibility in the sense that the control logic is all done by the microcontroller so if a different slew rate is needed, a simple change in driver code will solve it.
