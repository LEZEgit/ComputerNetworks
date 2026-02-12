from scapy.all import *

# Create an ICMP Echo Request Packet
packet = IP(dst="8.8.8.8") / ICMP()

# Send the packet and wait for a resposne
response = sr1(packet, timeout=2)

# Display the response
if response:
    response.show()
    print("="*20)

    # response and response.summary() print the same thing
    print("Response:", response)
    print("Response Summary:", response.summary())

    print("Reply from:", response[IP].src)
else:
    print("No response received.")
