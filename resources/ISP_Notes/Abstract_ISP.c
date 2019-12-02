/*******************************
HIGH LEVEL CODE FOR ISP
*******************************/
/*
Software considererations
1. To enter and stay in Serial programming mode, the AVR microcontroller reset line has to be kept active (low)
	1.1 Refer to entering programming mode doc
2. To perform chip erase the reset has to be pulsed to end the chip erase cycle 
	For performing 1. and 2. the programmer take control of the reset through the use of a fourth line (RESET)
3. SCK: Programmer supplies clock on the SCK pin. (This pin is always driven by the programmer)
	3.1 Immediately after the Reset goes active, SCK is driven zero by the programmer
	3.2 Send second byte ($53)
	3.3 When issuing the third byte of the programming enable instruction (PEI) the target will echo back the second byte 
		3.3.1 If the target doesn't echo back the second byte, give reset a positive pulse and issue a new (PEI)  
	3.4	All four bytes of (PEI) must be sent before any new transmission 
		"Read Memory Programming page 289 Memory Programming in datasheet"
	3.5 Consideration: Minimum low and high periods for the serial clock input are defined in the programming section
4. MOSI: The ISP supplies data to the target on the MOSI pin
5. MISO: The target responds on the MISO pin.
	5.1 When reset is applied to the target, the MISO is set up as an input to the programmer with no pull up
	5.2 Only after the "Programming Enable" is complete will the target set the MISO pin to become an output  
	5.3 After the target sets it as output will the programmer apply its pullup to keep the MISO line stable until its driven by the target
6. Command Format: Commands have a common 4 bytes format 
	6.1 First Byte: contains command code, selecting operation amd target memory 
	6.2 Second and third byte contain the address of the the selected memory address 
	6.3 Fourth byte: contains data going in either direction
		"The data sent by the target s usually the data sent in the previous byte"
	6.4 When the Reset pin is first pulled active, the only instruction accepted by the SPI interface is “Programming Enable”.
		6.4.1 The programming enable opens access to the FLASH and EEPROM memories  
		6.4.2 Access is given based on the settings of the Lock bits 
		6.4.3 The target does not reply with an acknowledge to the "Programming Enable", to check the if its been accepted, the device code can
				be read
	6.5 Device Code: It identifies the chip vendor, part family, flash size in Kb, family member 
		"Read Device code comment in In-system-programming document for details page-8"
7. Flash Program Memory Access: Depending upon target device the either a page write or a byte write is performed 
	7.1 Perform a chip erase before programming the Flash memory 
	
*/