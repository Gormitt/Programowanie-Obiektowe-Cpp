#include <iostream>
#include <stdlib.h>

#define DL_SLOWA 20
#define LICZBA_CZUJNIKOW 4
#define ERROR_ALOKACJA printf("error - blad podczas alokacji\n");
#define ERROR_PLIK_OTWARCIE printf("error - nie udalo sie otworzyc podanego pliku\n");
#define ERROR_PLIK_PUSTY printf("error - plik z danymi jest pusty\n");

struct pomiar {
	unsigned int nrPomiaru;
	unsigned int nrCzujnika;
	char data[DL_SLOWA];
	double temp;
	struct pomiar* nastepny;
};

struct pomiar* DodajElementListy(FILE* in) {
	struct pomiar* nowy = (struct pomiar*)malloc(sizeof(struct pomiar));
	if (nowy != NULL) {
		nowy->nastepny = NULL;

		unsigned int nrPomiaru;
		unsigned int nrCzujnika;
		char data[DL_SLOWA] = {'\0'};
		double temp;

		fscanf_s(in, "%d", &nrPomiaru);
		if (feof(in)) { // napotkana linia to koniec pliku
			return NULL;
		}

		fscanf_s(in, "%d", &nrCzujnika);
		fscanf_s(in, "%s", data, DL_SLOWA);
		fscanf_s(in, "%lf", &temp);

		nowy->nrPomiaru = nrPomiaru;
		nowy->nrCzujnika = nrCzujnika;
		nowy->temp = temp;
		strcpy_s(nowy->data, DL_SLOWA, data);

		return nowy;
	}
	else {
		ERROR_ALOKACJA
		exit(0);
	}
}

struct pomiar* DodajElementListy(int nrPomiaru, int nrCzujnika, char data[DL_SLOWA], double temp) {
	struct pomiar* nowy = (struct pomiar*)malloc(sizeof(struct pomiar));
	if (nowy != NULL) {
		nowy->nastepny = NULL;

		nowy->nrPomiaru = nrPomiaru;
		nowy->nrCzujnika = nrCzujnika;
		nowy->temp = temp;
		strcpy_s(nowy->data, DL_SLOWA, data);

		return nowy;
	}
	else {
		ERROR_ALOKACJA
		exit(0);
	}
}

struct pomiar* WczytajPlikDoListy(FILE* in) {
	struct pomiar* glowa = NULL;
	struct pomiar* ogon = NULL;
	while (!feof(in)) {
		if (glowa == NULL) {
			glowa = DodajElementListy(in);
			if (glowa != NULL) {
				ogon = glowa;
			}
			else { // jezeli funkcja chce dodac jako pierwszy element null to plik jest pusty
				ERROR_PLIK_PUSTY
				exit(0);
			}
		}
		else {
			ogon->nastepny = DodajElementListy(in);
			if (ogon->nastepny != NULL) {
				ogon = ogon->nastepny;
			}
		}
	}
	return glowa;
}

void WypiszListe(struct pomiar* glowa) {
	struct pomiar* tmp = glowa;
	if (tmp == NULL) {
		printf("info: lista jest pusta\n");
	}
	else {
		while (tmp) {
			printf("%d %d %s %lf\n", tmp->nrPomiaru, tmp->nrCzujnika, tmp->data, tmp->temp);
			tmp = tmp->nastepny;
		}
	}
}

void DealokujListe(struct pomiar* glowa) {
	struct pomiar* ogon = NULL;
	while (glowa) {
		ogon = glowa->nastepny;
		free(glowa);
		glowa = ogon;
	}
}

struct pomiar** UzupelnijCzujniki(struct pomiar* lista) {
	struct pomiar** czujniki = (struct pomiar**)calloc(sizeof(struct pomiar*), LICZBA_CZUJNIKOW);

	struct pomiar* tmp = lista;
	struct pomiar* glowy[LICZBA_CZUJNIKOW] = { NULL };
	struct pomiar* ogony[LICZBA_CZUJNIKOW] = { NULL };

	if (tmp != NULL && czujniki != NULL) {
		while (tmp) {
			int nr = tmp->nrCzujnika - 1;
			if (glowy[nr] == NULL) {
				glowy[nr] = DodajElementListy(tmp->nrPomiaru, tmp->nrCzujnika, tmp->data, tmp->temp);
				if (glowy[nr] != NULL) {
					ogony[nr] = glowy[nr];
				}
			}
			else {
				ogony[nr]->nastepny = DodajElementListy(tmp->nrPomiaru, tmp->nrCzujnika, tmp->data, tmp->temp);
				if (ogony[nr]->nastepny != NULL) {
					ogony[nr] = ogony[nr]->nastepny;
				}
			}
			czujniki[nr] = glowy[nr];

			tmp = tmp->nastepny;
		}
		return czujniki;
	}
}

void WypiszCzujniki(struct pomiar** czujniki) {

	for (int i = 0; i < LICZBA_CZUJNIKOW; i++) {
		printf("$ rekordy z czujnika nr: %d\n", i + 1);
		WypiszListe(czujniki[i]);
		putchar('\n');
	}
}

void DealokujCzujniki(struct pomiar** czujniki) {
	for (int i = 0; i < LICZBA_CZUJNIKOW; i++) {
		DealokujListe(czujniki[i]);
	}
	free(czujniki);
}

int main() {
	FILE* in;

	if (fopen_s(&in, "test.txt", "r") == 0 && in != NULL) {
		struct pomiar* lista = WczytajPlikDoListy(in);
		struct pomiar** czujniki = UzupelnijCzujniki(lista);

		WypiszCzujniki(czujniki);

		DealokujListe(lista);
		DealokujCzujniki(czujniki);
	}
	else {
		ERROR_PLIK_OTWARCIE
		exit(0);
	}

	return 0;
}

/*
TODO:
1. zmienic slowo kluczowe lista na pomiar, zeby potem odrozniac czujnik od pomiaru
bo dodanie do listy pomiarow jest inne niz dodanie do listy czujnikow
*/