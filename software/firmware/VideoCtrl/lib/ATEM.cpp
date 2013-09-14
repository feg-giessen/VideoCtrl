/*
Copyright 2012 Kasper Sk�rh�j, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the ATEM library for Arduino

The ATEM library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ATEM library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ATEM library. If not, see http://www.gnu.org/licenses/.

*/

#include "ATEM.h"
#include "arduino_bridge.h"
#include <string.h>
#include <stdio.h>
#include "lwip/netbuf.h"
#include "net/UdpClient.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
//#include <MemoryFree.h>

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEM::ATEM(){}

/**
 * Setting up IP address for the switcher (and local port to send packets from)
 */
void ATEM::begin(const ip_addr_t ip){
		// Set up Udp communication object:
	UdpClient Udp;
	_Udp = Udp;

	_switcherIP = ip;	// Set switcher IP address

	_isConnectingTime = 0;

	_ATEM_AMLv_channel=0;
}

/**
 * Initiating connection handshake to the ATEM switcher
 */
void ATEM::connect() {
	_isConnectingTime = millis();
	_localPacketIdCounter = 1;	// Init localPacketIDCounter to 1;
	_hasInitialized = false;
	_lastContact = 0;
	_Udp.begin(&_switcherIP, 9910);

		// Setting this, because even though we haven't had contact, it constitutes an attempt that should be responded to at least:
	_lastContact = millis();

	// Send connectString to ATEM:
	// TODO: Describe packet contents according to rev.eng. API
	ATEM_DEBUG("Sending connect packet to ATEM switcher.\n");

	u8_t connectHello[] = {
		0x10, 0x14, 0x53, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	_Udp.send(connectHello, 20);
}

/**
 * Keeps connection to the switcher alive - basically, this means answering back to ping packages.
 * Therefore: Call this in the Arduino loop() function and make sure it gets call at least 2 times a second
 * Other recommendations might come up in the future.
 */
void ATEM::runLoop() {

  // WARNING:
  // It can cause severe timing problems using "slow" functions such as Serial.print*()
  // in the runloop, in particular during "boot" where the ATEM delivers some 10-20 kbytes of system status info which
  // must exit the RX-buffer quite fast. Therefore, using Serial.print for debugging in this
  // critical phase will in it self affect program execution!

  // Limit of the RX buffer of the Ethernet interface is another general issue.
  // When ATEM sends the initial system status packets (10-20 kbytes), they are sent with a few microseconds in between
  // The RX buffer of the Ethernet interface on Arduino simply does not have the kapacity to take more than 2k at a time.
  // This means, that we only receive the first packet, the others seems to be discarded. Luckily most information we like to
  // know about is in the first packet (and some in the second, the rest is probably thumbnails for the media player).
  // It may be possible to bump up this buffer to 4 or 8 k by simply re-configuring the amount of allowed sockets on the interface.
  // For some more information from a guy seemingly having a similar issue, look here:
  // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1282170842

	uint16_t packetSize = 0;

	if (_isConnectingTime > 0)	{

			// Waiting for the ATEM to answer back with a packet 20 bytes long.
			// According to packet analysis with WireShark, this feedback from ATEM
			// comes within a few microseconds!
		packetSize = _Udp.read(_packetBuffer);
		if (packetSize==20)   {

				// Read the response packet. We will only subtract the session ID
				// According to packet analysis with WireShark, this feedback from ATEM
				// comes a few microseconds after our connect invitation above. Two packets immediately follow each other.
				// After approx. 200 milliseconds a third packet is sent from ATEM - a sort of re-sent because it gets impatient.
				// And it seems that THIS third packet is the one we actually read and respond to. In other words, I believe that
				// the ethernet interface on Arduino actually misses the first two for some reason!
			_sessionID = _packetBuffer[15];

			// Send connectAnswerString to ATEM:
			// TODO: Describe packet contents according to rev.eng. API
			u8_t connectHelloAnswerString[] = {
			  0x80, 0x0c, 0x53, 0xab, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00 };
			_Udp.send(connectHelloAnswerString, 12);

			_isConnectingTime = 0;	// End connecting
		} else {
			if (_isConnectingTime+2000 < (unsigned long)millis())	{
				ATEM_DEBUG("Timeout waiting for ATEM switcher response\n");
				_isConnectingTime = 0;
			}
		}
	} else {




	  // If there's data available, read a packet, empty up:
	 // Serial.println("ATEM runLoop():");
	  while(true) {	// Iterate until buffer is empty:
		  packetSize = _Udp.read(_packetBuffer);
		  if (packetSize != 0)   {

		    // Read out packet length (first word), remote packet ID number and "command":
		    uint16_t packetLength = ((_packetBuffer[0] & 0b00000111) << 8) | _packetBuffer[1];
		    _lastRemotePacketID = (_packetBuffer[10] << 8) | _packetBuffer[11];
		    uint8_t command = _packetBuffer[0] & 0b11111000;
		    bool command_ACK  = command & 0b00001000 ? true : false;	// If true, ATEM expects an acknowledgement answer back!
		    bool command_INIT = command & 0b00010000 ? true : false;	// If true, ATEM expects an acknowledgement answer back!
				// The five bits in "command" (from LSB to MSB):
				// 1 = ACK, "Please respond to this packet" (using the _lastRemotePacketID). Exception: The initial 10-20 kbytes of Switcher status
				// 2 = ?. Set during initialization? (first hand-shake packets contains that)
				// 3 = "This is a retransmission". You will see this bit set if the ATEM switcher did not get a timely response to a packet.
				// 4 = ? ("hello packet" according to "ratte", forum at atemuser.com)
				// 5 = "This is a response on your request". So set this when answering...


		    if (packetSize==packetLength) {  // Just to make sure these are equal, they should be!
			  _lastContact = millis();

		      // If a packet is 12 bytes long it indicates that all the initial information
		      // has been delivered from the ATEM and we can begin to answer back on every request
			  // Currently we don't know any other way to decide if an answer should be sent back...
		      if(!_hasInitialized && packetSize == 12) {
		        _hasInitialized = true;
		        ATEM_DEBUG("_hasInitialized=TRUE\n");
		      }

				if (packetLength > 12 && !command_INIT)	{	// !command_INIT is because there seems to be no commands in these packets and that will generate an error.
					_parsePacket(packetLength);
				}

		      // If we are initialized, lets answer back no matter what:
				// TODO: "_hasInitialized && " should be inserted back before "command_ACK" but
				// with Arduino 1.0 UDP library it has proven MORE likely that the initial
				// connection is made if we ALWAYS answer the switcher back.
				// Apparently the initial "chaos" of keeping up with the incoming data confuses
				// the UDP library so that we might never get initialized - and thus never get connected
				// So... for now this is how we do it:
				// CHANGED with arduino 1.0.1..... put back in.
		      if (_hasInitialized && command_ACK) {
		        ATEM_DEBUG("ACK, rpID: %d\n", _lastRemotePacketID);

		        _sendAnswerPacket(_lastRemotePacketID);
		      }

		    } else {
				// Flushing the buffer:
		    }
		  } else {
			break;	// Exit while(true) loop because there is no more packets in buffer.
		}
	  }
	}
}

bool ATEM::isConnectionTimedOut()	{
	unsigned long currentTime = millis();
	if (_lastContact>0 && _lastContact+10000 < currentTime)	{	// Timeout of 10 sec.
		_lastContact = 0;
		return true;
	}
	return false;
}

void ATEM::delay(const unsigned int delayTimeMillis)	{	// Responsible delay function which keeps the ATEM run loop up! DO NOT USE INSIDE THIS CLASS! Recursion could happen...
	unsigned long timeout = millis();
	timeout+=delayTimeMillis;

	while(timeout > millis())	{
		runLoop();
	}
}

/**
 * If a package longer than a normal acknowledgement is received from the ATEM Switcher we must read through the contents.
 * Usually such a package contains updated state information about the mixer
 * Selected information is extracted in this function and transferred to internal variables in this library.
 */
void ATEM::_parsePacket(uint16_t packetLength)	{
	 uint8_t idx;	// General reusable index usable for keyers, mediaplayer etc below.

 		// If packet is more than an ACK packet (= if its longer than 12 bytes header), lets parse it:
      uint16_t indexPointer = 12;	// 12 bytes has already been read from the packet...
      u8_t* ptr = &_packetBuffer[12];
      while (indexPointer < packetLength)  {

        // Read the length of segment (first word):
        _cmdLength = (ptr[0] << 8) | ptr[1];
		_cmdPointer = 0;

			// Get the "command string", basically this is the 4 char variable name in the ATEM memory holding the various state values of the system:
        char cmdStr[] = {
        		ptr[4], ptr[5], ptr[6], ptr[7], '\0'};

			// If length of segment larger than 8 (should always be...!)
        if (_cmdLength>8)  {
		  ptr += 8;

          // Extract the specific state information we like to know about:
          if(strcmp(cmdStr, "PrgI") == 0) {  // Program Bus status
			_ATEM_PrgI = ptr[1];
			ATEM_DEBUG("Program Bus: %d\n", _ATEM_PrgI);
          } else
          if(strcmp(cmdStr, "PrvI") == 0) {  // Preview Bus status
			_ATEM_PrvI = ptr[1];
			ATEM_DEBUG("Preview Bus: %d\n", _ATEM_PrvI);
          } else
          if(strcmp(cmdStr, "TlIn") == 0) {  // Tally status for inputs 1-8
            uint8_t count = ptr[1]; // Number of inputs
              // Currently is only 8 inputs supported so make sure to read max 8.
            if(count > 8) {
              count = 8;
            }
            	// Inputs 1-8, bit 0 = Prg tally, bit 1 = Prv tally. Both can be set simultaneously.
            for(uint8_t i = 0; i < count; ++i) {
              _ATEM_TlIn[i] = ptr[2+i];
            }

            ATEM_DEBUG("Tally updated: ");
          } else
          if(strcmp(cmdStr, "Time") == 0) {  // Time. What is this anyway?
        	  ATEM_DEBUG("Time %d:%d:%d:%d\n", ptr[0], ptr[1], ptr[2], ptr[3]);
	      } else
	      if(strcmp(cmdStr, "TrPr") == 0) {  // Transition Preview
			_ATEM_TrPr = ptr[1] > 0 ? true : false;
			ATEM_DEBUG("Transition Preview: %x\n", _ATEM_TrPr);
          } else
	      if(strcmp(cmdStr, "TrPs") == 0) {  // Transition Position
			_ATEM_TrPs_frameCount = ptr[2];	// Frames count down
			_ATEM_TrPs_position = ptr[4]*256 + ptr[5];	// Position 0-1000 - maybe more in later firmwares?
          } else
	      if(strcmp(cmdStr, "TrSS") == 0) {  // Transition Style and Keyer on next transition
			_ATEM_TrSS_KeyersOnNextTransition = ptr[2] & 0b11111;	// Bit 0: Background; Bit 1-4: Key 1-4
			ATEM_DEBUG("Keyers on Next Transition: %x\n", _ATEM_TrSS_KeyersOnNextTransition);

			_ATEM_TrSS_TransitionStyle = ptr[1];
			ATEM_DEBUG("Transition Style: %d\n", _ATEM_TrSS_TransitionStyle);	// 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
          } else
	      if(strcmp(cmdStr, "FtbS") == 0) {  // Fade To Black State
			_ATEM_FtbS_state = ptr[2]; // State of Fade To Black, 0 = off and 1 = activated
			_ATEM_FtbS_frameCount = ptr[3];	// Frames count down
			ATEM_DEBUG("FTB: %d/%d\n", _ATEM_FtbS_state, _ATEM_FtbS_frameCount);
          } else
	      if(strcmp(cmdStr, "FtbP") == 0) {  // Fade To Black - Positions(?) (Transition Time in frames for FTB): 0x01-0xFA
			_ATEM_FtbP_time = ptr[1];
          } else
	      if(strcmp(cmdStr, "TMxP") == 0) {  // Mix Transition Position(?) (Transition Time in frames for Mix transitions.): 0x01-0xFA
			_ATEM_TMxP_time = ptr[1];
          } else
	      if(strcmp(cmdStr, "DskS") == 0) {  // Downstream Keyer state. Also contains information about the frame count in case of "Auto"
			idx = ptr[0];
			if (idx <=1)	{
				_ATEM_DskOn[idx] = ptr[1] > 0 ? true : false;
				ATEM_DEBUG("Dsk Keyer %d:%d", idx+1, _ATEM_DskOn[idx]);
			}
          } else
	      if(strcmp(cmdStr, "DskP") == 0) {  // Downstream Keyer Tie
			idx = ptr[0];
			if (idx <=1)	{
				_ATEM_DskTie[idx] = ptr[1] > 0 ? true : false;
				ATEM_DEBUG("Dsk Keyer %d Tie: %d\n", idx+1, _ATEM_DskTie[idx]);
			}
          } else
		  if(strcmp(cmdStr, "KeOn") == 0) {  // Upstream Keyer on
			idx = ptr[1];
			if (idx <=3)	{
				_ATEM_KeOn[idx] = ptr[2] > 0 ? true : false;
				ATEM_DEBUG("Upstream Keyer %d: %d\n", idx+1, _ATEM_KeOn[idx]);
			}
	      } else
		  if(strcmp(cmdStr, "ColV") == 0) {  // Color Generator Change
				// Todo: Relatively easy: 8 bytes, first is the color generator, the last 6 is hsl words
		  } else
		  if(strcmp(cmdStr, "MPCE") == 0) {  // Media Player Clip Enable
				idx = ptr[0];
				if (idx <=1)	{
					_ATEM_MPType[idx] = ptr[1];
					_ATEM_MPStill[idx] = ptr[2];
					_ATEM_MPClip[idx] = ptr[3];
				}
		  } else
		  if(strcmp(cmdStr, "AuxS") == 0) {  // Aux Output Source
				uint8_t auxInput = ptr[0];
				if (auxInput <= 2)	{
					_ATEM_AuxS[auxInput] = ptr[1];
					ATEM_DEBUG("Aux %d Output: %d\n", auxInput+1, _ATEM_AuxS[auxInput]);
				}

		    } else
		    if(strcmp(cmdStr, "_ver") == 0) {  // Firmware version
				_ATEM_ver_m = ptr[1];	// Firmware version, "left of decimal point" (what is that called anyway?)
				_ATEM_ver_l = ptr[3];	// Firmware version, decimals ("right of decimal point")
		    } else
			if(strcmp(cmdStr, "_pin") == 0) {  // Name
				for(uint8_t i=0;i<16;i++)	{
					_ATEM_pin[i] = ptr[i];
				}
				_ATEM_pin[16] = 0;	// Termination
		    } else
			if(strcmp(cmdStr, "AMTl") == 0) {  // Audio Monitor Tally (on/off settings)
				// Same system as for video: "TlIn"... just implement when time.
		    } else
			// Note for future reveng: For master control, volume at least comes back in "AMMO" (CAMM is the command code.)
			if(strcmp(cmdStr, "AMIP") == 0) {  // Audio Monitor Input P... (state) (On, Off, AFV)
				if (ptr[0]<13)	{
					_ATEM_AudioChannelMode[ptr[0]]  = ptr[6];
					// 0 = Channel
					// 6 = On/Off/AFV
					// 10+11 = Balance (0xD8F0 - 0x0000 - 0x2710)
					// 8+9 = Volume (0x0020 - 0xFF65)
				}
	/*			for(uint8_t a=0;a<_cmdLength-8;a++)	{
	            	Serial.print((uint8_t)ptr[a], HEX);
	            	Serial.print(" ");
				}
				Serial.println("");
	*/
/*				1M/E:
					0: MASTER
					1: (Monitor?)
					2-9: HDMI1 - SDI8
					10: MP1
					11: MP2
					12: EXT

				TVS:
					0: MASTER
					1: (Monitor?)
					2-7: INPUT1-6 (HDMI - HDMI - HDMI/SDI - HDMI/SDI - SDI - SDI)
					8: EXT
				*/
		/*		Serial.print("Audio Channel: ");
				Serial.println(ptr[0]);	// ptr[2] seems to be input number (one higher...)
				Serial.print(" - State: ");
				Serial.println(ptr[3] == 0x01 ? "ON" : (ptr[3] == 0x02 ? "AFV" : (ptr[3] > 0 ? "???" : "OFF")));
				Serial.print(" - Volume: ");
				Serial.print((uint16_t)ptr[4]*256+ptr[5]);
				Serial.print("/");
				Serial.println((uint16_t)ptr[6]*256+ptr[7]);
		   */ } else
		/*	if(strcmp(cmdStr, "AMLv") == 0) {  // Audio Monitor Levels
				uint16_t readingOffset = 4+(_ATEM_AMLv_channel << 4);	// *16
				while (readingOffset+4 > 96)	{
					readingOffset-=96;
					_readToPacketBuffer();
				}
				_ATEM_AMLv[0] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.

				readingOffset+=4;

				while (readingOffset+4 > 96)	{
					readingOffset-=96;
					_readToPacketBuffer();
				}
				_ATEM_AMLv[1] = ((uint16_t)(_packetBuffer[readingOffset+1]<<8) | _packetBuffer[readingOffset+2]);	//drops the 8 least sign. bits! -> 15 bit resolution for VU purposes. fine enough.
			} else */
			if(strcmp(cmdStr, "VidM") == 0) {  // Video format (SD, HD, framerate etc.)
				_ATEM_VidM = _packetBuffer[0];
		    } else {



			// SHOULD ONLY THE UNCOMMENTED for development and with a high Baud rate on serial - 115200 for instance. Otherwise it will not connect due to serial writing speeds.
			/*
	            if (_serialOutput) {
					Serial.print(("???? Unknown token: "));
					Serial.print(cmdStr);
					Serial.print(" : ");
				}
				for(uint8_t a=(-2+8);a<_cmdLength-2;a++)	{
	            	if (_serialOutput) Serial.print((uint8_t)_packetBuffer[a], HEX);
	            	if (_serialOutput) Serial.print(" ");
				}
				if (_serialOutput) Serial.println("");
	        */
			}

          indexPointer+=_cmdLength;
        } else {
      		indexPointer = 2000;
        }
      }
}

/**
 * Sending a regular answer packet back (tell the switcher that "we heard you, thanks.")
 */
void ATEM::_sendAnswerPacket(uint16_t remotePacketID)  {

  //Answer packet:
  memset(_packetBuffer, 0, 12);			// Using 12 bytes of answer buffer, setting to zeros.
  _packetBuffer[2] = 0x80;  // ??? API
  _packetBuffer[3] = _sessionID;  // Session ID
  _packetBuffer[4] = remotePacketID/256;  // Remote Packet ID, MSB
  _packetBuffer[5] = remotePacketID%256;  // Remote Packet ID, LSB
  _packetBuffer[9] = 0x41;  // ??? API
  // The rest is zeros.

  // Create header:
  uint16_t returnPacketLength = 10+2;
  _packetBuffer[0] = returnPacketLength/256;
  _packetBuffer[1] = returnPacketLength%256;
  _packetBuffer[0] |= 0b10000000;

  // Send connectAnswerString to ATEM:
  _Udp.send(_packetBuffer, returnPacketLength);
}

/**
 * Sending a command packet back (ask the switcher to do something)
 */
void ATEM::_sendCommandPacket(const char cmd[4], uint8_t commandBytes[64], uint8_t cmdBytes)  {	// TEMP: 16->64

  if (cmdBytes <= 64)	{	// Currently, only a lenght up to 16 - can be extended, but then the _packetBuffer buffer must be prolonged as well (to more than 36)	<- TEMP 16->64
	  //Answer packet preparations:
	  memset(_packetBuffer, 0, 84);	// <- TEMP 36->84
	  _packetBuffer[2] = 0x80;  // ??? API
	  _packetBuffer[3] = _sessionID;  // Session ID
	  _packetBuffer[10] = _localPacketIdCounter/256;  // Remote Packet ID, MSB
	  _packetBuffer[11] = _localPacketIdCounter%256;  // Remote Packet ID, LSB

	  // The rest is zeros.

	  // Command identifier (4 bytes, after header (12 bytes) and local segment length (4 bytes)):
	  int i;
	  for (i=0; i<4; i++)  {
	    _packetBuffer[12+4+i] = cmd[i];
	  }

  		// Command value (after command):
	  for (i=0; i<cmdBytes; i++)  {
	    _packetBuffer[12+4+4+i] = commandBytes[i];
	  }

	  // Command length:
	  _packetBuffer[12] = (4+4+cmdBytes)/256;
	  _packetBuffer[12+1] = (4+4+cmdBytes)%256;

	  // Create header:
	  uint16_t returnPacketLength = 10+2+(4+4+cmdBytes);
	  _packetBuffer[0] = returnPacketLength/256;
	  _packetBuffer[1] = returnPacketLength%256;
	  _packetBuffer[0] |= 0b00001000;

	  // Send connectAnswerString to ATEM:
	  _Udp.send(_packetBuffer, returnPacketLength);

	  _localPacketIdCounter++;
	}
}

/**
 * Sets all zeros in packet buffer:
 */
void ATEM::_wipeCleanPacketBuffer() {
	memset(_packetBuffer, 0, 96);
}

/**
 * Sets all zeros in packet buffer:
 */
void ATEM::_sendPacketBufferCmdData(const char cmd[4], uint8_t cmdBytes)  {

  if (cmdBytes <= 96-20)	{
	  //Answer packet preparations:
	  uint8_t _headerBuffer[20];
	  memset(_headerBuffer, 0, 20);
	  _headerBuffer[2] = 0x80;  // ??? API
	  _headerBuffer[3] = _sessionID;  // Session ID
	  _headerBuffer[10] = _localPacketIdCounter/256;  // Remote Packet ID, MSB
	  _headerBuffer[11] = _localPacketIdCounter%256;  // Remote Packet ID, LSB

	  // The rest is zeros.

	  // Command identifier (4 bytes, after header (12 bytes) and local segment length (4 bytes)):
	  int i;
	  for (i=0; i<4; i++)  {
	    _headerBuffer[12+4+i] = cmd[i];
	  }

	  // Command length:
	  _headerBuffer[12] = (4+4+cmdBytes)/256;
	  _headerBuffer[12+1] = (4+4+cmdBytes)%256;

	  // Create header:
	  uint16_t returnPacketLength = 20+cmdBytes;
	  _headerBuffer[0] = returnPacketLength/256;
	  _headerBuffer[1] = returnPacketLength%256;
	  _headerBuffer[0] |= 0b00001000;

	  // Send connectAnswerString to ATEM:
	  _Udp.write(_headerBuffer, 20);
	  _Udp.send( _packetBuffer, cmdBytes);

	  _localPacketIdCounter++;
	}
}





/********************************
 *
 * General Getter/Setter methods
 *
 ********************************/

/**
 * Getter method: If true, the initial handshake and "stressful" information exchange has occured and now the switcher connection should be ready for operation.
 */
bool ATEM::hasInitialized()	{
	return _hasInitialized;
}

/**
 * Returns last Remote Packet ID
 */
uint16_t ATEM::getATEM_lastRemotePacketId()	{
	return _lastRemotePacketID;
}

uint8_t ATEM::getATEMmodel()	{
/*	Serial.println(_ATEM_pin);
	Serial.println(strcmp(_ATEM_pin, "ATEM Television ") == 0);
	Serial.println(strcmp(_ATEM_pin, "ATEM 1 M/E Produ") == 0);
	Serial.println(strcmp(_ATEM_pin, "ATEM 2 M/E Produ") == 0);	// Didn't test this yet!
*/
	if (_ATEM_pin[5]=='T')	{
		ATEM_DEBUG("ATEM TeleVision Studio Detected");
		return 0;
	}
	if (_ATEM_pin[5]=='1')	{
		ATEM_DEBUG("ATEM 1 M/E Detected");
		return 1;
	}
	if (_ATEM_pin[5]=='2')	{
		ATEM_DEBUG("ATEM 2 M/E Detected");
		return 2;
	}
	return 255;
}







/********************************
 *
 * ATEM Switcher state methods
 * Returns the most recent information we've
 * got about the switchers state
 *
 ********************************/

uint8_t ATEM::getProgramInput() {
	return _ATEM_PrgI;
}
uint8_t ATEM::getPreviewInput() {
	return _ATEM_PrvI;
}
bool ATEM::getProgramTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! <8 at the moment.
	return (_ATEM_TlIn[inputNumber-1] & 1) >0 ? true : false;
}
bool ATEM::getPreviewTally(uint8_t inputNumber) {
  	// TODO: Validate that input number exists on current model! 1-8 at the moment.
	return (_ATEM_TlIn[inputNumber-1] & 2) >0 ? true : false;
}
bool ATEM::getUpstreamKeyerStatus(uint8_t inputNumber) {
	if (inputNumber>=1 && inputNumber<=4)	{
		return _ATEM_KeOn[inputNumber-1];
	}
	return false;
}
bool ATEM::getUpstreamKeyerOnNextTransitionStatus(uint8_t inputNumber) {
	if (inputNumber>=1 && inputNumber<=4)	{
			// Notice: the first bit is set for the "background", not valid.
		return (_ATEM_TrSS_KeyersOnNextTransition & (0x01 << inputNumber)) ? true : false;
	}
	return false;
}

