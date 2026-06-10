/*
 * ============================================================
 *  CONTAGEM DE PRAZOS PROCESSUAIS - CPC/2015
 *  Lei n. 13.105, de 16 de marco de 2015
 *
 *  Regras aplicadas:
 *    Art. 219 - Contagem em dias uteis
 *    Art. 224 - Exclui o dia do comeco (intimacao/publicacao)
 *               e inclui o dia do vencimento
 *    Art. 224, par.3 - Publicacao no DJe considera-se feita
 *                      no primeiro dia util seguinte ao da
 *                      disponibilizacao
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * FERIADOS NACIONAIS FIXOS (dia/mes)
 * ============================================================ */
typedef struct { int dia; int mes; } FeriadoFixo;

static const FeriadoFixo FERIADOS_FIXOS[] = {
    { 1,  1},  /* Confraternizacao Universal          */
    {21,  4},  /* Tiradentes                          */
    { 1,  5},  /* Dia do Trabalho                     */
    { 7,  9},  /* Independencia do Brasil             */
    {12, 10},  /* Nossa Senhora Aparecida             */
    { 2, 11},  /* Finados                             */
    {15, 11},  /* Proclamacao da Republica            */
    {20, 11},  /* Consciencia Negra (Lei 14.759/2023) */
    {25, 12}   /* Natal                               */
};
static const int NUM_FERIADOS_FIXOS =
    (int)(sizeof(FERIADOS_FIXOS) / sizeof(FERIADOS_FIXOS[0]));

/* ============================================================
 * ESTRUTURA DE DATA
 * ============================================================ */
typedef struct { int dia; int mes; int ano; } Data;

/* ============================================================
 * FUNCOES AUXILIARES DE DATA
 * ============================================================ */

