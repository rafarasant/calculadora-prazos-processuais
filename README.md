# Calculadora de Prazos Processuais

Programa em C para calcular datas de vencimento de prazos processuais com base no Código de Processo Civil (CPC/2015 — LEI Nº 13.105, DE 16 DE MARÇO DE 2015).

## Sobre

Este projeto oferece uma calculadora de prazos processuais que:

- lê a data de disponibilização da informação no DJe;
- considera o primeiro dia útil seguinte como data de publicação/intimação;
- começa a contar o prazo a partir do dia útil seguinte à intimação;
- exclui fins de semana e feriados nacionais fixos;
- inclui feriados móveis relacionados à Páscoa (Carnaval, Sexta-feira Santa e Corpus Christi).

## Como compilar

No terminal, execute:

```bash
gcc -o calculadora prazo_processual.c
```

## Como executar

No terminal, execute:

```bash
./calculadora
```

## Observações

- O programa usa o arquivo `prazo_processual.c` como fonte principal.
- A validação de datas é feita para anos entre 1900 e 2100.
- Feriados estaduais, municipais e pontos facultativos não são considerados.
