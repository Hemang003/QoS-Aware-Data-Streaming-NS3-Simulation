#!/usr/bin/env python3
"""
analyze_qos.py
---------------
Parses NS-3 FlowMonitor XML (from qos_lte_streaming.cc)
and extracts QoS metrics into a CSV file.

Usage:
    python3 analyze_qos.py qos-results.xml
"""

import sys
import xml.etree.ElementTree as ET
import pandas as pd

def parse_qos(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    rows = []
    for flow in root.find("FlowStats").findall("Flow"):
        fid = int(flow.attrib.get("flowId", -1))
        tx = int(flow.attrib.get("txPackets", 0))
        rx = int(flow.attrib.get("rxPackets", 0))
        lost = int(flow.attrib.get("lostPackets", 0))
        txBytes = int(flow.attrib.get("txBytes", 0))
        rxBytes = int(flow.attrib.get("rxBytes", 0))
        delaySumNs = float(flow.attrib.get("delaySum", "+0ns").replace("+", "").replace("ns", ""))
        tFirstTx = float(flow.attrib.get("timeFirstTxPacket", "+0ns").replace("+", "").replace("ns", ""))
        tLastRx = float(flow.attrib.get("timeLastRxPacket", "+0ns").replace("+", "").replace("ns", ""))

        # duration in ns
        durationNs = max(0.0, tLastRx - tFirstTx)

        # compute metrics
        throughputKbps = (rxBytes * 8.0 / durationNs * 1e9 / 1024) if durationNs > 0 else 0.0
        avgDelayMs = (delaySumNs / rx / 1e6) if rx > 0 else 0.0
        lossRate = (lost / tx * 100.0) if tx > 0 else 0.0

        rows.append({
            "Flow ID": fid,
            "Tx Packets": tx,
            "Rx Packets": rx,
            "Lost Packets": lost,
            "Throughput (Kbps)": round(throughputKbps, 3),
            "Avg Delay (ms)": round(avgDelayMs, 6),
            "Packet Loss Rate (%)": round(lossRate, 3)
        })

    return pd.DataFrame(rows)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 analyze_qos.py qos-results.xml")
        sys.exit(1)

    xml_file = sys.argv[1]
    df = parse_qos(xml_file)

    out_csv = "qos_metrics.csv"
    df.to_csv(out_csv, index=False)
    print(f"✅ Metrics saved to {out_csv}\n")
    print(df)

