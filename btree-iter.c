/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
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
{//Podobne ako pri rekurzivnom, inicializovany strom 
  *tree = NULL; // je prazdny
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{
  if(tree == NULL){   //Ak je strom prazdny, nemame co hladat, vratime
    return false;     // false
  }
  
  while(tree != NULL){        //Dokym existuje nejaky node, tak zistime
    if(tree->key == key){     // ci je jeho kluc rovnaky ako hladany...
      *value = &tree->content;// ak ano, do value ulozime hodnotu tohto
      return true;            // uzlu a vratime true
    }
    else if(tree->key > key){ // Ak je hladany kluc mensi, musime ist do
      tree = tree->left;      // laveho podstromu kde su mensie hodnoty
    }                         // naopak ak je hladany kluc vacsi, ideme
    else{                     // do praveho podstromu
      tree = tree->right;
    }
  }
  return false;               //Ak sme narazili na to ze strom tento uzol
}                             // nema, vratime false

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
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
bool bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{
  if(*tree == NULL){    //Ak je strom prazdny rovno vytvaram new a vkladam
    bst_node_t *newNode = (bst_node_t*)malloc(sizeof(bst_node_t));
    if(newNode == NULL){// ho do stromu, overim ci nezlyhal malloc a 
      return false;     // nastavim jeho kluc a content a taktiez
    }
    newNode->key = key;    // jeho ukazatele, tym ze strom bol prazdny,
    newNode->content = value; // potom nas vkladany uzol je prvy a 
    newNode->right = NULL; // jediny, tym padom nema ziadne dalsie
    newNode->left = NULL;  // podstromy
    *tree = newNode;       //Vytvorenie korenoveho (jedineho) uzlu
    return true;
  }
  bst_node_t **tatko = tree;  //Zachovanie stromu (resp root pre buduce podstromy)
  bst_node_t *current = *tree;//Aktualny node
  while(current != NULL){
    if(current->key == key){   //Ak sme nasli zhodu prepiseme hodnotu uzlu
      if (current->content.value != NULL) {   
        free(current->content.value);
      }
      current->content = value;// a vratime true
      return true;
    }
    else if(current->key < key){//Ak je hladany kluc vacsi pojdeme do praveho
      tatko = &current->right;  // podstromu, prv vsak aktualizujeme tatkovsky 
      current = current->right; // uzol a az potom posunieme current dalej
    }
    else{
      tatko = &current->left;   //Naopak ak je hladany kluc mensi, pojdeme
      current = current->left;  // do laveho podstromu
    }
  }
  //Ak sme tu tak ho nenasiel musi ho vytvorit cez otca aby sme zachovali 
  *tatko = malloc(sizeof(bst_node_t));  // postupnost v strome, opat mu 
  (*tatko)->key = key;        // mu priradime kluc, hodnotu, nastavime
  (*tatko)->content = value;  // ukazovatele na NULL a vratime true
  (*tatko)->left = NULL;
  (*tatko)->right = NULL;
  return true;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  bst_node_t **current = tree;     //Vytvorime si pomocny current (drzitel)
  while((*current)->right != NULL){// stromu a dokym existuje nieco napravo
    current = &(*current)->right;  // od currenta tak sa nanho presunieme
  }
  target->key = (*current)->key;   //Po narazeni na posledny prvok ulozime
  if (target->content.value != NULL) {   
    free(target->content.value);
  }
  target->content = (*current)->content;// jeho kluc a hodnotu a vymazeme
  bst_node_t *tmp = *current;      // ho zo stromu (korektne uvolnime)
  *current = (*current)->left;
  free(tmp);
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
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  if(*tree == NULL){    //Ak je strom prazdny, danu polozku nemozme
    return;             // odstranit lebo tam pochopitelne nie je
  }
  else{                     //Inak si ulozime opat ukazatel na tree a dokym
    bst_node_t **current = tree;// nenarazime na terminal tak posudzujeme
    while(*current != NULL){// hodnotu jeho klucu s nasim zadanym klucom
      if((*current)->key < key){     //Ak je hladany kluc vacsi ako aktualny 
        current = &(*current)->right;// root, prejdeme do praveho podstromu
      }
      else if((*current)->key > key){//Ak je vacsi prejdeme do laveho podstromu
        current = &(*current)->left;
      } 
      else{ //Ak sme nasli zhodny tak ideme zistit kolko potomkov ma
        bst_node_t *tmp = *current; //Ak nema ziadneho jedna sa o listovy uzol a ten
        if((*current)->left == NULL && (*current)->right == NULL){// mozme bez problemov
          if((*current)->content.value != NULL){
            free((*current)->content.value);
          } 
          free(*current);           // odstranit
          *current = NULL;
        }
        //Ak ma iba laveho potomka, presunieme current na jeho laveho potomka a zmazeme
        else if((*current)->right == NULL && (*current)->left != NULL){ // aktualny uzol
          if((*current)->content.value != NULL){
            free((*current)->content.value);
          }
          *current = (*current)->left;
          free(tmp);
        }//Ak ma iba praveho potomka, presunieme crrent na jeho praveho potomka a zmazeme
        else if((*current)->right != NULL && (*current)->left == NULL){ // aktualny uzol
          if((*current)->content.value != NULL){
            free((*current)->content.value);
          }
          *current = (*current)->right;
          free(tmp);
        }
        else{ //Ak ma oboch potomkov volame pomocnu funkciu bst_replace_by_rightmost
          bst_replace_by_rightmost(*current, &(*current)->left);
        }
        return;
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
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  if(*tree == NULL){  //Ak je strom prazdny, nie je co disposovat
    return;
  }
  stack_bst_t stack;     //Inak iinicializujeme zasobnik a vlozime
  stack_bst_init(&stack);// don koren
  stack_bst_push(&stack, *tree);  

  while(!stack_bst_empty(&stack)){  //Cyklime dokym nie je zasobnik 
    bst_node_t *current = stack_bst_pop(&stack);// prazdny, spravime
    if(current->right != NULL){     // stack pop a ulozime si hodnotu
      stack_bst_push(&stack, current->right);// z vrcholu do nasej 
    }                               // premennej a zsitujeme ci ma 
    if(current->left != NULL){      // pravy a lavy podstrom, ak hej
      stack_bst_push(&stack, current->left);// vlozime na zasobnik tieto
    }                               // podstromy a uvolnime aktualny
    if(current->content.value != NULL) {  
      free(current->content.value);
    }
    free(current);
  } 
  *tree = NULL;                     //Na konci nastavime strom tak ako
  return;                           // bol po inicializacii
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items)
{
  bst_node_t *current = tree;//Opat ulozime si aktualny uzol, dokym ma za sebou
  while(current != NULL){    // este iny uzol tak root vlozime do items, praveho
    bst_add_node_to_items(current, items); // syna dame na stack a aktualny 
    if(current->right != NULL){ // prepneme na jeho laveho syna
      stack_bst_push(to_visit, current->right);
    }
    current = current->left;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{
  if(tree == NULL){ //Ak je strom prazdny, nie je co triedit
    return;
  }
  stack_bst_t stack;//Inak inicializujem zasobnik, volam funkciu
  stack_bst_init(&stack);// bst_leftmost_preorder pre ziskanie 
  bst_leftmost_preorder(tree, &stack, items);// posledneho laveho
  bst_node_t *current; 

  while(!stack_bst_empty(&stack)){  //Dokym zasobnik nie je prazdny
    current = stack_bst_pop(&stack);// vyberam vsetkych pravych synov
    bst_leftmost_preorder(current, &stack, items);// a volam opat
  }                                 // funkciu na spracovanie mostleft 
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  bst_node_t *current = tree;//Docasny node, prechadzam dokym nie
  while(current != NULL){    // dalsi lavy syn neexsituje a vkladam
    stack_bst_push(to_visit, current);// si aktulne roots postupne
    current = current->left; // na stack
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{
  if(tree == NULL){ //Ak je storm prazdny, nemam co prechadzat
    return;
  }
  stack_bst_t stack;//Inicializujem zasobnik, a volam funkciu
  stack_bst_init(&stack);// pre najdenie najviac laveho uzlu
  bst_leftmost_inorder(tree, &stack);
  bst_node_t *current;

  while(!stack_bst_empty(&stack)){  //Dokym zasobnik nie je prazdny,
    current = stack_bst_pop(&stack);// vyberam vsetky povodne roots a
    bst_add_node_to_items(current, items);// pridavam ich do items, 
    bst_leftmost_inorder(current->right, &stack);// potom zavolam 
  }                                 // mostleft pre prave uzly
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit)
{//left, right, root
  bst_node_t *current = tree;
  while(current != NULL){ //Opat ziskam podobne najlavejsieho, po 
    stack_bst_push(to_visit, current); // ceste si ukladam nodes
    stack_bool_push(first_visit, true);// a taktiez poradie v akom
    current = current->left;           // som ich prechadzal coz
  }                       // je pre postorder podstatne
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{
  if(tree == NULL){ //Strom prazdny, nerobim nic
    return;
  }
  stack_bst_t stack;//Inak inicializujem zasobnik pre uzly
  stack_bool_t booleans; // ale aj zasobnik pre poradie, 
  stack_bst_init(&stack);// zavolam funkciu pre najdenie 
  stack_bool_init(&booleans);// najlavejsieho
  bst_leftmost_postorder(tree, &stack, &booleans);
  bst_node_t *current;
  bool sided;

  while(!stack_bst_empty(&stack)){  //Dokym zasobnik nie je prazdny, 
    current = stack_bst_top(&stack);//Nacitam posledny uzol a potom 
    sided = stack_bool_top(&booleans);// jeho hodnotu (boolean) a 
    stack_bst_pop(&stack);          // popnem oba zasobniky
    stack_bool_pop(&booleans);

    if(sided){                      //Ak mal praveho syna, vlozime root
      stack_bst_push(&stack, current);// na zasbonik ale jeho boolean 
      stack_bool_push(&booleans, false);// uz nastavime na false, lebo
      bst_leftmost_postorder(current->right, &stack, &booleans);// right
    }                               // syna uz spracovavame volanim leftmost
    else{                           //Ak praveho syna nema, root iba vlozime
      bst_add_node_to_items(current, items);// do items
    }
  }
}
