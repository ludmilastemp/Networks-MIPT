from netfilterqueue import NetfilterQueue
from scapy.all import *
import argparse
import argparse
import json
import os
from typing import Dict, Any, List

song = [
    "Never.gonna.give.you.up",
    "Never.gonna.let.you.down",
    "Never.gonna.run.around.and.desert.you",
    "Never.gonna.make.you.cry",
    "Never.gonna.say.goodbye",
    "Never.gonna.tell.a.lie.and.hurt.you"
]
ip_dst = ""
ip_dst_reverse = ""

def sing_song(pkt):
    print()
    
    payload = pkt.get_payload()
    scapy_pkt = IP(payload)
    ip = scapy_pkt[IP]
    udp = scapy_pkt[UDP]

    if (ip.dst == ip_dst):
        print("send icmp")
        print("ttl = ", ip.ttl)
        pkt.drop()

        if ip.ttl == len(song) + 1:
            print("unreachable")
            send(IP(src=ip.dst, dst=ip.src)/ICMP(
                type="dest-unreach",
                code="port-unreachable",
            )/ip/udp)
            return
        
        print("time-exceeded")
        send(IP(src="1.1.1." + str(10 + ip.ttl), dst=ip.src)/ICMP(
            type="time-exceeded",
            code="ttl-zero-during-transit",
        )/ip/udp)
        return
    
    if not scapy_pkt.haslayer(DNS):
        pkt.accept()
        return

    dns = scapy_pkt[DNS]

    if (dns.qdcount != 1):
        pkt.accept()
        return
    
    if (dns.qd[0].qname.decode('utf-8') == ip_dst_reverse + ".in-addr.arpa."):
        print("send dns")
        print("finally")

        pkt.drop()
        answer = DNSRR(
                rrname=dns.qd.qname,
                type="PTR",
                rclass="IN",
                ttl=3600,
                rdata="Never.gonna.give.you.up"
            )

        send(IP(src=ip.dst, dst=ip.src)/UDP(
            sport=udp.dport,
            dport=udp.sport,
        )/DNS(
            id=dns.id,
            qr=1,
            ancount=1,
            an=answer,
        ))

        return

    qname = dns.qd[0].qname.decode('utf-8')

    if qname.split('.') == None:
        pkt.accept()
        return
    
    if not qname.split('.')[0].isdigit():
        pkt.accept()
        return
    
    count = int(qname.split('.')[0])

    if (qname != str(count) + ".1.1.1.in-addr.arpa."):
        pkt.accept()
        return
    
    print("send dns")
    print("count = ", count)
    pkt.drop()
    answer = DNSRR(
            rrname=dns.qd.qname,
            type="PTR",
            rclass="IN",
            ttl=3600,
            rdata=song[count - 10 - 1]
        )

    send(IP(src=ip.dst, dst=ip.src)/UDP(
        sport=udp.dport,
        dport=udp.sport,
    )/DNS(
        id=dns.id,
        qr=1,
        ancount=1,
        an=answer,
    ))

    print()
    return
    

def get_rules():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '-q', '--queue',
        type=int,
        default=0,
        choices=range(0, 16),
        help='Netfilter queue number 0-15 (default: 0)'
    )
    
    parser.add_argument(
        '--ip',
        type=str,
        default="162.252.205.157",
        help='ip address'
    )
    
    args = parser.parse_args()
    return args

args = get_rules()
ip_dst = args.ip
ips = ip_dst.split('.')
ip_dst_reverse = ips[3] + "." + ips[2] + "." + ips[1] + "." + ips[0]
nfqueue = NetfilterQueue()
nfqueue.bind(args.queue, sing_song)
try:
    nfqueue.run()
except KeyboardInterrupt:
    print('')

nfqueue.unbind()