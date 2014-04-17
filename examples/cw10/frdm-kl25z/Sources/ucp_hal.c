/** @file ucp_hal.c 
* @brief Hardware Abstraction Layer - MCU-specific code provided by the user.
*
* @note Toto jsou funkce, ktere vyuziva UCP aplikace. Tyto funkce
* musi implementovat sam uzivatel knihovny pro svuj konkretni hardware.
* napr. pokud meri teplotu teplomerem s analogovym vystupem, bude ve
* funkci ucphal_read_input() cist hodnotu z AD prevodniku.
*
*/

#include "ucp_hal.h"
#include "smt160_kl25.h"	/* Temperature sensor SMT160-30 */
//#include "ucp_swpwm.h"      /*  volitelne; softwarova PWM */
//#include "top_gb60.h"   /* ovladat modelu tepelne soustavy */

/** Inicializace potrebna pro funkce zde implementovane.
* 
* @return 0 pokud probehla uspesne, jinak chybovy kod.
* @note Funkce je implementovana uzivatelem nasi knihovny a zalezi na nem jak. Knihovna tuto funkci
* zavola jednou na zacatku programu. Mela by provest napr. nastaveni potrebnych pinu jako vystupy,
* inicializaci AD prevodniku (pokud je potreba), atd.
*/
uint8_t ucphal_init(void)
{
     /* TODO:  add your hardware initialization */
	smt160_init();
    
    // Priklad s vyuzitim softwarove PWM poskytovane v knihovne UCP 
    // ucp_pwm_setduty(1, 0);    /* Vystup na kanale 1 nastavit na 0 */
    
    // Priklad: model tepelne soustavy rizeny pomoci vlastniho ovladace 
    // InitTop(); 
           
    return 0;    
}

/** Nacteni vstupu pro dany kanal
* @param channel cislo kanalu, ktery se ma nacist. Toto je application-specific.
* @return namerena data
* @note Funkce je implementovana uzivatelem nasi knihovny a zalezi na nem jak. Knihovna ocekava, ze
*   vrati namerenou hodnotu (float) ve spravnych jednotkach. Tuto hodnotu pak pouziuje primo jako vstup Y
* do regulatoru. 
*/
float ucphal_read_input(uint8_t channel)
{
    /* TODO: add code to read input for given control-channel */
    
	// Cteni teploty primo merenim pomeru 0 a 1 na pinu?
	// na GB60 je teplota na pinu MCU header 24 a 26 (pro 2 kanaly casovace)
	
    
    /* Priklad pro cteni teploty pomoci ovladace pro model tepelne soustavy 
    float tmp;
    int rawtemp;
    static float old_val = 0.0f;
    
    // TODO: napr. pro model tep soustavy zde nacteni pres jeji ovladac
    if ( channel == 1 )
    {        
        // podporujeme jen 1 kanal; pokud je kanal jiny nez 1, vracime 0.
        // Ovladac vraci teplotu x 100, napr. 2500 pro 25 C
        rawtemp = GetTemp();
        if ( rawtemp <= 0 )      // pokud nejsou platna data, vraci GetTemp() nulu.
            return old_val;
        tmp = (float)rawtemp / 100;
        old_val = tmp;
        return tmp;
    }
    */
        
    return 0.0f;
}


/** "zapis" = provedeni akcniho zasahu pro dany kanal
* @param channel cislo kanalu
* @param value hodnota, ktera se ma zapsat. Toto je hodnota vypoctena regulatorem.
* @return none
* @note Funkce je implementovana uzivatelem nasi knihovny a zalezi na nem jak. Knihovna ji vola s
* vysledkem ziskanym z regulatoru. Hodnota !value" by mela byt uz primo hodnota, ktera se aplikuje, napr.
* 0 az 100 % v pripade realizace akcniho zasahu pomoci PWM, tak aby regulator pracoval s omezenim
* akcniho zasahu primo ve spravnych jednotkach.
*/
void ucphal_write_output(uint8_t channel, float value)
{
    /* TODO:  implement */
    
    /* Priklad pro pouziti naseho SW PWM 
    Predpokladame, ze regulator vraci akcni zasah jako hodnotu 0 az 100 */
    // ucp_pwm_setduty(channel, (uint8_t)value);  
   
}

/** Nacteni pozadovane hodnoty (W) pro dany kanal
* @param channel cislo kanalu, ktery se ma nacist. Toto je application-specific.
* @return pozadovanou hodnotu
* @note Funkce je implementovana uzivatelem nasi knihovny a zalezi na nem, jak. Knihovna ocekava, ze
*   vrati pozadovanou hodnotu (float) ve spravnych jednotkach. Tuto hodnotu pak pouzije primo 
* jako vstup W do regulatoru. 
* Hodnota muze byt napr. prectena z potenciometru, nebo spravovana uzivatelovou aplikaci podle stisku
* tlacitek napr. (+) a (-), apod.
*/
float ucphal_read_setpoint(uint8_t channel)
{
    /* TODO:  implement */
    /* Napriklad zde muze byt ziskani hodnoty podle pozice potenciometru na kitu... */
    return 0.0f;    
}

/* --- Nepovinne funkce ----- */

/** Zapis na vystupni diskretni pin, 0 nebo 1 (high / low)
* Je vyuzivana v softwarove PWM (ucp_swpwm.c).
* Nemusi byt implementovana pokud uzivatel nechce vyuzit softwarove PWM 
* @param  cislo kanalu, pro ktery se ma zapsat. Oznaceni (kodovani) pinu application-specific.
* @param value  hodnota, kterou zapsat na pin (log. 1 nebo log. 0)
* @note Funkce je implementovana uzivatelem nasi knihovny a zalezi na nem jak. Knihovna ocekava, ze
*   provede nastaveni pinu na 1 (=zapne akcni zasah) pro dany kanal. Prirazeni pinu regulacnim kanalum
* zalezi na uzivateli, ktery tuto funkci implementuje.
*/
void ucphal_write_pin(uint8_t channel, bool value)
{
    /* TODO:  implement */
    /* Priklad pro model tepelne sosustavy s vyuzitim ovladace, tj. nemusime 
    primo na piny, ale volame radeji funkce ovladace:
    if ( channel == 1 )
    {  
         if (value)
            TopOn();
         else
            TopOff();
    }
    */
    
    // ostatni kanaly ignorujeme
}
