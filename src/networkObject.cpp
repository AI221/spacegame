#include "networkObject.h"
#ifdef NETWORKING_ENABLED

/*void paraseCharToBytes(char* originalData, void* buffer, size_t bufferSize)
{
	for (int i; i<bufferSize;i++)
	{
		buffer  = buffer << (8*i) | buffer;
	}
}

JUST MEMCPY OVER THE CHAR* ARRAY. NO NEED FOR THIS BULLSHIT

*/

//some more thoughts, all those uis and stuff that are using glue can be left as-is if glue is fully networked...

/*

void parseAddRenderedObject(char* bytes, size_t size)
{


}


void parse(GE_NetworkSocket* recieverSocket)
{
	char buffer[256];


	GE_Read(recieverSocket,buffer,sizeof(buffer));

	//parse
	


	char instructionBuffer[256];
	int countInstructionBuffer = 0;

	int ibyte = 0;


	bool icomingInstruction;
	char currentInstruction;
	while(true)
	{
		char byte = buffer[ibyte];
		if (byte == GE_NETWORKING_INSTRUCTION_INCOMING_INSTURCTION_BYTE)
		{
			if (icomingInstruction) //they want to insert the same byte that incoming instruction is represented by; similar to double slashes
			{
				instructionBuffer[countInstructionBuffer] =  GE_NETWORKING_INSTRUCTION_INCOMING_INSTURCTION_BYTE;
				countInstructionBuffer++;
				icomingInstruction = false;
			}
			else
			{
				icomingInstruction = true;
			}
		}
		else if (icomingInstruction)
		{
			if (byte == GE_NETWORKING_INSTRUCTION_END_TICK_TRANSMISSION)
			{
				break;
			}
			currentInstruction = byte;
			icomingInstruction = false;
		}
		else if (byte == GE_NETWORKING_INSTRUCTION_END_INSTRUCTION)
		{
			switch (currentInstruction)
			{
				case GE_NETWORKING_INSTRUCTION_NEW_RENDERED_OBJECT:
					

					break;

			}
		}
		//IDEA: Add an instruction to set the value of a GlueObject buffer. Would be to any arbitrary size_t, with a STOP after the end. Would need some protection, obviously.





	
		ibyte++;
		


	}



}


*/
#endif