bool ATEM::getDownstreamKeyerStatus(uint8_t inputNumber) {
	if (inputNumber>=1 && inputNumber<=2)	{
		return _ATEM_DskOn[inputNumber-1];
	}
	return false;
}
uint16_t ATEM::getTransitionPosition() {
	return _ATEM_TrPs_position;
}
bool ATEM::getTransitionPreview()	{
	return _ATEM_TrPr;
}
uint8_t ATEM::getTransitionType()	{
	return _ATEM_TrSS_TransitionStyle;
}
uint8_t ATEM::getTransitionMixTime() {
	return _ATEM_TMxP_time;		// Transition time for Mix Transitions
}
bool ATEM::getFadeToBlackState() {
	return _ATEM_FtbS_state;    // Active state of Fade-to-black
}
uint8_t ATEM::getFadeToBlackFrameCount() {
	return _ATEM_FtbS_frameCount;    // Returns current frame in the FTB
}
uint8_t ATEM::getFadeToBlackTime() {
	return _ATEM_FtbP_time;		// Transition time for Fade-to-black
}
bool ATEM::getDownstreamKeyTie(uint8_t keyer)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		return _ATEM_DskTie[keyer-1];
	}
	return false;
}
uint8_t ATEM::getAuxState(uint8_t auxOutput)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 1 Key (13), Media 2 (14), Media 2 Key (15), Program (16), Preview (17), Clean1 (18), Clean 2 (19)

	if (auxOutput>=1 && auxOutput<=3)	{	// Todo: Should match available aux outputs
		return _ATEM_AuxS[auxOutput-1];
    }
	return 0;
}
uint8_t ATEM::getMediaPlayerType(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPType[mediaPlayer-1];	// Media Player 1/2: Type (1=Clip, 2=Still)
	}
	return 0;
}
uint8_t ATEM::getMediaPlayerStill(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPStill[mediaPlayer-1]+1;	// Still number (if MPType==2)
	}
	return 0;
}
uint8_t ATEM::getMediaPlayerClip(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		return _ATEM_MPClip[mediaPlayer-1]+1;	// Clip number (if MPType==1)
	}
	return 0;
}
uint16_t ATEM::getAudioLevels(uint8_t channel)	{
		// channel can be 0 (L) or 1 (R)
	return _ATEM_AMLv[channel];
}
uint8_t ATEM::getAudioChannelMode(uint8_t channelNumber)	{
	if (channelNumber<13)	{
/*		0: MASTER
		1: (Monitor?)
		2-9: HDMI1 - SDI8
		10: MP1
		11: MP2
		12: EXT*/
		return _ATEM_AudioChannelMode[channelNumber];
	}

	return 0;
}


