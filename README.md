# NFQUEUE

## Сборка

```bash
git clone --branch NFQUEUE https://github.com/ludmilastemp/Networks-MIPT.git
cd Networks-MIPT
python3 py.py -r dns_rules.json -q 1 -v
```

## JSON Rule File Examples
```
[
  {
    "action": "ACCEPT",
    "name": "ya.ru",
    "ancount": 2
  },
  {
    "action": "DROP",
    "name": "ya.ru",
    "ttl": 200
  }
]
```