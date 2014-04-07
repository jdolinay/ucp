/*      verze jazyka C ovladace pro model Tepelne soustavy

; Knihovna funkci byla vytvorena pro ovladani modelu tepelne soustavy.                     
; V teto knihovne jsou realizovany tyto zakladni funkce:                                   
;                                                                                          
;	-	InitTop - inicializace modelu pro vyhodnocovani signalu teplotniho cidla pomoci      
;               casovace TPM2.                                                             
; 	-	GetTemp - vypocet teploty povrchu rezistoru pomoci casovace TPM2                
; 	-	TopOn - zapnuti topeni + signalizace                                                 
; 	-	TopOff  - vypnuti topeni + signalizace   
; 
 - vyzaduje podporu "floating point" v projektu
- Vyuziva casovac TPM2
- Oproti ovladaci ASM jsou funkce pojmenovany bez podtrzitek (aby nedoslo k chybnemu pouziti H souboru s ASM kodem)                                            
; Revize: 20.11.2007 Jan Dolinay
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */



// Protoypy  - exportovane funkce
void TopOff(void);
void TopOn(void);
void InitTop(void);
int GetTemp(void);

// interni funkce
interrupt void TPM2C0_int(void);
interrupt void TPM2C2_int(void);
uint vzorec(void);

// globalni promenne
uint T1;
uint T2;
uint time1, time2, time3, time4;
uchar podminka;
uchar podminka1, podminka2, podminka_preruseni;     

// Presmerovani vektoru na nase obsluhy preruseni
void (*const obs_top1)(void) @0xFFEC = TPM2C0_int;
void (*const obs_top2)(void)@0xFFE8 = TPM2C2_int;


 //
 //INIT_TOP inicializuje matematickou knihovnu, nastavi A-D prevodnik pro nastaveni zadane teploty,
// nastavi casovac TPM2, kanaly CH0 a CH2 casovace TPM2 prepne do rezimu Input capture a nastavi
// promenne pro spravnou funkci cele knihovny 
void InitTop(void)
{
        PTFD_PTFD7 = 0;              // vypnute topeni
        PTFPE_PTFPE7 = 0;
        PTFDD_PTFDD7 = 1;       // PTF7 jako vystup
        
        //   MOV	    #$0f,PTFD	          ; inic. datoveho reg. portu F
        //   MOV 	  #0,PTFPE	          ; odpojime pull-up rezistory
        //   MOV  	  #$80,PTFDD    		  ; PTF7 vystupni rezim
         	
       //   MOV	    #0,PTDDD          	; port D vstupni rezim
       // MOV	    #$00,PTDPE	     
        PTDDD = 0;              // port D vystup
        PTDPE = 0;              // pull up disabled 
        
        TPM2SC = 0b00001010;          // inicializace TPM2, fsource=busclk, prescaler=8
        TPM2MOD = 0xFFFF;
        TPM2C0SC = 0b01001100;   // ; inicializace kanalu CH0 casovace TPM2, rezim Input capture, preruseni 
                                                                // od nabezne a sestupne hrany 
        TPM2C2SC = 0b01000100;   // inicializace kanalu CH2 casovace TPM2, rezim Input capture, preruseni 
                                                             // od nabezne hrany
        EnableInterrupts;
        T1 = T2 = 0;
        time1 = time2 = time3 = time4 = 0;   
        podminka = 0;
        podminka1 = podminka2 = 0;
        podminka_preruseni = 0;                                                                     
}