/********************************
 *
 * ATEM Switcher Change methods
 * Asks the switcher to changes something
 *
 ********************************/



void ATEM::changeProgramInput(uint8_t inputNumber)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 2 (14)

  uint8_t commandBytes[4] = {0, inputNumber, 0, 0};
  _sendCommandPacket("CPgI", commandBytes, 4);
}
void ATEM::changePreviewInput(uint8_t inputNumber)  {
  // TODO: Validate that input number exists on current model!

  _wipeCleanPacketBuffer();
  _packetBuffer[1] = inputNumber;
  _sendPacketBufferCmdData("CPvI", 4);
}
void ATEM::doCut()	{
  _wipeCleanPacketBuffer();
  _packetBuffer[1] = 0xef;
  _packetBuffer[2] = 0xbf;
  _packetBuffer[3] = 0x5f;
  _sendPacketBufferCmdData("DCut", 4);
}
void ATEM::doAuto()	{
  _wipeCleanPacketBuffer();
  _packetBuffer[1] = 0x32;
  _packetBuffer[2] = 0x16;
  _packetBuffer[3] = 0x02;
  _sendPacketBufferCmdData("DAut", 4);
}
void ATEM::fadeToBlackActivate()	{
  _wipeCleanPacketBuffer();
  _packetBuffer[1] = 0x02;
  _packetBuffer[2] = 0x58;
  _packetBuffer[3] = 0x99;
  _sendPacketBufferCmdData("FtbA", 4);	// Reflected back from ATEM in "FtbS"
}
void ATEM::changeTransitionPosition(u16_t value)	{
	if (value>0 && value<=1000)	{
		uint8_t commandBytes[4] = {0, 0xe4, (value*10)/256, (value*10)%256};
		_sendCommandPacket("CTPs", commandBytes, 4);  // Change Transition Position (CTPs)
	}
}
void ATEM::changeTransitionPositionDone()	{	// When the last value of the transition is sent (1000), send this one too (we are done, change tally lights and preview bus!)
	uint8_t commandBytes[4] = {0, 0xf6, 0, 0};  	// Done
	_sendCommandPacket("CTPs", commandBytes, 4);  // Change Transition Position (CTPs)
}
void ATEM::changeTransitionPreview(bool state)	{
	uint8_t commandBytes[4] = {0x00, state ? 0x01 : 0x00, 0x00, 0x00};
	_sendCommandPacket("CTPr", commandBytes, 4);	// Reflected back from ATEM in "TrPr"
}
void ATEM::changeTransitionType(uint8_t type)	{
	if (type <= 4)	{	// 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
		uint8_t commandBytes[4] = {0x01, 0x00, type, 0x02};
		_sendCommandPacket("CTTp", commandBytes, 4);	// Reflected back from ATEM in "TrSS"
	}
}
void ATEM::changeTransitionMixTime(uint8_t frames)	{
	if (frames>=1 && frames<=0xFA)	{
		uint8_t commandBytes[4] = {0x00, frames, 0x00, 0x00};
		_sendCommandPacket("CTMx", commandBytes, 4);	// Reflected back from ATEM in "TMxP"
	}
}
void ATEM::changeFadeToBlackTime(uint8_t frames)	{
	if (frames>=1 && frames<=0xFA)	{
		uint8_t commandBytes[4] = {0x01, 0x00, frames, 0x02};
		_sendCommandPacket("FtbC", commandBytes, 4);	// Reflected back from ATEM in "FtbP"
	}
}
void ATEM::changeUpstreamKeyOn(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
	  _wipeCleanPacketBuffer();
	  _packetBuffer[1] = keyer-1;
	  _packetBuffer[2] = state ? 0x01 : 0x00;
	  _packetBuffer[3] = 0x90;
	  _sendPacketBufferCmdData("CKOn", 4);	// Reflected back from ATEM in "KeOn"
	}
}
void ATEM::changeUpstreamKeyNextTransition(uint8_t keyer, bool state)	{	// Not supporting "Background"
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		uint8_t stateValue = _ATEM_TrSS_KeyersOnNextTransition;
		if (state)	{
			stateValue = stateValue | (0b10 << (keyer-1));
		} else {
			stateValue = stateValue & (~(0b10 << (keyer-1)));
		}
				// TODO: Requires internal storage of state here so we can preserve all other states when changing the one we want to change.
		uint8_t commandBytes[4] = {0x02, 0x00, 0x6a, stateValue & 0b11111};
		_sendCommandPacket("CTTp", commandBytes, 4);	// Reflected back from ATEM in "TrSS"
	}
}
void ATEM::changeDownstreamKeyOn(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?

		uint8_t commandBytes[4] = {keyer-1, state ? 0x01 : 0x00, 0xff, 0xff};
		_sendCommandPacket("CDsL", commandBytes, 4);	// Reflected back from ATEM in "DskP" and "DskS"
	}
}
void ATEM::changeDownstreamKeyTie(uint8_t keyer, bool state)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		uint8_t commandBytes[4] = {keyer-1, state ? 0x01 : 0x00, 0xff, 0xff};
		_sendCommandPacket("CDsT", commandBytes, 4);
	}
}
void ATEM::doAutoDownstreamKeyer(uint8_t keyer)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
  		uint8_t commandBytes[4] = {keyer-1, 0x32, 0x16, 0x02};	// I don't know what that actually means...
  		_sendCommandPacket("DDsA", commandBytes, 4);
	}
}
void ATEM::changeAuxState(uint8_t auxOutput, uint8_t inputNumber)  {
  // TODO: Validate that input number exists on current model!
	// On ATEM 1M/E: Black (0), 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8), Bars (9), Color1 (10), Color 2 (11), Media 1 (12), Media 1 Key (13), Media 2 (14), Media 2 Key (15), Program (16), Preview (17), Clean1 (18), Clean 2 (19)

	if (auxOutput>=1 && auxOutput<=3)	{	// Todo: Should match available aux outputs
  		uint8_t commandBytes[4] = {auxOutput-1, inputNumber, 0, 0};
  		_sendCommandPacket("CAuS", commandBytes, 4);
		//Serial.print("freeMemory()=");
		//Serial.println(freeMemory());
    }
}
void ATEM::settingsMemorySave()	{
	uint8_t commandBytes[4] = {0, 0, 0, 0};
	_sendCommandPacket("SRsv", commandBytes, 4);
}
void ATEM::settingsMemoryClear()	{
	uint8_t commandBytes[4] = {0, 0, 0, 0};
	_sendCommandPacket("SRcl", commandBytes, 4);
}
void ATEM::changeColorValue(uint8_t colorGenerator, uint16_t hue, uint16_t saturation, uint16_t lightness)  {
	if (colorGenerator>=1 && colorGenerator<=2
			&& hue<=3600
			&& saturation <=1000
			&& lightness <= 1000
		)	{	// Todo: Should match available aux outputs
  		uint8_t commandBytes[8] = {0x07, colorGenerator-1,
			highByte(hue), lowByte(hue),
			highByte(saturation), lowByte(saturation),
			highByte(lightness), lowByte(lightness)
							};
  		_sendCommandPacket("CClV", commandBytes, 8);
    }
}
void ATEM::mediaPlayerSelectSource(uint8_t mediaPlayer, bool movieclip, uint8_t sourceIndex)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{	// TODO: Adjust to particular ATEM model... (here 1M/E)
		uint8_t commandBytes[12];
		memset(commandBytes, 0, 12);
  		commandBytes[1] = mediaPlayer-1;
		if (movieclip)	{
			commandBytes[0] = 4;
			if (sourceIndex>=1 && sourceIndex<=2)	{
				commandBytes[4] = sourceIndex-1;
			}
		} else {
			commandBytes[0] = 2;
			if (sourceIndex>=1 && sourceIndex<=32)	{
				commandBytes[3] = sourceIndex-1;
			}
		}
		commandBytes[9] = 0x10;
		_sendCommandPacket("MPSS", commandBytes, 12);

			// For some reason you have to send this command immediate after (or in fact it could be in the same packet)
			// If not done, the clip will not change if there is a shift from stills to clips or vice versa.
		uint8_t commandBytes2[8] = {0x01, mediaPlayer-1, movieclip?2:1, 0xbf, movieclip?0x96:0xd5, 0xb6, 0x04, 0};
		_sendCommandPacket("MPSS", commandBytes2, 8);
	}
}

