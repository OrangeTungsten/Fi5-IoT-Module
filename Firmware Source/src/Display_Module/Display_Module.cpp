#include <Arduino.h>
#include "Display_Module.h"
#include "Logo.h"
#include "Configuration.h"

// E-ink display libraray initialization
#ifdef DISP_E_INK

    #include <GxEPD2_3C.h> 
    #include "FreeMonoBold9pt7b.h"
    #include "FreeSans9pt7b.h"

    GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(
        GxEPD2_213_Z98c(DISP_CS_PIN, DISP_DC_PIN, DISP_RESET_PIN, DISP_BUSY_PIN)
    ); //128x256

#endif

//Display initialization
void InitializeDisplay() {
    #ifdef DISP_E_INK

        display.init();
        display.setRotation(3); //0,2 - Portrait; 1,3 Landscape
        display.setFullWindow();
        //display.fillScreen(0xFFFF);  //White Background

    #endif
}

//Fill display with specified color
void ClearDisplay(const char* color){
    #ifdef DISP_E_INK

        uint16_t FillColor = 0xFFFF; //Default white fill

        //Choosing color
        if(strcmp(color, "black") == 0) FillColor = 0x0000;       //Black
        else if(strcmp(color, "white") == 0) FillColor = 0xFFFF;  //White 
        else if(strcmp(color, "red") == 0) FillColor = 0xF800;    //Red 

        do {
            display.fillScreen(FillColor);  //White Background
        } while(display.nextPage());

    #endif
}

//This function updates display with given variables
void UpdateDisplay(String tP, String fP, String pT, String fU, String dT){

    #ifdef DISP_E_INK

        display.firstPage();
        do {

            //Clear
            display.fillScreen(0xFFFF);

            //Font
            display.setFont(&FreeMonoBold9pt7b);

            //Header
            display.setTextColor(0x0000); //Black
            display.setCursor(90, 12);
            display.print("Total");
            display.setCursor(1, 22);
            for(int i = 0; i<22; i++){
            display.print("-");  
            }
            
            //Values
            
            display.setTextColor(0x0000); //Black
            display.setCursor(5, 35);
            display.print("Prints     | ");
            display.setTextColor(0xF800); //Red
            display.print(tP);

            display.setTextColor(0x0000); //Black
            display.setCursor(5, 49);
            display.print("Successful | ");
            display.setTextColor(0xF800); //Red
            display.print(fP);

            display.setTextColor(0x0000); //Black
            display.setCursor(5, 63);
            display.print("Time       | ");
            display.setTextColor(0xF800); //Red
            display.print(pT);
            display.print("min");

            display.setTextColor(0x0000); //Black
            display.setCursor(5, 77);
            display.print("Filament   | ");
            display.setTextColor(0xF800); //Red
            display.print(fU);
            display.print("cm");

            //Logo
            display.drawXBitmap(196, 80, EINK_LOGO, 48, 48, 0x0000);

            //Date and time
            display.setCursor(5, 120);
            display.print(dT);

        } while (display.nextPage());

    #endif

}
