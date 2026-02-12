from scapy.all import *

# Sniff 5 packets and print summary
packets = sniff(count=5)
print(packets.show())
