/*
 * AT_Commands.h
 *
 *  Created on: 28-Nov-2019
 *      Author: Hp
 */

#ifndef AT_COMMANDS_H_
#define AT_COMMANDS_H_
#endif /* AT_COMMANDS_H_ */

// ALWAYS PUT \r\n AFTER ALL COMMANDS !!!

/***********************************
SANITY CHECK
Receive OK
************************************/
//char sanity_check[10] = {'A','T','\r','\n','\0'};
char sanity_check[] = "AT\r\n";

/***********************************
set HC-05 in master mode
Receive OK
************************************/
//char set_master[12] = {'A','T','+','R','O','L','E','=','1','\r','\n','\0'};
char set_master[] = "AT+ROLE=1\r\n";

/***********************************
set HC-05 in slave mode
Receive OK
************************************/
//char set_master[12] = {'A','T','+','R','O','L','E','=','1','\r','\n','\0'};
char set_slave[] = "AT+ROLE=0\r\n";

/***********************************
Inquire HC-05 role
1- Master
0- Slave
Receive
+ROLE:<Param>
OK
************************************/
//char set_master[12] = {'A','T','+','R','O','L','E','=','1','\r','\n','\0'};
char IQ_role[] = "AT+ROLE?\r\n";

/***********************************
set connection mode
0-connect the module to specified address
Bluetooth address specified by binding command
Receive OK
************************************/
char set_cmode[] = "AT+CMODE=0\r\n";


/***********************************
Inquire connection mode
Receive UART= +CMODE:<PARAM>
OK
************************************/
char IQ_cmode[] = "AT+CMODE?\r\n";

/***********************************
set connection mode
0-connect the module to specified address
Bluetooth address specified by binding command
Receive OK
************************************/
// EDIT ADDRESS BEFORE USING
char set_binding[] = "AT+BIND=14,3,62012\r\n";

/***********************************
Inquire Binding address
Receive +BIND<param>
OK
************************************/
char IQ_binding[] = "AT+BIND?\r\n";

/***********************************
Inquire Bluetooth address
Receive +ADDR:1234:56:abcdef
OK
************************************/
char IQ_address[] = "AT+ADDR?\r\n";

/***********************************
Set UART parameters
baud rate, stop bit, parity bit
38400,0-1 stop bit,0-no parity
page 9, hc-05 at commands
Receive OK
************************************/
char Set_uart[] = "AT+UART=38400,0,0\r\n";

/***********************************
Inquire serial parameter
Receive UART= baudrate,stop bit,parity bit
OK
************************************/
char IQ_uart[] = "AT+UART?\r\n";

/***********************************
Inquire working state
Receive UART= +STATE: <PARAM>
"INITIALIZED"
"READY"
"PAIRABLE"
"PAIRED"
"INQUIRING"
"CONNECTING"
"CONNECTED"
"DISCONNECTED"
OK
************************************/
char IQ_state[] = "AT+STATE?\r\n";

/***********************************
PAIR DEVICE
Receive
OK - SUCCESS
FAIL-FAILURE
************************************/
char set_pair[] = "AT+PAIR=ADDRESS,TIME in SECONDS\r\n";

/***********************************
Set module back to original state
Receive
OK - SUCCESS
FAIL-FAILURE
************************************/
char set_reset[] = "AT+ORGL\r\n";

















