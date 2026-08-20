## 🎥 Demo
Watch The first take off :
## Overview
I wanted to make small esp32c3 based qaudcopter that I could control from my Phone. I first tried to use WiFi, but the WiFi chip on my ESP wouldnt seem to work. So, I switched to BLE. The drone uses an MPU6050 as its Gyroscope.
## How it works
The Gyroscope measures the rate of change of angles and the desired rates of roll pitch and yaw are sent over BLE from an android app that I made using MIT app maker. The micro-controller measures the difference between the desired rates and the current rates then uses a PID loop to calculate the motor input required at each motor to correct the rotation rates in each axis. The loop has a frequency of 250hz so fresh commands are sent to the motor every 4 milliseconds.
## Bill of Materials (BOM)

| Component | Description | Qty | Price (USD) | Link |
| :--- | :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32-C3 Super Mini Development Board | 1 | $2.21 | [AliExpress](https://www.aliexpress.com/w/wholesale-esp32-c3-super-mini.html) |
| **IMU Sensor** | GY-521 MPU-6050 6-DOF Accelerometer/Gyroscope | 1 | $1.59 | [AliExpress](https://www.aliexpress.com/w/wholesale-gy-521-mpu6050.html) |
| **Motors & Props** | 8520 Coreless Brushed DC Motors (CW/CCW) with Propellers | 4 | $5.81 | [AliExpress](https://www.aliexpress.com/w/wholesale-coreless-motor-8520-propeller.html) |
| **Motor Drivers** | SI2300 N-Channel MOSFETs (SOT-23 package) | 50 | $0.99 | [AliExpress](https://www.aliexpress.com/w/wholesale-si2300.html) |
| **Flyback Diodes** | 1N4148 Switching Diodes (SMD 1206) | 50 | $0.61 | [AliExpress](https://www.aliexpress.com/w/wholesale-switching-diode-1n4148.html) |
| **Gate Resistors** | 200Ω Resistors (PWM current limiting) | 100 | $0.99 | [AliExpress](https://www.aliexpress.com/w/wholesale-200-ohm-1-watt-resistor.html) |
| **Pull-down Resistors** | 10kΩ Resistors | 100 | $0.99 | [AliExpress](https://www.aliexpress.com/w/wholesale-10k-pull-up-resistor.html) |
| **Bulk Capacitor** | 470µF 16V/25V Electrolytic Capacitor (Main battery filtering) | 4 | $1.51 | [AliExpress](https://www.aliexpress.com/w/wholesale-470uf-capacitor.html) |
| **Capacitors** | 100µF & 10µF Assortment (Decoupling/Filtering) | 1 kit | $1.50 | [AliExpress](https://www.aliexpress.com/w/wholesale-smd-capacitor-assortment.html) |
| **Voltage Regulator** | XC6206P332MR 3.3V LDO Voltage Regulator (SOT-23) | 20 | $1.91 | [AliExpress](https://www.aliexpress.com/item/32811070407.html) |
| **Prototyping Board** | Double-sided Perfboard (2x8cm) | 10 | $1.20 | [AliExpress](https://www.aliexpress.com/w/wholesale-double-sided-perfboard.html) |
| **Wiring** | 30AWG Flexible Silicone Wire Kit | 1 kit | $3.50 | [AliExpress](https://www.aliexpress.com/w/wholesale-30awg-silicone-wire-kit.html) |
| **Battery** | 1S 3.7V 500mAh LiPo Battery | 1 | $7.00 | [AliExpress](https://www.aliexpress.com/item/1005003268560510.html) |
| **Airframe** | 3D Printed Frame Service (SLA/Resin) | 1 | $1.00 | [JLCPCB](https://jlcpcb.com/3d-printing) |
| **Total** | | | **$30.81** | |

## Wiring diagram
<img width="2560" height="1809" alt="WhatsApp Image 2026-08-20 at 5 38 04 PM" src="https://github.com/user-attachments/assets/42ad7b27-c5f9-4a6a-9117-ecfe9bc163bb" />


## Programming
*Software required*:-
1) Arduino IDE
2) ESP32C3 libaries
3) MPU6050 library
4) BLE libraries
*Customizing the P, I and D gains*:-
In my opinion the only method for obtaining the right gains is just trial error; because everydrone, even if it foollows the same blueprint, will have a different COM and mass. The following guide can be quite helpful in determining the gains:-
<img width="1600" height="412" alt="WhatsApp Image 2026-08-20 at 11 23 37 AM" src="https://github.com/user-attachments/assets/db75dc49-ebe8-4e69-a13f-405428662d1a" />
Furthermore, I think it is also prudent to add the ability to change the PID gains wirelessly as plugging the drone in repeatedly can be quite annoying. I did this by adding a BLE characteristic to wirelessly send the commands from my phone using nrf connect.