void ATEM::mediaPlayerClipStart(uint8_t mediaPlayer)  {
	if (mediaPlayer>=1 && mediaPlayer<=2)	{
		uint8_t commandBytes2[8] = {0x01, mediaPlayer-1, 0x01, 0xbf, 0x21, 0xa9, 0x94, 0xfa}; // 3rd byte is "start", remaining 5 bytes seems random...
		_sendCommandPacket("SCPS", commandBytes2, 8);
	}
}


void ATEM::changeSwitcherVideoFormat(uint8_t format)	{
	// Changing the video format it uses: 525i59.94 NTSC (0), 625i50 PAL (1), 720p50 (2), 720p59.94 (3), 1080i50 (4), 1080i59.94 (5)
	if (format <= 5)	{	// Todo: Should match available aux outputs
  		uint8_t commandBytes[4] = {format, 0xeb, 0xff, 0xbf};
  		_sendCommandPacket("CVdM", commandBytes, 4);
    }
}



void ATEM::changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize)	{	// TEMP
  		uint8_t commandBytes[64] = {0x00, 0x00, 0x00, 0b1111, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, highByte(Xsize), lowByte(Xsize), 0x00, 0x00, highByte(Ysize), lowByte(Ysize), (Xpos >>24) & 0xFF, (Xpos >>16) & 0xFF, (Xpos >>8) & 0xFF, (Xpos >>0) & 0xFF, (Ypos >>24) & 0xFF, (Ypos >>16) & 0xFF, (Ypos >>8) & 0xFF, (Ypos >>0) & 0xFF, 0xbf, 0xff, 0xdb, 0x7f, 0xc2, 0xa2, 0x09, 0x90, 0xdb, 0x7e, 0xbf, 0xff, 0x82, 0x34, 0x2e, 0xB, 0x05, 0x00, 0x00, 0x00, 0x34, 0xc1, 0x00, 0x2c, 0xe2, 0x00, 0x4e, 0x02, 0xa3, 0x98, 0xac, 0x02, 0xdb, 0xd9, 0xbf, 0xff, 0x74, 0x34, 0xe9, 0x01};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}


