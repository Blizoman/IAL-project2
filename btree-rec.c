/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree)
{ 
  *tree = NULL;     //Nastavime strom ako prazdny teda NULL
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{ 
  if(tree == NULL){         //Ak je strom prazdny, nemame co hladat
    return false;           // vratime jednoducho false
  }
  else if(tree->key == key){//Ak sa kluc zhoduje s hladanym klucom, 
    *value = &tree->content;// ulozime si ukazatel na jeho content
    return true;            // do value a vratime true
  }
  else if(tree->key > key){ //Ak je hladany kluc mensi ako kluc korena
    return bst_search(tree->left, key, value); // presunieme sa do 
  }                         // laveho podstromu a opakujeme proces
  else if(tree->key < key){ //Ak je hladany kluc vacsi ako kluc korena
    return bst_search(tree->right, key, value);// presunieme sa do 
  }                         // praveho podstromu a opakujeme proces
  return false;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel. V případě úspěchu funkce tuto skutečnost
 * indikuje návratovou hodnotou true, v opačném případě (např. při selhání 
 * alokace) vrací funkce false.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
bool bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{ 
  if(*tree == NULL){    //Ak je strom empty tak ho rovno vlozim 
    bst_node_t *newNode = (bst_node_t*)malloc(sizeof(bst_node_t));
    if(newNode == NULL){//Vytvorime uzol (allokujeme) a overime
      return false;     // ci sa allokacia podarila
    }
    *tree = newNode;    //Nastavime nas novy (prvy/jediny) uzol ako koren
    newNode->key = key; // a nastavime jeho kluc a hodnotu ktore sme dostali
    newNode->content = value; // ako parametre funkcie a taktiez jeho pravy
    newNode->right = NULL;    // a lavy podstrom ktore su NULL kedze v strome
    newNode->left = NULL;     // okrem nasho uzlu iny nie je a vratime true
    return true;            
  }
  else if((*tree)->key == key){ //Inak ak sa kluc rovna nejakemu klucu uz 
    if ((*tree)->content.value != NULL) {
        free((*tree)->content.value); // uvolnieme staru hodnotu
    }
    (*tree)->content = value;   // existujuceho uzlu, nahradime ho a vratime
    return true;                // opat true
  }
  else if((*tree)->key > key){  //Ak je kluc hladany kluc mensi ako root key
    return bst_insert(&(*tree)->left, key, value); // urcite budeme pridavat
  }                             // do laveho podstromu
  else if((*tree)->key < key){  //Opat ak je hladany kluc vacsi ako kluc korenu
    return bst_insert(&(*tree)->right, key, value);// budeme pridavat do praveho
  }                             // podstromu

  return false;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  if((*tree)->right != NULL){   //Dokym existuje njeaky dalsi pravy uzol opakujeme
    bst_replace_by_rightmost(target, &(*tree)->right);// proces
  }
  else{ //Ak sme nasli "najpravejsi" ulozime jeho key a content do cieloveho uzlu
    target->key = (*tree)->key; // a bezpecne sa ho zbavime
    if (target->content.value != NULL) {   // <-- uvolni staru hodnotu cieloveho uzla
      free(target->content.value);
    }
    target->content = (*tree)->content;
    
    bst_node_t *tmp = *tree;  // Mazanie posledneho uzlu
    *tree = (*tree)->left;
    free(tmp);
  }
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  if(*tree == NULL){  //Ak je strom prazdny, nie je co odstranovat
    return;
  }
  else{
    if((*tree)->key < key){ //Inak opat prehladavame podla hodnoty klucu
      bst_delete(&(*tree)->right, key); // ak je hladany vacsi, ideme do 
    }                       // praveho podstromu, ak je mensi ideme do 
    else if((*tree)->key > key){        // laveho podstromu a rekurzivne
      bst_delete(&(*tree)->left, key);  // opakujeme proces volania bst_delete
    }
    else if((*tree)->key == key){ //Ak je hladany kluc rovnaky aktualnemu uzlu
      if((*tree)->left == NULL && (*tree)->right == NULL){  //Skontorlujeme pocet
        if((*tree)->content.value != NULL){
          free((*tree)->content.value);
        }
        free(*tree);              // potomkov, pripad A nema ani jedneho teda ide
        *tree = NULL;             // o list tak ho jednoducho odstranime a nastavime
      }                           // tento konrketny podstrom ako neexistujuci
      //Ak ma iba laveho potomka, prepojime otcovsky na nadchadzajuci vlavo a
      else if((*tree)->right == NULL && (*tree)->left != NULL){// uvolnime
        bst_node_t *tmp = *tree;
        if(tmp->content.value != NULL){
          free(tmp->content.value);
        }
        *tree = (*tree)->left;
        free(tmp);
      }//Ak ma iba laveho potomka, prepojime otcovsky na nadchadzajuci vpravo a
      else if((*tree)->right != NULL && (*tree)->left == NULL){// uvolnime
        bst_node_t *tmp = *tree;
        if(tmp->content.value != NULL){
          free(tmp->content.value);
        }
        *tree = (*tree)->right;
        free(tmp);
      }
      else{ //Ak ma oboch potomkov, voalme pomocnu funkciu bst_replace_by_rightmost
        bst_replace_by_rightmost(*tree, &(*tree)->left);
      }
    }
  }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  if(*tree != NULL){              //Ak strom nie je prazdny, rekurzivne
    bst_dispose(&(*tree)->left);  // budeme uvonovat kazdy podstrom dokym
    bst_dispose(&(*tree)->right); // sa neukonci podmienka ze strom je 
    if ((*tree)->content.value != NULL) {
      free((*tree)->content.value);
    }
    free(*tree);                  // uplne prazdy
    *tree = NULL;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{ //root, left, right
  if(tree != NULL){ //Opat skontrolujeme ci strom nie je prazdny a vlozime
    bst_add_node_to_items(tree, items); // najprv otcovsky uzol, potom 
    bst_preorder((tree)->left, items);  // prejdeme do laveho podstromu a 
    bst_preorder((tree)->right, items); // nakoniec do praveho podstromu
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{ //left, root, right
  if(tree != NULL){ //Kontrola ci strom nie je empty, ak nie tak najprv 
    bst_inorder((tree)->left, items);  // prejdeme lavy podstrom potom
    bst_add_node_to_items(tree, items);// vlozime koren do items a nakoniec
    bst_inorder((tree)->right, items); // pravy podstrom toto rekurzivne 
  }                                    // opakujeme
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{ //left, right, root
  if(tree != NULL){ //Kontrola ci strom nie je empty, ak nie je zacneme
    bst_postorder((tree)->left, items); // lavym podstromom, potom ideme 
    bst_postorder((tree)->right, items);// pravy spracovat a az na koniec
    bst_add_node_to_items(tree, items); // pridame aktualny root do items
  }
}
