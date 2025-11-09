from netfilterqueue import NetfilterQueue
from scapy.all import *
import argparse
import argparse
import json
import os
from typing import Dict, Any, List

def check_rules(pkt):
    print()
    print()
    ip = IP(pkt.get_payload())
    if not ip.haslayer(DNS): 
        pkt.accept()
        return
    
    dns = ip[DNS]
    ancount = dns.ancount
    print(f"ancount: {ancount}")
    for an in dns.an:
        name = an.rrname.decode()[0:-1]
        type = an.type
        ttl =  an.ttl
        rdata = an.rdata
        print()
        print(f"name: {name}")
        print(f"type: {type}")
        print(f"ttl: {ttl}")
        print(f"rdata: {rdata}")

        for rule in rules_config['rules']:
            if 'ancount' in rule and dns.ancount > rule['ancount']:
                continue
            if 'ttl' in rule and an.ttl > rule['ttl']:
                continue
            if 'name' in rule and an.rrname.decode()[0:-1] != rule['name']:
                continue
            if 'type' in rule and an.type != rule['type']:
                continue
            if 'rdlen' in rule and an.rdlen != rule['rdlen']:
                continue

            if rule['action'] == 'ACCEPT':
                print("packet accept")
                pkt.accept()
            if rule['action'] == 'DROP':
                print("packet drop")
                pkt.drop()
            return

        print()
        print()

    print("packet accept (default)")
    pkt.accept()
    return

class JSONRuleParser:
    def __init__(self):
        self.valid_actions = {'ACCEPT', 'DROP'}
        self.valid_fields = {'ancount', 'name', 'type', 'ttl', 'rdlen'}
    
    def parse_rule_file(self, filename: str) -> Dict[str, Any]:
        rules = []
        
        if not os.path.exists(filename):
            return {'rules': rules, 'errors': [f"File not found: {filename}"]}
        
        try:
            with open(filename, 'r') as f:
                content = f.read().strip()
            
            if not content:
                return {'rules': rules, 'errors': ["File is empty"]}
            
            try:
                rules_data = json.loads(content)
            except json.JSONDecodeError as e:
                return {'rules': rules, 'errors': [f"Invalid JSON: {e}"]}
            
            errors = []
            for i, rule_data in enumerate(rules_data):
                rule_errors = self._validate_rule(rule_data, i)
                
                if rule_errors:
                    errors.extend(rule_errors)
                else:
                    rules.append(rule_data)
            return {'rules': rules, 'errors': errors}
            
        except Exception as e:
            return {'rules': rules, 'errors': [f"Error parsing file: {e}"]}
    
    def _validate_rule(self, rule_data: Dict, index: int) -> List[str]:
        errors = []
        
        if not isinstance(rule_data, dict):
            return [f"Rule {index + 1}: Must be a JSON object"]
        
        action = rule_data.get('action', '').upper()
        if action not in self.valid_actions:
            errors.append(f"Rule {index + 1}: Invalid action '{action}'. Must be one of {list(self.valid_actions)}")
        
        for key in rule_data.keys():
            if key not in self.valid_fields and key != 'action':
                errors.append(f"Rule {index + 1}: Invalid field '{key}'. Valid fields: {list(self.valid_fields)}")
        
        if 'name' in rule_data and not isinstance(rule_data['name'], str):
            errors.append(f"Rule {index + 1}: 'name' must be a string")
        
        for field in ['ancount', 'type', 'ttl', 'rdlen']:
            if field in rule_data and not isinstance(rule_data[field], int):
                errors.append(f"Rule {index + 1}: '{field}' must be an integer")
        
        return errors

def get_rules():
    parser = argparse.ArgumentParser(
        description='DNS Interceptor with JSON rule-based filtering',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
JSON Rule File Examples:
[
    {
        "action": "ACCEPT",
        "name": "ya.ru",
        "type": 28
    },
    {
        "action": "DROP",
        "name": "malicious.com"
    }
]
        '''
    )
    
    parser.add_argument(
        '-r', '--rules',
        type=str,
        default='dns_rules.json',
        help='Path to JSON rule file (default: dns_rules.json)'
    )
    
    parser.add_argument(
        '-q', '--queue',
        type=int,
        default=0,
        choices=range(0, 16),
        help='Netfilter queue number 0-15 (default: 0)'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    rule_parser = JSONRuleParser()
    rules_config = rule_parser.parse_rule_file(args.rules)
    
    if rules_config.get('errors'):
        print("Rule validation errors:")
        for error in rules_config['errors']:
            print(f"  - {error}")
        
    print("JSON configuration loaded successfully!")
    if args.verbose:
        print(f"Queue number: {args.queue}")
        print(f"Rules file: {args.rules}")
        print(f"Successfully loaded {len(rules_config['rules'])} rules")
        
        if rules_config['rules']:
            print("\nLoaded rules:")
            for i, rule in enumerate(rules_config['rules']):
                print(f"  Rule {i+1}: {rule}")
    
    return args, rules_config

args, rules_config = get_rules()
nfqueue = NetfilterQueue()
nfqueue.bind(args.queue, check_rules)
try:
    nfqueue.run()
except KeyboardInterrupt:
    print('')

nfqueue.unbind()