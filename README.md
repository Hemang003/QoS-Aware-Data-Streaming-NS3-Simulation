# QoS-Aware Data Streaming in Mobile Networks (NS-3 Simulation)

This project uses **NS-3 LTE module** to simulate UDP video streaming with mobile UEs.
It measures **throughput, delay, and packet loss** as the UE moves at different speeds.

## Folder structure
- `src/qos_lte_streaming.cc` → NS-3 simulation source code
- `scripts/analyze_qos.py` → Python parser for FlowMonitor XML results
- `results/` → Stores generated XML/CSV results (ignored in GitHub)

## How to run
1. Copy `src/qos_lte_streaming.cc` into your NS-3 `scratch/` folder.
2. Build NS-3:
   ```bash
   ./waf build



