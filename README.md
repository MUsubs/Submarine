# Submarine
>[!NOTE]
> You are currently viewing the [main](https://github.com/MUsubs/Submarine/tree/main) branch, here you can read information about the project, and the repository and file structure should you want to learn more about the project Source Code, or want to reproduce the product.

The Submarine project is a combination of many components, divided into three categories:
1. Autonomous Navigation with Computer Vision (NAV), a land-side portion consisting of a Computer Vision model which tracks the position of the Submarine, as well as being part of the main control application.
2. Submarine Control (ASN), a submarine-side portion in charge of all the actuators in the Submarine, acting on commands gotten from Autonomous Navigation to make the Submarine sail.
3. Data Transmission and Gathering (SEN), the bridge between land- and submarine-side components using IR data transmission, as well as commanding the sensors aboard the vessel.

# NAV
Project Source on branch [Land-Main](https://github.com/MUsubs/Submarine/tree/Land-Main) with C++ namespace `nav`

Microcontroller (C++, RP2040 + FreeRTOS) code in [Land-Main Submarine](https://github.com/MUsubs/Submarine/tree/Land-Main) folder, Desktop application (Python, native) code in [Land-Main Submarine/desktop](https://github.com/MUsubs/Submarine/tree/Land-Main/desktop) folder.

# ASN
Project Source on branch [Sub-Main](https://github.com/MUsubs/Submarine/tree/Sub-Main) with C++ namespace `asn`

Microcontroller (C++, RP2040 + FreeRTOS) code in [Sub-Main Submarine](https://github.com/MUsubs/Submarine/tree/Sub-Main) folder

# SEN
Project Source on branch [Land-Main](https://github.com/MUsubs/Submarine/tree/Land-Main) and [Sub-Main](https://github.com/MUsubs/Submarine/tree/Sub-Main) with C++ namespace `sen`

Microcontroller (C++, RP2040 + FreeRTOS) code in [Land-Main Submarine](https://github.com/MUsubs/Submarine/tree/Land-Main) and [Sub-Main Submarine](https://github.com/MUsubs/Submarine/tree/Sub-Main) folders