void ATEM::changeDVESettingsTemp_Rate(uint8_t rateFrames)	{	// TEMP
  		uint8_t commandBytes[64] = {0b100, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xff, 0xdb, 0x7f, 0xc2, 0xa2, 0x09, 0x90, 0xdb, 0x7e, 0xbf, 0xff, 0x82, 0x34, 0x2e, 0xB, 0x05, 0x00, 0x00, 0x00, 0x34, 0xc1, 0x00, 0x2c, 0xe2, 0x00, 0x4e, 0x02, 0xa3, 0x98, 0xac, 0x02, 0xdb, 0xd9, 0xbf, 0xff, rateFrames, 0x34, 0xe9, 0x01};
  		_sendCommandPacket("CKDV", commandBytes, 64);
}
void ATEM::changeDVESettingsTemp_RunKeyFrame(uint8_t runType)	{	// runType: 1=A, 2=B, 3=Full, 4=on of the others (with an extra paramter:)
  		uint8_t commandBytes[8] = {0x02, 0x00, 0x00, 0x02, 0x00, runType, 0xff, 0xff};
  		_sendCommandPacket("RFlK", commandBytes, 8);
}
void ATEM::changeKeyerMask(uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
		// In "B11110", bits are (from right to left): 0=?, 1=topMask, 2=bottomMask, 3=leftMask, 4=rightMask
  		uint8_t commandBytes[12] = {0b11110, 0x00, 0x00, 0x00, highByte(topMask), lowByte(topMask), highByte(bottomMask), lowByte(bottomMask), highByte(leftMask), lowByte(leftMask), highByte(rightMask), lowByte(rightMask)};
  		_sendCommandPacket("CKMs", commandBytes, 12);
}

