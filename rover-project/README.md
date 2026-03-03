## 3D-Printed Bluetooth RC Rover

A custom-built, remote-controlled rover featuring a 3D-printed chassis, powered by an **XIAO ESP32C3** and programmed in **C++**. This project involved designing the physical structure, writing the motor control firmware, and implementing a **BLE (Bluetooth Low Energy)** interface for smartphone control.

### Challenges & Technical Insights

- **Efficient Chassis Design:** I focused on a lean design to minimize 3D-printing material while creating dedicated internal routing to keep the electronics and wiring organized and secure.

- **Hardware-Software Logic:** Beyond basic code, I focused on how firmware drives physical movement. I implemented **Pulse Width Modulation (PWM)** to manage duty cycles, allowing for smooth, variable motor speeds rather than simple binary control.

- **Pivoting to BLE:** Since the ESP32C3 uses **Bluetooth Low Energy**, I moved away from standard Serial Bluetooth to learn BLE protocols. This involved setting up the rover as a server to broadcast services and handle real-time data packets from my phone.

- **Safety Fail-safes:** To prevent the rover from driving off if the connection dropped, I added a watchdog timer. If the hardware doesn't receive a command within a set window, it triggers an automatic "full stop."
- **Hardware Recovery:** After frying the original microcontroller during testing, I had to source a compatible replacement, **resolder the entire circuit**, and reconfigure the code's pin definitions to match the new hardware layout.

- 
- **Range Optimization:** To stabilize the connection, I integrated an **external antenna**. This simple hardware upgrade significantly boosted the signal reliability and operational range during final testing.