int eh_bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int dias_no_mes(int mes, int ano) {
    static const int dias[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (mes == 2 && eh_bissexto(ano)) return 29;
    return dias[mes];
}

int data_valida(Data d) {
    if (d.ano < 1900 || d.ano > 2100) return 0;
    if (d.mes < 1 || d.mes > 12)      return 0;
    if (d.dia < 1 || d.dia > dias_no_mes(d.mes, d.ano)) return 0;
    return 1;
}

Data proximo_dia(Data d) {
    d.dia++;
    if (d.dia > dias_no_mes(d.mes, d.ano)) {
        d.dia = 1;
        d.mes++;
        if (d.mes > 12) { d.mes = 1; d.ano++; }
    }
    return d;
}

/* Dia da semana: 0=Dom, 1=Seg, 2=Ter, 3=Qua, 4=Qui, 5=Sex, 6=Sab */
int dia_da_semana(Data d) {
    int m = d.mes, y = d.ano;
    if (m < 3) { m += 12; y--; }
    int k = y % 100, j = y / 100;
    int h = (d.dia + (13*(m+1))/5 + k + k/4 + j/4 + 5*j) % 7;
    return ((h + 6) % 7);
}

Data adicionar_dias(Data d, int n) {
    int i;
    for (i = 0; i < n; i++) d = proximo_dia(d);
    return d;
}

Data subtrair_dias(Data d, int n) {
    int i;
    for (i = 0; i < n; i++) {
        d.dia--;
        if (d.dia < 1) {
            d.mes--;
            if (d.mes < 1) { d.mes = 12; d.ano--; }
            d.dia = dias_no_mes(d.mes, d.ano);
        }
    }
    return d;
}

/* Calcula a data da Pascoa pelo algoritmo de Butcher */
Data pascoa(int ano) {
    int a = ano % 19;
    int b = ano / 100;
    int c = ano % 100;
    int d = b / 4;
    int e = b % 4;
    int f = (b + 8) / 25;
    int g = (b - f + 1) / 3;
    int h = (19*a + b - d - g + 15) % 30;
    int i = c / 4;
    int k = c % 4;
    int l = (32 + 2*e + 2*i - h - k) % 7;
    int m = (a + 11*h + 22*l) / 451;
    int mes = (h + l - 7*m + 114) / 31;
    int dia = ((h + l - 7*m + 114) % 31) + 1;
    Data p;
    p.dia = dia; p.mes = mes; p.ano = ano;
    return p;
}

int eh_feriado(Data d) {
    int i;
    Data p, seg_carnaval, ter_carnaval, sex_santa, corpus;

    for (i = 0; i < NUM_FERIADOS_FIXOS; i++)
        if (d.dia == FERIADOS_FIXOS[i].dia && d.mes == FERIADOS_FIXOS[i].mes)
            return 1;

    p            = pascoa(d.ano);
    seg_carnaval = subtrair_dias(p, 48);
    ter_carnaval = subtrair_dias(p, 47);
    sex_santa    = subtrair_dias(p, 2);
    corpus       = adicionar_dias(p, 60);

    if (d.dia==seg_carnaval.dia && d.mes==seg_carnaval.mes) return 1;
    if (d.dia==ter_carnaval.dia && d.mes==ter_carnaval.mes) return 1;
    if (d.dia==sex_santa.dia    && d.mes==sex_santa.mes)    return 1;
    if (d.dia==corpus.dia       && d.mes==corpus.mes)       return 1;

    return 0;
}

int eh_fim_de_semana(Data d) {
    int ds = dia_da_semana(d);
    return (ds == 0 || ds == 6);
}

int eh_dia_util(Data d) {
    return !eh_fim_de_semana(d) && !eh_feriado(d);
}

/* ============================================================
 * LOGICA PRINCIPAL DE CONTAGEM
 * ============================================================ */
Data calcular_vencimento(Data disponibilizacao, int prazo_dias) {
    Data d;
    int contagem;

    /* 1) Dia da intimacao = 1o dia util apos disponibilizacao (art. 224, par.3) */
    d = proximo_dia(disponibilizacao);
    while (!eh_dia_util(d))
        d = proximo_dia(d);

    /* 2) Contagem comeca no dia seguinte ao da intimacao (art. 224, caput) */
    d = proximo_dia(d);
    contagem = 0;
    while (contagem < prazo_dias) {
        if (eh_dia_util(d)) contagem++;
        if (contagem < prazo_dias) d = proximo_dia(d);
    }
    return d;
}

/* ============================================================
 * MAIN
 * ============================================================ */
int main(void) {
    Data disponibilizacao, intimacao, inicio_contagem, vencimento;
    int opcao, prazo;
    const char *recurso_nome;
    const char *base_legal_prazo;
    static const char *NOME_DIA[] = {
        "domingo","segunda-feira","terca-feira",
        "quarta-feira","quinta-feira","sexta-feira","sabado"
    };
    char buf[32];

    printf("\n");
    printf("+--------------------------------------------------------------+\n");
    printf("|       CONTAGEM DE PRAZOS PROCESSUAIS - CPC/2015             |\n");
    printf("|         Lei n. 13.105, de 16 de marco de 2015               |\n");
    printf("+--------------------------------------------------------------+\n");
    printf("\n");

    /* --- Leitura da data --- */
    while (1) {
        int d, m, a;
        printf("  Data da Disponibilizacao da Informacao no DJe (dd/mm/aaaa): ");
        fflush(stdout);
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        if (sscanf(buf, "%d/%d/%d", &d, &m, &a) != 3) {
            printf("  Formato invalido. Use dd/mm/aaaa.\n\n");
            continue;
        }
        disponibilizacao.dia = d;
        disponibilizacao.mes = m;
        disponibilizacao.ano = a;
        if (!data_valida(disponibilizacao)) {
            printf("  Data inexistente no calendario. Tente novamente.\n\n");
            continue;
        }
        break;
    }

    /* --- Menu --- */
    printf("\n");
    printf("  Selecione o tipo de recurso:\n");
    printf("  +-----------------------------------------------------------+\n");
    printf("  | 1) Embargos de Declaracao           (art. 1.023, CPC)    |\n");
    printf("  | 2) Apelacao                         (art. 1.003, CPC)    |\n");
    printf("  | 3) Agravo de Instrumento            (art. 1.003, CPC)    |\n");
    printf("  | 4) Recurso Especial / Extraordinario (art. 1.003, CPC)   |\n");
    printf("  +-----------------------------------------------------------+\n");
    printf("  Opcao: ");
    fflush(stdout);
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &opcao);

    switch (opcao) {
        case 1:
            recurso_nome     = "Embargos de Declaracao";
            prazo            = 5;
            base_legal_prazo = "art. 1.023, caput, CPC/2015";
            break;
        case 2:
            recurso_nome     = "Apelacao";
            prazo            = 15;
            base_legal_prazo = "art. 1.003, par.5, CPC/2015";
            break;
        case 3:
            recurso_nome     = "Agravo de Instrumento";
            prazo            = 15;
            base_legal_prazo = "art. 1.003, par.5, CPC/2015";
            break;
        case 4:
            recurso_nome     = "Recurso Especial / Extraordinario";
            prazo            = 15;
            base_legal_prazo = "art. 1.003, par.5, CPC/2015";
            break;
        default:
            printf("\n  Opcao invalida. Encerrando.\n\n");
            return 1;
    }

    /* --- Calculo --- */
    vencimento = calcular_vencimento(disponibilizacao, prazo);

    /* Recalcula intimacao e inicio para exibicao */
    intimacao = proximo_dia(disponibilizacao);
    while (!eh_dia_util(intimacao))
        intimacao = proximo_dia(intimacao);
    inicio_contagem = proximo_dia(intimacao);

    /* --- Resultado --- */
    printf("\n");
    printf("+--------------------------------------------------------------+\n");
    printf("|                   RESULTADO DO CALCULO                      |\n");
    printf("+--------------------------------------------------------------+\n");
    printf("\n");
    printf("  Recurso : %s\n", recurso_nome);
    printf("  Prazo   : %d dias uteis (%s)\n", prazo, base_legal_prazo);
    printf("\n");
    printf("  REGRA DE CONTAGEM (CPC/2015)\n");
    printf("  ----------------------------------------------------------\n");
    printf("  Art. 219 - Os prazos sao contados em dias UTEIS,\n");
    printf("             excluindo sabados, domingos e feriados.\n");
    printf("\n");
    printf("  Art. 224 - Exclui-se o dia do comeco (publicacao/\n");
    printf("             intimacao) e inclui-se o dia do vencimento.\n");
    printf("\n");
    printf("  Art. 224, par.3 - A publicacao no DJe considera-se\n");
    printf("             realizada no 1o dia util seguinte a data\n");
    printf("             de disponibilizacao.\n");
    printf("  ----------------------------------------------------------\n");
    printf("\n");
    printf("  Disponibilizacao no DJe  : %02d/%02d/%04d (%s)\n",
           disponibilizacao.dia, disponibilizacao.mes, disponibilizacao.ano,
           NOME_DIA[dia_da_semana(disponibilizacao)]);
    printf("  Dia da Publicacao        : %02d/%02d/%04d (%s)\n",
           intimacao.dia, intimacao.mes, intimacao.ano,
           NOME_DIA[dia_da_semana(intimacao)]);
    printf("  Inicio da contagem       : %02d/%02d/%04d (%s)\n",
           inicio_contagem.dia, inicio_contagem.mes, inicio_contagem.ano,
           NOME_DIA[dia_da_semana(inicio_contagem)]);
    printf("\n");
    printf("  ----------------------------------------------------------\n");
    printf("  Portanto, o prazo vencera no %do dia util a partir\n", prazo);
    printf("  do dia seguinte ao da intimacao.\n");
    printf("\n");
    printf("  >>> VENCIMENTO: %02d/%02d/%04d (%s)\n",
           vencimento.dia, vencimento.mes, vencimento.ano,
           NOME_DIA[dia_da_semana(vencimento)]);
    printf("  ----------------------------------------------------------\n");
    printf("\n");
    printf("  ATENCAO: Este programa considera apenas os feriados nacionais\n");
    printf("  fixos e os moveis vinculados a Pascoa (Carnaval, Sexta-feira\n");
    printf("  Santa e Corpus Christi). Feriados estaduais, municipais e\n");
    printf("  pontos facultativos devem ser verificados separadamente.\n");
    printf("\n");

    return 0;
}
