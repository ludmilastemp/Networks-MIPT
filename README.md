# Server DB

Сервер базы данных типа "ключ-значение", работающий по протоколам TCP или UDP.

## Сборка

Проект собирается с помощью системы сборки Make.

```bash
git clone --branch server-db https://github.com/ludmilastemp/Networks-MIPT.git
cd Networks-MIPT
make
```

## Запуск

### TCP
Запуск TCP сервера:
```bash
./db.out server tcp
```
Запуск TCP клиента:
```bash
./db.out client tcp
```

### UDP
Запуск UDP сервера:
```bash
./db.out server udp
```
Запуск UDP клиента:
```bash
./db.out client udp
```

## Описание протокола

Сервер поддерживает два типа операций: чтение `get` и запись `set` данных. Данные хранятся в виде пар "ключ-значение". Все команды передаются в виде текстовых строк в кодировке *UTF-8*, завершающихся символом перевода строки `\n`.

Чтение значения по ключу `<KEY>`:
```
get <KEY>
```
Запись значения `<VALUE>` по ключу `<KEY>`:
```
set <KEY> <VALUE>
```

## Запись трафика
 
Записи трафика TCP и UDP взаимодействия для теста `TestGetSetGet*()` расположены в `cap/`.
