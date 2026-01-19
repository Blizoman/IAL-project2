/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory ../iter/btree.c a ../rec/btree.c)
 * implementujte triviální funkci letter_count. Všimněte si, že výstupní strom může být značně degradovaný 
 * (až na úroveň lineárního seznamu). Jako typ hodnoty v uzlu stromu využijte 'INTEGER'.
 * 
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 * 
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case insensitive), znaku 
 * mezery ' ', a ostatních znaků (ve stromu reprezentováno znakem podtržítka '_'). Výstup je 
 * ukládán průběžně do stromu (klíč vždy lowercase). V případě úspěchu funkce tuto skutečnost
 * indikuje návratovou hodnotou true, v opačném případě (např. při selhání 
 * operace insert) vrací funkce false.
 * 
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce obsahovat:
 * 
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 * 
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce.
*/
bool letter_count(bst_node_t **tree, char *input) {
  bst_init(tree); //Inicializacia stromu

  for(int i = 0; input[i] != '\0'; i++){ // Prejdeme cely retazec a urcime
    char key_char;                       // o aky typ sa jedna, teda pismeno, white_space alebo
    if((input[i] >= 'A' && input[i]<= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){// podtrzitko
      if(input[i] >= 'A' && input[i] <= 'Z')
        key_char = input[i] - 'A' + 'a'; //Pismena su case-insensitive, takze
      else                               // ich pomocou ascii konvertujeme na 
        key_char = input[i];             // na male ak boli povodne velke
    }
    else if(input[i] == ' '){            //Pre WHITE_SPACE
      key_char = ' ';                    //Nastavime key_char 
    }
    else{                                //Pre znak '_'
      key_char = '_';                    //Nastavime key_char 
    }

    bst_node_content_t *found_content;   //Uzol pre hladanie v strome
    if(bst_search(*tree, key_char, &found_content)){//Ak dany uzol existuje...
      (*(int*)found_content->value)++;            //Zvysime len jeho hodnotu
    }                                    //Ak sme ho nenasli, musime ho pridat
    else{
      bst_node_content_t new_content;    //Inicializujeme kontent, upravime
      int *new_value = malloc(sizeof(int)); //ho a posleme insert funkcii
      *new_value = 1;                    //Allokujeme jeho value (kedze moze ist
      new_content.value = new_value;     // aj o ine types ako int)       
      new_content.type = INTEGER;
      if(!bst_insert(tree, key_char, new_content)){
        free(new_value);                 //Overime, ci sa vlozenie podarilo,
        return false;                    // ak nie uvolnime allokovanu value
      }                                  // a vratime false inak pokracujeme
    }                                    // dalsou polozkou v zadanom stringu
  }
  return true;
}