//GET_TEMP  slouzi pro vypocet skutecne teploty. Algoritmus pro vypocet skutecne teploty 
//je urcen pro rozsah teplot 0-145°C. Vzorec je ve tvaru 
//      t[°C]=212.76596*T2/T1-68.08512.
// Vysledek prevodu je predavan v registru H:X ve tvaru Ts*100 - Skutecna teplota 56.12°C
//- v registru H:X je predana hodnota 5612.
// Hodnotu T1 a T2 ziskavame pomoci kanalu 0 a 2 casovace TPM2, ktere pracujio v rezimu zachyceni stavu.
// Vraci 0 pokud jeste neni k dispozici vypoctena teplota (po startu programu)
int GetTemp(void)
{
        uint tep; 
topd1_opak:
        if ( (podminka & podminka2) == 0b00000001 )
        {
                 // vypocet   
                 podminka_preruseni = 1;
                 if ( time2 <= time1 )
                 {
                        // vypocek kdyz doslo k preteceni casovace   
                        T2 = time2 + (TPM2MOD - time1);                     // (TPM2MOD - time1) je kolik zbyvalo do preteceni                
                 }
                 else
                 {
                        // vypocet kdyz nedoslo k preteceni
                        T2 =  time2 - time1;
                 }    
                 
                 // Podobny vypocet pro t1
                  if ( time4 <= time3 )
                 {
                        // vypocek kdyz doslo k preteceni casovace   
                        T1 = time4 + (TPM2MOD - time3);                     // (TPM2MOD - time1) je kolik zbyvalo do preteceni                
                 }
                 else
                 {
                        // vypocet kdyz nedoslo k preteceni
                        T1 =  time4 - time3;
                 } 
                 
                   time1 = time2 = time3 = time4 = 0;                  
                   podminka_preruseni = 0;  
                    tep = vzorec();
                    //if ( tep < 14500 )
                    //    goto topd1_opak;
                    return tep;                          
        }
        else
        {
                // nejsou pripravena data, cekame?
              __RESET_WATCHDOG();  
             goto topd1_opak;     // lepe pockat jinka prakticky nikdy nebudou aktualni data
             // Updata
           //  if ( T1 != 0 )
           //             return vzorec();    // proste pouziju posledni platna data, pokud nejaka jsou... 
           //     else
           //             return 0;                
        }
}
    
// ===================          
// Zapnuti topeni
// ====================
void TopOn(void)
{
        PTFD_PTFD7 = 1;               
}

// ===================          
// Zapnuti topeni
// ====================
void TopOff(void)
{
        PTFD_PTFD7 = 0;               
}
          
	        
////////////////////////////////////////////////////// Interni funkce
//
// vypocet skutecne teploty
// vzorec je ve tvaru t=konst1*T2/T1-konst2 =212.76596 * T2/T1- 68.08512
//  uklada do skut_teplota
uint vzorec(void)
{
        uint p =   (int)((float)T2 / T1 * 21277) - 6809;        //   (int)(212.76596 * (float)T2 / T1 - 68.08512 * 100);
        // nakonec vynulovat T1 a T2... 
        // proc? nemohlo by zustat pro priste radeji nez by cekal na nove vysledky at vrati posledni platne...
        T1 = T2 = 0;
        
         return  (p);
}

// =================================================================
//V obsluze preruseni urcujeme promenne
//time1-urceni zacatku impulsu vystupniho signalu od teplomeru (start log 1)
// time2-urceni konce impulsu vystupniho signalu od teplomeru    (konec log 1)
// time3-urceni pocatku periody signalu od teplomeru
// time4-urceni konce periody signalu od teplomeru 
//Z techto promennych se v hlavnim programu urci promenne T1(delka periody signalu) a T2 (doba, po kterou 
//je signal v urovni log 1) a z tectodvou promennych se vypocita hodnota skutecne teploty  
//
// ================================
//preruseni od kanálu 0 casovace 2
interrupt void TPM2C0_int(void)
{
        TPM2C0SC &= 0x7F;  
        if ( podminka_preruseni == 1 )
                return;
        podminka = 0;
        if ( PTDD_PTDD3 == 0 )
        {         
                time2 = TPM2C0V;     
                podminka = 1;  
        }
        else        
       {
                time1 = TPM2C0V;
       }        
}


// preruseni od kanálu 2 casovace 2
interrupt void TPM2C2_int(void)
{
        TPM2C2SC &= 0x7F;    
        if ( podminka_preruseni == 1 )
                return;     
        podminka2 = 0;
        if ( podminka1 != 0 )   // urceni zda je to pocatek nebo konec impulzu
        {
                // skok
                if ( podminka1 != 1 )
                        return;         
                time4 = TPM2C2V;
                podminka1 = 0;
                podminka2 = 1;
        }
        else
        {
                  time3 = TPM2C2V;
                  podminka1 = 1;                
        }
          
}