void ATEM::changeDownstreamKeyMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
		// In "B11110", bits are (from right to left): 0=?, 1=topMask, 2=bottomMask, 3=leftMask, 4=rightMask
		if (keyer>=1 && keyer<=2)	{
  			uint8_t commandBytes[12] = {0b11110, keyer-1, 0x00, 0x00, highByte(topMask), lowByte(topMask), highByte(bottomMask), lowByte(bottomMask), highByte(leftMask), lowByte(leftMask), highByte(rightMask), lowByte(rightMask)};
  			_sendCommandPacket("CDsM", commandBytes, 12);
		}
}



// Statuskode retur: KeBP, data byte 7 derefter er fill source, databyte 8 er key source, data byte 2 er upstr. keyer 1-4 (0-3)
// Key source command er : CKeC - og ellers ens med...
void ATEM::changeUpstreamKeyFillSource(uint8_t keyer, uint8_t inputNumber)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		uint8_t commandBytes[4] = {0, keyer-1, inputNumber, 0};
		_sendCommandPacket("CKeF", commandBytes, 4);
	}
}

// Statuskode retur: DskB, data byte 2 derefter er fill source, data byte 3 er key source, data byte 1 er keyer 1-2 (0-1)
// Key source command er : CDsC - og ellers ens med...
void ATEM::changeDownstreamKeyFillSource(uint8_t keyer, uint8_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		uint8_t commandBytes[4] = {keyer-1, inputNumber, 0, 0};
		_sendCommandPacket("CDsF", commandBytes, 4);
	}
}

