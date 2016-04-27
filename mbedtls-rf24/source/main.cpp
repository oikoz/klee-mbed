/*
* RF24L01 Send Receive Hello Program
* Kisung Lee
* Last Updated 2016/04/12
*/

#include "mbed-drivers/mbed.h"
#include "mbedtls-rf24/nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P my_nrf24l01p(PTD6, PTD7, PTD5, PTD4, PTB20, PTC18);    // mosi, miso, sck, csn, ce, irq

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

int run-rf24() {

// The nRF24L01+ supports transfers from 1 to 32 bytes
#define TRANSFER_SIZE   1

    char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;

    my_nrf24l01p.powerUp();

    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );

    pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE );

    my_nrf24l01p.setTransferSize( TRANSFER_SIZE );

    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();

    while (1) {
        
        // If we've received anything over the host serial link...
        if ( pc.readable() ) {

            // ...add it to the transmit buffer
            txData[txDataCnt++] = pc.getc();

            // If the transmit buffer is full
            if ( txDataCnt >= sizeof( txData ) ) {
            
                // Send the transmitbuffer via the nRF24L01+
                my_nrf24l01p.write( NRF24L01P_PIPE_P0, txData, txDataCnt );
                 pc.printf( "%c", txData[txDataCnt-1] );
                txDataCnt = 0;
            }

            // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
            myled1 = !myled1;
        }

        // If we've received anything in the nRF24L01+...
        if ( my_nrf24l01p.readable() ) {
            
            // ...read the data into the receive buffer
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );

            // Display the receive buffer contents via the host serial link
            for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {

                pc.putc( rxData[i] );
            }

            // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
            myled2 = !myled2;
        }
    }
 }

void app_start(int, char*[]) {
    /* Use 115200 bps for consistency with other examples */
    get_stdio_serial().baud(115200);
    minar::Scheduler::postCallback(mbed::util::FunctionPointer0<void>(run).bind());
}
