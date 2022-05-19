/*
 * d2s_UART_stop.h
 *
 *  Created on: Dec 6, 2016
 *      Author: lushuai
 */

#ifndef D2S_UART_STOP_H_
#define D2S_UART_STOP_H_
#include "./generic_d2s/d2s_Serial.h"

class d2s_UART_stop: public d2s_Serial {
	private:
		d2s_UART_stop(std::string pName):d2s_Serial(pName){};
		d2s_UART_stop(const d2s_UART_stop&);
		d2s_UART_stop operator= (const d2s_UART_stop&);

	public:
	    //singleton
	    static d2s_UART_stop& Instance(){
	        static d2s_UART_stop instance("uart_stop");
	        d2s_UART_stop& ref = instance;
	        return ref;
	    }

	    /*general parameters*/

	    int getAddressBits(){ return 0;}
	    int getDataBits(){ return 1;}

	    int getLowWaveformIndex(){ return 0;}
	    int getHighWaveformIndex(){ return 1;}
	    int getLowStrobeWaveformIndex(){ return 2;}
	    int getHighStrobeWaveformIndex(){ return 3;}
	    int getMaskStrobeWaveformIndex(){ return 4;}

	    std::string getInterfaceName(){ return "Uart";}

	    /*write parameters*/
	    std::string getWriteTemplatePatternName() { return "UART_Write_stop";}
	    std::string getWritePinNane() { return "UART0_RXD";}

	  //  int	getWriteAddressVectorNumberMSB() { return 0;}
	  //  int	getWriteAddressVectorNumberLSB() { return 8;}

	    int	getWriteDataVectorNumberMSB() { return 0;}
	    int	getWriteDataVectorNumberLSB() { return 0;}
};



#endif /* D2S_UART_STOP_H_ */
