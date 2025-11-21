# TLS

## Сборка

Проект собирается с помощью системы сборки Make.

```bash
git clone --branch TLS https://github.com/ludmilastemp/Networks-MIPT.git
cd Networks-MIPT
make
```

## Генерация сертификата 
```
openssl req -new -newkey rsa:4096 -x509 -sha256 -days 365 -nodes -out cert/crt.crt -keyout cert/key.key
```

## Запуск

### TCP
Запуск TCP сервера:
```bash
./tls.out server --addr 1.1.1.1 --port 1234 --crt cert/crt.crt --key cert/key.key
```
Запуск TCP клиента:
```bash
./tls.out client --addr 1.1.1.1 --port 1234
```
