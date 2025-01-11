### EE193 Networked Embedded Systems Final Project: Campus-Wide Wireless Temperature Monitoring Network

#### Project Overview
This project, developed as part of an advanced embedded systems course at Tufts University, focuses on designing and deploying a wireless network of temperature monitoring probes across campus. The network collects and transmits ambient temperature data in real-time, supporting environmental and facility management efforts. Key project highlights include scalability, low-cost design, and robust performance in varying environmental conditions.

#### Features
- **Data Collection**: Hourly temperature measurements with timestamps accurate to within five minutes.
- **Precision**: Readings accurate to within ±2°F.
- **Data Transmission**: MQTT protocol ensures efficient and reliable communication with a campus server.
- **Power Efficiency**: Probes operate autonomously for up to six months on a LiPo battery.
- **Environmental Durability**: The system withstands various weather conditions, including rain, ice, and wind.
- **Cost-Effective Design**: Each unit costs less than $15 to produce at scale (1,000 units).

#### Technical Highlights
- **Microcontroller**: ESP32-C3-WROOM module for low power consumption and WiFi capability.
- **Sensor**: PCT2075DP,118 temperature sensor for high accuracy and low energy usage.
- **Custom PCBs**: Designed using KiCAD for compact integration and efficient performance.
- **Software**: ESP-IDF framework and Arduino were used for programming, with MQTT and WiFi functionalities implemented.
- **Dashboard**: A custom-built web interface for real-time data visualization and analysis.

#### Deployment
Three nodes were deployed across the Tufts Medford/Somerville campus. These 3 nodes were operational from May 2024 to January 2025. Unfortunately, as of January 2025, they have been removed by Tufts and are no longer operational. 

#### Lessons Learned & Future Improvements
- **Lessons Learned**: 
  - Ensure compatibility of programming boards with microcontrollers.
  - Perform continuity tests on all boards before use.
- **Potential Improvements**: 
  - Transition from LiPo batteries to solar panels for eco-friendly, self-sustaining power.
  - Explore alternative sensors and ESP modules to further enhance power efficiency.

#### Repository Content & Helpful Links
- **Writeup**: For a detailed writeup of this project, including pictures of our KiCAD schematics and snippets of code, visit this link: https://www.notion.so/lindazhao/Team-Fire-Nation-Final-Report-dc0f7105e2b34866ada27040306ecb55?pvs=4.
- **Schematics and PCB Layouts**: A KiCAD zip folder of everything can be found here: https://drive.google.com/file/d/1rzjhcRprmjvbViDM0Jm00A539Co3ytU3/view?usp=sharing.
- **BOM**: We were the only team who managed to get within $5 of the goal cost of $15. Our BOM can be found at this link: https://docs.google.com/spreadsheets/d/19IVyPjfXpfomtAPkblObAk700TYVvV6gad1exjRFkIU/edit?usp=sharing.
- **Code**: Source code for ESP32 nodes, including MQTT and WiFi functionalities, can be found in the `src` directory in our `main` branch.
- **Arduino Test Code**: We had trouble implementing our project with the espressidf code at first, so we tested functionality of our hardware and simulated a working sensor with the Arduino code found at this link: https://github.com/gmbeddard/TeamF_Final_Arduino.
- **Dashboard**: Interface designs and implementations are located in on my teammate Linda's Github at https://github.com/lindazha0/dashboard-server.
- **Programming/Debugger Boards**: We primarily used the Digikey ESP Programmer Board, found at this link: https://www.digikey.com/en/products/detail/espressif-systems/ESP-PROG/10259352?utm_adgroup=&utm_source=google&utm_medium=cpc&utm_campaign=PMax.

#### Authors
I'd like to thank my lovely teammates on Team Fire Nation: Emily Stanisha (@estanisha) and Chuyi (Linda) Zhao (@lindazha0). Together, we were one of the only teams in EE193 to get all 3 sensors working and recording data. 

Thanks for reading!
