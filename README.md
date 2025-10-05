
# QoS-Aware Data Streaming in Mobile Networks (NS-3 Simulation)

This project simulates **QoS-Aware UDP video streaming** in LTE mobile networks using the **NS-3 network simulator**.  
It measures **Quality of Service (QoS)** parameters such as **Throughput**, **Average Delay**, and **Packet Loss Rate** while the **User Equipment (UE)** moves at different speeds.  

The goal is to understand how **mobility affects data streaming quality** — for example, why buffering or disconnections occur when moving farther from a base station.

---

##  Objective

The objective of this simulation is to:
- Study the effect of **UE speed** on network performance metrics.  
- Analyze **QoS behavior** (Throughput, Delay, Packet Loss) for mobile streaming.  
- Identify challenges in maintaining video streaming quality under mobility.

---

##  Requirements

Before running the project, make sure you have:

- **NS-3 (version ≥ 3.35)** installed  
- **Python 3.x**  
- Python dependencies:
  ```bash
  pip install pandas
A basic understanding of NS-3 command-line usage
 Project Structure

QoS-LTE-Simulation/
│
├── src/
│   └── qos_lte_streaming.cc       # NS-3 simulation C++ source code
│
├── scripts/
│   └── analyze_qos.py             # Python script to analyze FlowMonitor results
│
├── results/
│   ├── qos-results.xml            # Generated XML output (FlowMonitor)
│   └── qos_metrics.csv            # Processed QoS metrics (from Python)
│
├── .gitignore
├── README.md
└── LNSE




 How to Run the Simulation
 Step 1: Copy the simulation file
Copy qos_lte_streaming.cc to your NS-3 installation folder under scratch/:

bash
Copy code
cp src/qos_lte_streaming.cc ~/Downloads/ns-allinone-3.41/ns-3.41/scratch/
️ Step 2: Build NS-3
bash
Copy code
cd ~/Downloads/ns-allinone-3.41/ns-3.41
./waf build
️ Step 3: Run the simulation with different UE speeds
You can control the mobile user’s speed (in meters per second) using the --ueSpeed argument.

Examples:

bash
Copy code
./waf --run "scratch/qos_lte_streaming --ueSpeed=1.0"
./waf --run "scratch/qos_lte_streaming --ueSpeed=3.0"
./waf --run "scratch/qos_lte_streaming --ueSpeed=5.0"
Each run generates a FlowMonitor result file:
qos-results.xml

To save multiple runs, rename them:

bash
Copy code
mv qos-results.xml results/qos-results-1.xml
 Step 4: Analyze QoS Results (Python)
After each simulation, run the analysis script:

bash
Copy code
cd scripts
python3 analyze_qos.py ../results/qos-results-1.xml
The script:

Parses the XML result file

Extracts Throughput, Delay, and Packet Loss metrics

Saves output to results/qos_metrics.csv

 Results and Interpretation
Metric	Meaning	Observation
Throughput (Kbps)	Data successfully received per second	Decreases as UE moves faster due to weaker LTE signal
Average Delay (ms)	Time taken for packets to reach UE	Normally increases with speed, but may be zero if no packets received
Packet Loss Rate (%)	Percentage of lost packets	Increases with mobility; higher loss means unstable connection

 Real-World Analogy
At 1 m/s (walking), the connection is stable, similar to streaming on foot near a Wi-Fi router.

At 3–5 m/s (fast movement or vehicle), the UE moves out of LTE range faster, causing disconnections or buffering, like watching YouTube in a moving car with poor signal.

 Example Workflow
Run simulation for multiple speeds (1 m/s, 3 m/s, 5 m/s).

Parse XML results using the Python script.

Collect data in qos_metrics.csv.

Plot graphs like:

Throughput vs UE Speed

Average Delay vs UE Speed

Packet Loss vs UE Speed

Compare QoS trends to understand how mobility affects streaming quality.

📊 Results Preview
You can visualize your results using matplotlib or Excel:

python
Copy code
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/qos_metrics.csv")
plt.plot(df["UE Speed"], df["Throughput (Kbps)"])
plt.xlabel("UE Speed (m/s)")
plt.ylabel("Throughput (Kbps)")
plt.title("Throughput vs UE Speed")
plt.show()
Expected trend:

Throughput ↓ decreases as UE speed ↑ increases

Packet Loss ↑ increases with speed

Delay may fluctuate depending on signal quality and link duration

 Future Improvements
Implement handover between multiple eNodeBs for realistic mobility

Use adaptive bitrate streaming (ABR) for better realism

Add multiple UEs to study congestion and network fairness

Extend simulation time to observe long-term performance trends

️ Demo / Output Example
Sample terminal output from FlowMonitor:

yaml
Copy code
Flow 1 (1.0.0.1 -> 7.0.0.2)
  Tx Packets: 500
  Rx Packets: 280
  Throughput: 625.45 Kbps
  Delay Sum: 0.0123 s
  Lost Packets: 220
This demonstrates the flow statistics collected during the simulation.

📜 License
This project is licensed under the MIT License — see the LICENSE file for details.

© 2025 Hemang Parmar
All rights reserved.

Acknowledgments
This project was created as part of the ENGR 5720G – Pervasive and Mobile Computing course at Ontario Tech University.
Special thanks to the NS-3 development community for providing a robust simulation platform.



