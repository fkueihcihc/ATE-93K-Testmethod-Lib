/*
 * d2sUART.h
 *
 *  Created on: Dec 6, 2016
 *      Author: lushuai
 */

#ifndef D2SUART_H_
#define D2SUART_H_
#include "./generic_d2s/d2s_Serial.h"

class d2s_UART: public d2s_Serial{
private:
	d2s_UART(std::string pName):d2s_Serial(pName){};
	d2s_UART(const d2s_UART&);
	d2s_UART operator= (const d2s_UART&);

public:
    //singleton
    static d2s_UART& Instance(){
        static d2s_UART instance("pUART");
        d2s_UART& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 1;}
    int getDataBits(){ return 8;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}

    std::string getInterfaceName(){ return "UART";}
    //std::string getPatternStoragePath() { return "d2s_pattern"; }  // not supported in 7.2.3; supported in 7.4

    /*write parameters*/
    std::string getWriteTemplatePatternName() { return "UART_Write_template_pUART";}
    std::string getWritePinName() { return "UART0_RXD";}

    int	getWriteAddressVectorNumberMSB() { return 0;}
    int	getWriteAddressVectorNumberLSB() { return 0;}

    int	getWriteDataVectorNumberMSB() { return 8;}
    int	getWriteDataVectorNumberLSB() { return 1;}


    /*read parameters*/
    std::string getReadTemplatePatternName() { return "UART_Read_template_pUART";}
    std::string getReadAddressPinName() {return "UART0_RXD";}
    std::string getReadPinName() { return "UART0_TXD";}

    int	getReadAddressVectorNumberMSB() { return 0;}
    int	getReadAddressVectorNumberLSB() { return 0;}

    int getReadDataCycleNumberMSB() {return 1;}
    int getReadDataCycleNumberLSB() {return 8;}

    int	getReadDataVectorNumberMSB() { return 1;}
    int	getReadDataVectorNumberLSB() { return 8;}

    /*wait parameters*/
    std::string getTimingPort() {return "pUART";}
    std::string getWaitTemplatePatternName() {return "UART_Wait_template_pUART";}
};
#endif /* D2SUART_H_ */
