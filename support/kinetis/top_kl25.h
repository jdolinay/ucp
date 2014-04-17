// --------------------------------------------------------------------------------
// Hlavickovy soubor pro ovladac tepelne soustavy  v jazyku C:  top_kl25.c a .h
// Pro Freedom board FRDM-KL25Z (Kinetis MCU)
// --------------------------------------------------------------------------------

#ifndef  TOP_KL25Z
    #define TOP_KL25Z

void TopOff(void);
void TopOn(void);
void InitTop(void);
short GetTemp(void);

#endif  /* TOP_KL25Z */