void ATEM::changeDownstreamKeyKeySource(uint8_t keyer, uint8_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
	  	// TODO: Validate that input number exists on current model!
		// 0-15 on 1M/E
		uint8_t commandBytes[4] = {keyer-1, inputNumber, 0, 0};
		_sendCommandPacket("CDsC", commandBytes, 4);
	}
}

void ATEM::changeAudioChannelMode(uint8_t channelNumber, uint8_t mode)	{	// Mode: 0=Off, 1=On, 2=AFV
  if (mode<=2)	{
	  _wipeCleanPacketBuffer();
	  _packetBuffer[0] = 0x01;	// Setting ON/OFF/AFV
	  _packetBuffer[1] = channelNumber;	// Input 1-8 = channel 0-7(!), Media Player 1+2 = channel 8-9, Ext = channel 10 (For 1M/E!)
	  _packetBuffer[2] = mode;	// 0=Off, 1=On, 2=AFV
	  _packetBuffer[3] = 0x03;
	  _sendPacketBufferCmdData("CAMI", 8);	// Reflected back from ATEM as "AMIP"
  }
}
void ATEM::changeAudioChannelVolume(uint8_t channelNumber, uint16_t volume)	{

	/*
	Based on data from the ATEM switcher, this is an approximation to the integer value vs. the dB value:
	dB	+60 added	Number from protocol		Interpolated
	6	66	65381		65381
	3	63	46286		46301,04
	0	60	32768		32789,13
	-3	57	23198		23220,37
	-6	54	16423		16444,03
	-9	51	11627		11645,22
	-20	40	3377		3285,93
	-30	30	1036		1040,21
	-40	20	328		329,3
	-50	10	104		104,24
	-60	0	33		33

	for (int i=-60; i<=6; i=i+3)  {
	   Serial.print(i);
	   Serial.print(" dB = ");
	   Serial.print(33*pow(1.121898585, i+60));
	   Serial.println();
	}


	*/

// CAMI command structure:  	CAMI    [01=buttons, 02=vol, 04=pan (toggle bits)] - [input number, 0-�] - [buttons] - [buttons] - [vol] - [vol] - [pan] - [pan]
// CAMM: 01:de:80:00:e4:10:ff:bf (master) [volume is 80:00]

  _wipeCleanPacketBuffer();
  _packetBuffer[0] = 0x02;	// Setting Volume Level
  _packetBuffer[1] = channelNumber;	// Input 1-8 = channel 0-7(!), Media Player 1+2 = channel 8-9, Ext = channel 10 (For 1M/E!)		///		Input 1-6 = channel 0-5(!), Ext = channel 6 (For TVS!)
//  _packetBuffer[2] = 0xff;	// ??
//  _packetBuffer[3] = 0xbf;	// ??

	if (volume > 0xff65)	{
		volume = 0xff65;
	}

  _packetBuffer[4] = volume/256;
  _packetBuffer[5] = volume%256;
//  _packetBuffer[6] = volume/256;
//  _packetBuffer[7] = volume%256;

//  _packetBuffer[8] = 0x57;	// ??
//  _packetBuffer[9] = 0x84;	// ??
//  _packetBuffer[10] = 0xb3;	// ??
//  _packetBuffer[11] = 0xac;	// ??

  _sendPacketBufferCmdData("CAMI", 8);
}

