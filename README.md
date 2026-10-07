# Automated HPLC–AAS Control Unit

A laboratory-built automated control unit designed to synchronize a **High Performance Liquid Chromatography (HPLC) system, an Atomic Absorption Spectrometry (AAS) instrument, and a servo-actuated three-way valve** during automated analytical measurements.

The system provides programmable valve timing based on chromatographic retention times, coordinates the start of the analytical measurement, and displays the current process status to the operator.

**Original version:** February 2023
**Updated:** August 2024

**Main photo of the complete control unit:**

<img width="1802" height="1742" alt="Figure s3 2" src="https://github.com/user-attachments/assets/3d1c3a79-3eda-4071-ab95-65adfb3a1a67" />



## Overview

In the analytical procedure, different species eluting from an HPLC column need to be processed at specific times. Therefore, the gas flow to the atomizer must be switched between different states according to the chromatographic retention times. Manually operating this process was demanding and susceptible to errors.

To automate this process, a dedicated **Automated Gas Control unit** was developed.

The controller:

* Detects the start of an HPLC run
* Triggers the AAS measurement
* Controls a servo-actuated three-way valve
* Executes programmable valve timing sequences
* Provides an LCD-based configuration interface
* Stores configuration parameters in EEPROM
* Displays the current step and elapsed time during operation

**Schematic diagram of the electronic circuit:**

<img width="1031" height="854" alt="Fig S1" src="https://github.com/user-attachments/assets/54f508a2-b66f-4944-89de-a18138c16520" />



## System Architecture

The control unit acts as an interface between the chromatographic separation and the atomic absorption measurement.

```text
                   ┌─────────────────┐
                   │      HPLC       │
                   └───────┬─────────┘
                           │
                      Start Signal
                           │
                           ▼
                ┌─────────────────────┐
                │     Control Unit    │
                │                     │ 
                │  Arduino Leonardo   │
                └──────┬───────┬──────┘
                       │       │
             Servo     │       │ USB / Keyboard
             Control   │       │ Trigger
                       │       │
                       ▼       ▼
                ┌──────────┐  ┌─────────┐
                │  3-Way   │  │   AAS   │
                │  Valve   │  │         │
                └────┬─────┘  └─────────┘
                     │
                     ▼
                 Atomizer
```



## Automated Operation

The automated sequence follows the analytical workflow:

```text
1. Sample injection
       │
       ▼
2. HPLC chromatographic run starts
       │
       ▼
3. Control unit recognizes the run-start signal
       │
       ├───────────────► AAS measurement triggered
       │
       ▼
4. Programmable valve sequence starts
       │
       ▼
5. Valve switches according to retention times
       │
       ▼
6. Sequence completed
```

Valve operation is referenced to the start of the chromatographic run, eliminating manual intervention during the separation and providing reproducible synchronization between the instruments.



## HPLC Synchronization

The controller recognizes the beginning of the chromatographic run through an external **RS232 signal from the HPLC**.

Once the run starts, the unit automatically initiates the programmed sequence, allowing valve timing to be synchronized with the actual chromatographic separation.



## AAS Integration

The unit automatically initiates the AAS measurement after recognizing the start of the chromatographic run.

The **ATmega32u4-based Arduino Leonardo** communicates with the computer running the AAS software through USB and is recognized as a USB keyboard. The firmware sends an **ENTER key event**, which is used as the shortcut for starting AAS signal acquisition.

```text
Control unit
 │
 │ USB
 ▼
Computer running AAS software
 │
 │ ENTER key
 ▼
AAS signal acquisition
```

This provides a simple interface between the custom controller and the existing instrument software without requiring dedicated code on the AAS computer.



## Automated Valve Control

The gas flow is controlled using a **three-way valve actuated by a servo motor**.

The servo has two programmable positions corresponding to the required valve states:

* **OPEN** — gas flow released
* **CLOSE** — gas flow blocked

The angular positions can be adjusted directly from the controller menu, allowing the actuator to be calibrated to the specific valve installation without modifying the firmware.



## Programmable Sequence

The controller provides a menu for programming the valve sequence according to the expected chromatographic retention times.

The firmware supports up to **12 programmable valve steps**, alternating between OPEN and CLOSE states. Each step has an independently programmed timing interval.

During operation, the LCD displays the current step and elapsed time.

**Menu and step sequence displayed in the unit:**

<img width="1000" height="2225" alt="FigS3git" src="https://github.com/user-attachments/assets/2a73c13f-c6b5-4d9f-b7a2-0bce1082aecf" />



## User Interface

The controller can be configured directly from the front panel without requiring a computer.

The interface consists of:

* 16×2 character LCD with I²C interface
* Rotary encoder
* Pushbuttons

The menu allows the operator to configure:

* HPLC synchronization mode
* AAS measurement triggering
* Valve opening and closing times
* OPEN and CLOSED servo positions



## Parameter Storage

The programmed parameters are stored in the microcontroller's **EEPROM**, allowing the configuration to be retained after power is removed.

Stored parameters include:

* HPLC operating mode
* AAS trigger configuration
* Valve OPEN and CLOSED positions
* Six opening intervals
* Six closing intervals



## Electronics

The main electronic components are:

| Component         | Specification                 | Function                                 |
| ----------------- | ----------------------------- | ---------------------------------------- |
| Microcontroller   | Arduino Leonardo / ATmega32u4 | Main controller                          |
| Display           | 16×2 LCD + I²C module         | User interface                           |
| Servo motor       | MG995, 180°                   | Three-way valve actuation                |
| Rotary encoder    | KY-040                        | Menu navigation and parameter adjustment |
| Pushbuttons       | PBS-10B                       | User control                             |
| Power supply      | 12 V DC, 1 A                  | System power                             |
| Voltage regulator | LM2596 step-down              | 12 V → 5 V                               |
| HPLC interface    | RS232                         | Run-start synchronization                |

**Interior view of the control unit:**

<img width="1322" height="749" alt="IMG_5373 brilho mod" src="https://github.com/user-attachments/assets/394719ca-fb1c-40c9-a487-924a7795025e" />



## Author

**Gilberto Coelho**

This project was developed as part of laboratory automation and analytical instrumentation research.

The system was used in the analytical procedure described in the following scientific publication:

https://doi.org/10.1016/j.aca.2025.344884
