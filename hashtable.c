/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) { 
  int result = 1;                   //Counter pre ascii hodnoty 
  int length = strlen(key);         //Zistime dlzku retazca a 
  for (int i = 0; i < length; i++) {// pouzijeme pocas prechadzania
    result += key[i];               //Scitame vsetky hodnoty znakov
  }                                 // v stringu a nakoniec urcime 
  return (result % HT_SIZE);        // index v poli pre ulozenie
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {     //Kazdu cast tabulky nastavime pocas
  for (int i = 0; i < HT_SIZE; i++) { // inicializacie ako prazdnu, teda
    (*table)[i] = NULL;               // indexujeme pomocou premennej 'i'
  }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  int index = get_hash(key);          //Ziskame index daneho klucu,
  ht_item_t *item = (*table)[index];  // vytiahneme z tabulky dany prvok
  while(item != NULL){                // a kontrolujeme ci ma rovnky kluc
    if(strcmp(item->key, key) == 0){  // ako hladame
      return item;                    //Ake sme nasli zhodu, vratime prvok
    }
    item = item->next;                // inak sa posuvame na dalsi prvok
    
  }
  return NULL;                        //Ak sme nic nenasli vratime NULL
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 * V případě úspěchu funkce tuto skutečnost indikuje návratovou hodnotou 
 * true, v opačném případě (např. při selhání alokace) vrací funkce false.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
bool ht_insert(ht_table_t *table, char *key, float value) {
  ht_item_t *item = ht_search(table, key);//Opat pouzijeme funkciu search pre
  if(item != NULL){                       // vyhladanie prvku v tabulke, ak 
    item->value = value;                  // existuje iba zmenime jeho hodnotu
    return true;                          // a vratime true
  }   
  ht_item_t *newItem = (ht_item_t*)malloc(sizeof(ht_item_t)); //Inak ak prvok 
  if(newItem != NULL){                    // neexistuje, vytvorime ho (allokujeme),
    newItem->key = key;                   // nastavime jeho kluc, hodnotu a opat
    newItem->value = value;               // ziskame index pomocou funkcie get_hash
    int index = get_hash(newItem->key);   // pre nastavenie naslednika nasho noveho
    newItem->next = (*table)[index];      // prvku, potom uz len vlozime nas novy 
    (*table)[index] = newItem;            // prvok a vratime true
    return true;
  }
  return false;
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) { 
  ht_item_t *item = ht_search(table, key);  //Ziskame prvok na zaklade zadaneho klucu
  if(item != NULL){                         //Ak existuje vratime jeho hodnotu ak nie
    return &item->value;                    // vratime NULL
  }
  return NULL;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  int index = get_hash(key);             //Opat ziskame index prvku v tabulke, 
  ht_item_t *current = (*table)[index];  // vytvorime si pomocne prvky: aktualny
  ht_item_t *previous = NULL;            // podla indexu a predchodcu ktroeho neskor
                                         // vyuzijeme pre prepojenie po vymazani
  while(current != NULL){                //Ak dany prvok existuje, skontrolujeme
    if(strcmp(current->key, key)==0){    // jeho kluc so zadanym, ak su zhodne,
      if(previous != NULL){              // skontrolujeme ci sa jedna o prvy ,
        previous->next = current->next;  // ak nie, musime prepojit naslednika mazaneho
      }                                  // aby nan ukazoval naslednik prechodcu
      else{                              //Ak bol prvy, iba ho vymazeme teda tuto poziciu
        (*table)[index] = current->next; // nahradime nasledovnikom mazaneho
      }
      free(current);
      return;
    }
    previous = current;                  //Ak sa dane kluce nerovnaju, hladame dalej
    current = current->next;             // teda prechodca sa stane aktualnym a aktualny
  }                                      // sa posunie dalej, ak funkcia nenajde hladany
}                                        // prvok, jednoducho skonci bez vysledku

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
  for(int i = 0; i < HT_SIZE; i++){      //Prejdeme cez celu tabulku, vezmeme
    ht_item_t *item = (*table)[i];       // kazdy prvok, a dokym obsahuje nejake
    while(item != NULL){                 // prvky tak ich postupne uvolnujeme a
      ht_item_t *next = item->next;      // nakoniec aj samotny prvok tabulky
      free(item);
      item = next;
    }
    (*table)[i] = NULL;                 
  }
}