void ATEM::changeAudioMasterVolume(uint16_t volume)	{

// CAMI command structure:  	CAMI    [01=but, 02=vol, 04=pan (toggle bits)] - [input number, 0-�] - [buttons] - [buttons] - [vol] - [vol] - [pan] - [pan]
// CAMM: 01:de:80:00:e4:10:ff:bf (master) [volume is 80:00]

  _wipeCleanPacketBuffer();
  _packetBuffer[0] = 0x01;

	if (volume > 0xff65)	{
		volume = 0xff65;
	}

  _packetBuffer[2] = volume/256;
  _packetBuffer[3] = volume%256;

  _sendPacketBufferCmdData("CAMM", 8);
}
void ATEM::sendAudioLevelNumbers(bool enable)	{
  _wipeCleanPacketBuffer();
  _packetBuffer[0] = enable ? 1 : 0;
  _sendPacketBufferCmdData("SALN", 4);
}
void ATEM::setAudioLevelReadoutChannel(uint8_t AMLv)	{
				/*
				Channels on an 1M/E: (16 byte segments:)
				0: MASTER
				1: (Monitor?)
				2-9: HDMI1 - SDI8
				10: MP1
				11: MP2
				12: EXT

Values:
FCP			HyperDeck	ATEM			Value
Output		Studio 		Input			in
Level:		Playback:	Colors:			Protocol:

0 			red			red				32767
-3 			red			red				23228
-6 			red/yellow	red				16444
-9 			yellow		red/yellow		11640
-12 		yellow		yellow			8240
-18 		green		yellow/green	4130
-24			green		green			2070
-42			green		green			260


(Values = 32809,85 * 1,12^dB (trendline based on numbers above))
(HyperDeck Studio: red=>yellow @ -6db, yellow=>green @ -15db (assumed))
(ATEM: red=>yellow @ -9db, yellow=>green @ -20db)
(ATEM Input registered the exact same level values, FCP had been writing to the ProRes 422 file.)

				*/
	_ATEM_AMLv_channel = AMLv;	// Should check that it's in range 0-12
}

#pragma GCC diagnostic pop
