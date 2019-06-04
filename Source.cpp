#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>

//branching, jumping, dual issuing

using namespace std;

int totalPredictions = 0;
int misprediction = 0;
int instBufH ;     // Head for the instruction buffer
int instBufT ;	  // Tail for the instruction buffer

int LW_R;// = 2, 
int LW_C;//= 1 - 1; //+1 to compute the address
int SW_R; //= 2, 
int SW_C;// = 1 - 1; //+1 to compute the address
int JMP_R; //= 3, 
int JMP_C;// = 1 - 1;
int BEQ_R; //= 2, 
int BEQ_C;// = 1 - 1;
int ADD_R; //= 3, 
int ADD_C; //= 2 - 1;
int NAND_R; //= 1, 
int NAND_C;// = 1 - 1;
int MULT_R; //= 2, 
int MULT_C;// = 8 - 1;
int ROB_entries;// = 6;
int instBufferSize;// = 4;

struct FU {
	//string Name;
	char Busy;
	string Op;
	int Vj,Vk,Qj,Qk,Dest,A;
	int EXEcycle; //to trace number of execution cycles
	int instNum;
	int wReady;
	int cReady;
};

struct ROBentry {
	string Type;
	int Dest, Value;
	char Ready;
	int instNum;
};

struct Register {
	//string name;
	int Val, ROB;
};

struct DataItem {
	int Addr, Val;
};

// Instruction Format
struct Inst {
	string type;
	string rd,op1,op2; 

};

struct InstBuffer {
	Inst inst;
	bool validBit;
	int instNum;
};


struct InstTableEntry {
	Inst inst;
	int F, I, E, W, C;
};

/*int ROBFree(ROBentry ROB[], int ROB_entries)
{
	for (int i = 0; i < ROB_entries; i++)
	{
		if(ROB[i].)
	}
}*/

int getIndex(DataItem Data[], int Addr, int NumOfDataItems) //to get the index of a certain address in the data memory
{
	for (int i = 0; i < NumOfDataItems; i++)
		if (Data[i].Addr == Addr)
			return i;
}

void execute(FU inst, ROBentry ROB[], DataItem Data[], int NumOfDataItems)
{
	inst.EXEcycle--;
	if (inst.Op == "LW")
	{
		ROB[inst.Dest].Value = Data[getIndex(Data, inst.A + inst.Vj, NumOfDataItems)].Val;
	}
	else if (inst.Op == "SW")
	{
		ROB[inst.Dest].Value = inst.Vk; //the value to be stored in the memory
		ROB[inst.Dest].Dest = inst.A + inst.Vj; //address = Vj + A
		//Data[getIndex(Data, inst.A + inst.Vj, NumOfDataItems)].Val=; WRITE BACK
	}
	else if (inst.Op == "JMP")
	{
		//ROB[inst.Dest].Value = inst.Vk; //Vk equals the immediate value
	}
	else if (inst.Op == "BEQ")
	{
		totalPredictions++;
		if (inst.Vj == inst.Vk) //the calculated decision is stored in ROB value, while the prediction is stored in ROB Dest
			ROB[inst.Dest].Value = inst.A; //T=1, NT=0;
		else
			ROB[inst.Dest].Value = inst.instNum+1;
		if (ROB[inst.Dest].Value != ROB[inst.Dest].Dest)
		{
			misprediction++;
		}

	}
	else if (inst.Op == "JALR")
	{
		//inst.A = inst.Vk; //address stored in Vk
		ROB[inst.Dest].Value = inst.Vj;
	}
	else if (inst.Op == "RET")
	{
		//ROB[inst.Dest].Value = inst.Vk; 
		//inst.A = inst.Vk;
	}
	else if (inst.Op == "ADD")
	{
		ROB[inst.Dest].Value = inst.Vj + inst.Vk;
	}
	else if (inst.Op == "SUB")
	{
		ROB[inst.Dest].Value = inst.Vj - inst.Vk;
	}
	else if (inst.Op == "ADDI")
	{
		ROB[inst.Dest].Value = inst.Vj + inst.Vk;
	}
	else if (inst.Op == "NAND")
	{
		ROB[inst.Dest].Value = inst.Vj & inst.Vk;
		ROB[inst.Dest].Value = ~ ROB[inst.Dest].Value;
	}
	else if (inst.Op == "MUL")
	{
		ROB[inst.Dest].Value = inst.Vj * inst.Vk;
	}
}

void writeBack(FU RSLoad[], FU RSStore[], FU RSJmp[], FU RSBeq[], FU RSAdd[], FU RSNand[], FU RSMul[], ROBentry ROB[], int dest)
{

	ROB[dest].Ready = 'Y';
	for (int i = 0; i < LW_R; i++)
	{
		if (RSLoad[i].Busy == 'Y')
		{

			if (RSLoad[i].Qj == dest)
			{
				RSLoad[i].Qj = -1;
				RSLoad[i].Vj = ROB[dest].Value;
			}
			if (RSLoad[i].Qk == dest)
			{
				RSLoad[i].Qk = -1;
				RSLoad[i].Vk = ROB[dest].Value;
			}
		}

	}
	for (int i = 0; i < SW_R; i++)
	{
		if (RSStore[i].Busy == 'Y')
		{
			if (RSStore[i].Qj == dest)
			{
				RSStore[i].Qj = -1;
				RSStore[i].Vj = ROB[dest].Value;
			}
			if (RSStore[i].Qk == dest)
			{
				RSStore[i].Qk = -1;
				RSStore[i].Vk = ROB[dest].Value;
			}
		}
	}

	for (int i = 0; i < JMP_R; i++)
	{
		if (RSJmp[i].Busy == 'Y')
		{
			if (RSJmp[i].Qj == dest)
			{
				RSJmp[i].Qj = -1;
				RSJmp[i].Vj = ROB[dest].Value;
			}
			if (RSJmp[i].Qk == dest)
			{
				RSJmp[i].Qk = -1;
				RSJmp[i].Vk = ROB[dest].Value;
			}
		}
	}
	
	for (int i = 0; i < BEQ_R; i++)
	{
		if (RSBeq[i].Busy == 'Y')
		{
			if (RSBeq[i].Qj == dest)
			{
				RSBeq[i].Qj = -1;
				RSBeq[i].Vj = ROB[dest].Value;
			}
			if (RSBeq[i].Qk == dest)
			{
				RSBeq[i].Qk = -1;
				RSBeq[i].Vk = ROB[dest].Value;
			}
		}
	}
	
	for (int i = 0; i < ADD_R; i++)
	{
		if (RSAdd[i].Busy == 'Y')
		{
			if (RSAdd[i].Qj == dest)
			{
				RSAdd[i].Qj = -1;
				RSAdd[i].Vj = ROB[dest].Value;
			}
			if (RSAdd[i].Qk == dest)
			{
				RSAdd[i].Qk = -1;
				RSAdd[i].Vk = ROB[dest].Value;
			}
		}
	}
	for (int i = 0; i < NAND_R; i++) 
	{
		if (RSNand[i].Busy == 'Y')
		{
			if (RSNand[i].Qj == dest)
			{
				RSNand[i].Qj = -1;
				RSNand[i].Vj = ROB[dest].Value;
			}
			if (RSNand[i].Qk == dest)
			{
				RSNand[i].Qk = -1;
				RSNand[i].Vk = ROB[dest].Value;
			}
		}
	}
	for (int i = 0; i < MULT_R; i++)
	{
		if (RSMul[i].Busy == 'Y')
		{
			if (RSMul[i].Qj == dest)
			{
				RSMul[i].Qj = -1;
				RSMul[i].Vj = ROB[dest].Value;
			}
			if (RSMul[i].Qk == dest)
			{
				RSMul[i].Qk = -1;
				RSMul[i].Vk = ROB[dest].Value;
			}
		}
	}
}


void FlushRS_ROB_Buffer(FU RSLoad[], FU RSStore[], FU RSJmp[], FU RSBeq[], FU RSAdd[], FU RSNand[], FU RSMul[], ROBentry ROB[], InstBuffer instBuffer[])
{
	instBufH = 0;
	instBufT = 0;
	for (int i = 0; i < instBufferSize; i++)
	{
		instBuffer[i].validBit = 0;
	}
	for (int i = 0; i < ROB_entries; i++)
	{
		ROB[i].Type = "FREE";
		ROB[i].Dest = -1;
		ROB[i].instNum = -1;
		ROB[i].Ready = 'N';
		ROB[i].Value = -1;

	}
	for (int i = 0; i < LW_R; i++)
	{
		RSLoad[i].Busy = 'N';
		RSLoad[i].Qj = -1;
		RSLoad[i].Qk = -1;
		RSLoad[i].EXEcycle = -1;
		RSLoad[i].wReady = 0;
	}
	for (int i = 0; i < SW_R; i++)
	{
		RSStore[i].Busy = 'N';
		RSStore[i].Qj = -1;
		RSStore[i].Qk = -1;
		RSStore[i].EXEcycle = -1;
		RSStore[i].wReady = 0;
	}

	for (int i = 0; i < JMP_R; i++)
	{
		RSJmp[i].Busy = 'N';
		RSJmp[i].Qj = -1;
		RSJmp[i].Qk = -1;
		RSJmp[i].EXEcycle = -1;
		RSJmp[i].wReady = 0;
	}
	for (int i = 0; i < BEQ_R; i++)
	{
		RSBeq[i].Busy = 'N';
		RSBeq[i].Qj = -1;
		RSBeq[i].Qk = -1;
		RSBeq[i].EXEcycle = -1;
		RSBeq[i].wReady = 0;
	}
	for (int i = 0; i < ADD_R; i++)
	{
		RSAdd[i].Busy = 'N';
		RSAdd[i].Qj = -1;
		RSAdd[i].Qk = -1;
		RSAdd[i].EXEcycle = -1;
		RSAdd[i].wReady = 0;
	}
	for (int i = 0; i < NAND_R; i++)
	{
		RSNand[i].Busy = 'N';
		RSNand[i].Qj = -1;
		RSNand[i].Qk = -1;
		RSNand[i].EXEcycle = -1;
		RSNand[i].wReady = 0;
	}
	for (int i = 0; i < MULT_R; i++)
	{
		RSMul[i].Busy = 'N';
		RSMul[i].Qj = -1;
		RSMul[i].Qk = -1;
		RSMul[i].EXEcycle = -1;
		RSMul[i].wReady = 0;
	}
}

bool checkInstBuffer(InstBuffer instBuffer[])
{
	for (int i = 0; i < instBufferSize; i++)
		if (instBuffer[i].validBit == 1)
			return true;
	return false;
}

int main()
{
	//Tables Definitions
	Register Reg[8];
	InstTableEntry InstTable[1000];
	InstBuffer instBuffer[10]; //assuming max instruction buffer size = 10
	ROBentry ROB[10]; //assuming max ROB size = 10
	FU RSLoad[5], RSStore[5], RSJmp[5], RSBeq[5], RSAdd[5], RSNand[5], RSMul[5]; //assume max reservation station length of each type = 5

	cout << "Enter the number of reservation stations and the number of cycles needed for each type" << endl;
	cin >> LW_R >> LW_C >> SW_R >> SW_C >> JMP_R >> JMP_C >> BEQ_R >> BEQ_C >> ADD_R >> ADD_C >> NAND_R >> NAND_C >> MULT_R >> MULT_C;
	


	cout << "Enter the number of ROB entries" << endl;
	cin >> ROB_entries;// = 6;
	cout << "Enter the buffer size" << endl;
	cin >> instBufferSize;// = 4;

	//Initialization
	for (int i = 0; i < 8; i++)
	{
		Reg[i].Val = 0; 
		Reg[i].ROB = -1;
	}

	for (int i = 0; i < instBufferSize; i++)
		instBuffer[i].validBit = 0;
	
	FlushRS_ROB_Buffer(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, instBuffer);
	
	//start taking input from user
	int IC; //instruction count
	Inst InstList[1000];
	int startingAddr;
	int NumOfDataItems;
	DataItem Data[1000];
	//bool write = 0;

	cout << "Program:" << endl;
	cout << "Enter the number of instructions, please" << endl;
	cin >> IC; //assume max IC equals 1000
	cout << "Insert the program here, please" << endl;

	for (int i = 0; i < IC; i++)
	{
		/*Assumption: for branch and jump instructions,
		instead of using labels the using should input the index of
		the target instruction offset*/

		cin >> InstList[i].type;
		char e; //to get the white space
		e = getchar();
		if (InstList[i].type == "JMP" || InstList[i].type == "RET")
		{
			InstList[i].rd = "";
			InstList[i].op1 = "";
			getline(cin, InstList[i].op2);
		}
		else if (InstList[i].type == "JALR")
		{
			getline(cin, InstList[i].rd, ',');
			getline(cin, InstList[i].op2);
		}
		else
		{
			getline(cin, InstList[i].rd, ',');
			getline(cin, InstList[i].op1, ',');
			getline(cin, InstList[i].op2);
		}

	}


	cout << "Starting Address: ";
	cin >> startingAddr;
	cout << "Enter the number of data items, please" << endl;
	cin >> NumOfDataItems; //assume max NumOfDataItems =1000
	cout << "Data Items:" << endl;
	for (int i = 0; i < NumOfDataItems; i++)
	{
		cout << "Address: ";
		cin >> Data[i].Addr;
		cout << "Value: ";
		cin >> Data[i].Val;
	}

	int clk = 0; //representing the current clock cycle
	int ROBh = 0; int ROBt=0;

	int instCounter = 0;
	int stall = 0;
	int incrementHandler = 0;
	int committed = 0;
	bool Finish = true;
	bool CommitF = true;

	LW_C--;  SW_C--;  JMP_C--; BEQ_C--; ADD_C--;  NAND_C--; MULT_C--; //Because in the execution it loops from 0 to the total number of cycles inclusive

	while (committed < IC && Finish)
	{
		// COMMITTED
		while (ROB[ROBh].Ready == 'Y' && CommitF)
		{
			if (ROB[ROBh].Type == "SW")
				Data[getIndex(Data, ROB[ROBh].Dest, NumOfDataItems)].Val = ROB[ROBh].Value;
			else if (ROB[ROBh].Type == "LW" || ROB[ROBh].Type == "ADD" || ROB[ROBh].Type == "SUB" || ROB[ROBh].Type == "ADDI" ||
				ROB[ROBh].Type == "MUL" || ROB[ROBh].Type == "NAND" || ROB[ROBh].Type == "LW" || ROB[ROBh].Type == "JALR")
			{
				Reg[ROB[ROBh].Dest].Val = ROB[ROBh].Value;
				Reg[ROB[ROBh].Dest].ROB = -1;
			}
			else if (ROB[ROBh].Type == "BEQ" && (ROB[ROBh].Dest != ROB[ROBh].Value))
			{
				instCounter = ROB[ROBh].Value;
				FlushRS_ROB_Buffer(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, instBuffer);
			}

			InstTable[ROB[ROBh].instNum].C = clk;
			stall = 0; //to enable issuing again if stall was =1

			//Flush the RS unit
			if (ROB[ROBh].Type == "LW")
				for (int i = 0; i < LW_R; i++)
				{
					if (RSLoad[i].Dest == ROBh && RSLoad[i].cReady == 1)
					{
						RSLoad[i].Busy = 'N';
						RSLoad[i].Qj = -1;
						RSLoad[i].Qk = -1;
						RSLoad[i].EXEcycle = -1;
						RSLoad[i].wReady = 0;
					}


				}

			if (ROB[ROBh].Type == "SW")
				for (int i = 0; i < SW_R; i++)
				{
					if (RSStore[i].Dest == ROBh && RSStore[i].cReady == 1)
					{
						RSStore[i].Busy = 'N';
						RSStore[i].Qj = -1;
						RSStore[i].Qk = -1;
						RSStore[i].EXEcycle = -1;
						RSStore[i].wReady = 0;
					}
				}

			if (ROB[ROBh].Type == "JMP")
				for (int i = 0; i < JMP_R; i++)
				{
					if (RSJmp[i].Dest == ROBh && RSJmp[i].cReady == 1)
					{
						RSJmp[i].Busy = 'N';
						RSJmp[i].Qj = -1;
						RSJmp[i].Qk = -1;
						RSJmp[i].EXEcycle = -1;
						RSJmp[i].wReady = 0;
					}

				}


			if (ROB[ROBh].Type == "BEQ")
				for (int i = 0; i < BEQ_R; i++)
				{
					if (RSBeq[i].Dest == ROBh && RSBeq[i].cReady == 1)
					{
						RSBeq[i].Busy = 'N';
						RSBeq[i].Qj = -1;
						RSBeq[i].Qk = -1;
						RSBeq[i].EXEcycle = -1;
						RSBeq[i].wReady = 0;
					}

				}


			if (ROB[ROBh].Type == "ADD" || ROB[ROBh].Type == "SUB" || ROB[ROBh].Type == "ADDI")
				for (int i = 0; i < ADD_R; i++)
				{
					if (RSAdd[i].Dest == ROBh && RSAdd[i].cReady == 1)
					{
						RSAdd[i].Busy = 'N';
						RSAdd[i].Qj = -1;
						RSAdd[i].Qk = -1;
						RSAdd[i].EXEcycle = -1;
						RSAdd[i].wReady = 0;
					}

				}

			if (ROB[ROBh].Type == "NAND")
				for (int i = 0; i < NAND_R; i++)
				{
					if (RSNand[i].Dest == ROBh && RSNand[i].cReady == 1)
					{
						RSNand[i].Busy = 'N';
						RSNand[i].Qj = -1;
						RSNand[i].Qk = -1;
						RSNand[i].EXEcycle = -1;
						RSNand[i].wReady = 0;
					}

				}

			if (ROB[ROBh].Type == "MUL")
				for (int i = 0; i < MULT_R; i++)
				{
					if (RSMul[i].Dest == ROBh && RSMul[i].cReady == 1)
					{
						RSMul[i].Busy = 'N';
						RSMul[i].Qj = -1;
						RSMul[i].Qk = -1;
						RSMul[i].EXEcycle = -1;
						RSMul[i].wReady = 0;
					}

				}
			ROB[ROBh].Type = "FREE";
			ROB[ROBh].Ready = 'N';
			ROBh = (ROBh + 1) % ROB_entries;
			if (committed > IC)
			{
				Finish = false;
				CommitF = false;
			}
			else
				committed++;
		}

		if (committed > IC)
		{
			Finish = false;
		}
		//EXECUTION && WRITE BACK

		// Execute LW 
		for (int i = 0; i < LW_R; i++)
		{
			if (RSLoad[i].wReady == 1)
			{
				writeBack (RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSLoad[i].Dest);
				RSLoad[i].cReady = 1;
				InstTable[ROB[RSLoad[i].Dest].instNum].W = clk; //timing table write
				RSLoad[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}
			 
			if (RSLoad[i].Busy == 'Y' && RSLoad[i].Qj == -1)
			{
				if (RSLoad[i].EXEcycle == -1)	// Execute didn't started 
					RSLoad[i].EXEcycle = LW_C;
				else if ((RSLoad[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSLoad[i].EXEcycle--;
				else if (RSLoad[i].EXEcycle == 0)
				{
					//this is to handle load store dependencies
					bool LoadF = true;
					for (int j = ROBh; j != RSLoad[i].Dest; j = (j + 1) % ROB_entries)
					{
						if (RSLoad[i].A == ROB[j].Dest && ROB[j].Type == "SW")
							LoadF = false;    // stall
					}
					if (LoadF)
					{
						InstTable[RSLoad[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
						execute(RSLoad[i], ROB, Data, NumOfDataItems);
						RSLoad[i].wReady = 1;	// RS ready to write 
					}
				}
			}

		}

		// Execute Store  
		for (int i = 0; i < SW_R; i++)
		{
			if (RSStore[i].wReady==1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSStore[i].Dest);
				RSStore[i].cReady = 1;
				InstTable[ROB[RSStore[i].Dest].instNum].W = clk; //timing table write
				RSStore[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}

			if (RSStore[i].Busy == 'Y' && RSStore[i].Qj == -1 && RSStore[i].Qk == -1)
			{
				if (RSStore[i].EXEcycle == -1)	// Execute didn't started 
					RSStore[i].EXEcycle = SW_C;
				else if ((RSStore[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSStore[i].EXEcycle--;
				else if (RSStore[i].EXEcycle == 0)
				{
					InstTable[RSStore[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSStore[i], ROB, Data, NumOfDataItems);
					RSStore[i].wReady = 1;	// RS ready to write
				}
			}
		}

		// Execute JMP
		for (int i = 0; i < JMP_R; i++)
		{
			if (RSJmp[i].wReady==1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSJmp[i].Dest);
				RSJmp[i].cReady = 1;
				InstTable[ROB[RSJmp[i].Dest].instNum].W = clk; //timing table write
				RSJmp[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}

			if (RSJmp[i].Busy == 'Y')
			{
				if (RSJmp[i].EXEcycle == -1)	// Execute didn't started 
					RSJmp[i].EXEcycle = JMP_C;
				else if ((RSJmp[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSJmp[i].EXEcycle--;

				else if (RSJmp[i].EXEcycle == 0)
				{
					InstTable[RSJmp[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSJmp[i], ROB, Data, NumOfDataItems);
					RSJmp[i].wReady = 1;	// RS ready to write
				}
			}
		}
		
		// Execute BEQ 
		for (int i = 0; i < BEQ_R; i++) 
		{
			if (RSBeq[i].wReady == 1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSBeq[i].Dest);
				RSBeq[i].cReady = 1;
				InstTable[ROB[RSBeq[i].Dest].instNum].W = clk; //timing table write
				RSBeq[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}
		if (RSBeq[i].Busy == 'Y' && RSBeq[i].Qj == -1 && RSBeq[i].Qk == -1)
			{
				if (RSBeq[i].EXEcycle == -1)	// Execute didn't started 
					RSBeq[i].EXEcycle = BEQ_C;
				else if ((RSBeq[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSBeq[i].EXEcycle--;

				else if (RSBeq[i].EXEcycle == 0)
				{
					InstTable[RSBeq[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSBeq[i], ROB, Data, NumOfDataItems);
					RSBeq[i].wReady = 1;	// RS ready to write
				}
			}
		}


		// Execute ALU (ADD, SUB, ADDI)
		for (int i = 0; i < ADD_R; i++) 
		{

			if (RSAdd[i].wReady == 1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSAdd[i].Dest);
				RSAdd[i].cReady = 1;
				InstTable[ROB[RSAdd[i].Dest].instNum].W = clk; //timing table write
				RSAdd[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}

			if (RSAdd[i].Busy == 'Y' && RSAdd[i].Qj == -1 && RSAdd[i].Qk == -1)
			{
				//cout << "Heeh" << endl;
				if (RSAdd[i].EXEcycle == -1)	// Execute didn't started 
					RSAdd[i].EXEcycle = ADD_C;
				else if ((RSAdd[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSAdd[i].EXEcycle--;

				else if (RSAdd[i].EXEcycle == 0)
				{
					InstTable[RSAdd[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSAdd[i], ROB, Data, NumOfDataItems);
					RSAdd[i].wReady = 1;	// RS ready to write
				}
				
			}
		}
		
		// Execute NAND 
		for (int i = 0; i < NAND_R; i++) 
		{
			if (RSNand[i].wReady == 1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSNand[i].Dest);
				RSNand[i].cReady = 1;
				InstTable[ROB[RSNand[i].Dest].instNum].W = clk; //timing table write
				RSNand[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}
			if (RSNand[i].Busy == 'Y' && RSNand[i].Qj == -1 && RSNand[i].Qk == -1)
			{
				if (RSNand[i].EXEcycle == -1)	// Execute didn't started 
					RSNand[i].EXEcycle = NAND_C;
				else if ((RSNand[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSNand[i].EXEcycle--;

				else if (RSNand[i].EXEcycle == 0)
				{
					InstTable[RSNand[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSNand[i], ROB, Data, NumOfDataItems);
					RSNand[i].wReady = 1;	// RS ready to write
				}
			}
		}
		
		// Execute MULT
		for (int i = 0; i < MULT_R; i++) 
		{
			if (RSMul[i].wReady == 1)
			{
				writeBack(RSLoad, RSStore, RSJmp, RSBeq, RSAdd, RSNand, RSMul, ROB, RSMul[i].Dest);
				RSMul[i].cReady = 1;
				InstTable[ROB[RSMul[i].Dest].instNum].W = clk; //timing table write
				RSMul[i].wReady = 0;   //set the write ready bit to 0 == Free the RS 
			}

			if (RSMul[i].Busy == 'Y' && RSMul[i].Qj == -1 && RSMul[i].Qk == -1)
			{
				if (RSMul[i].EXEcycle == -1)	// Execute didn't started 
					RSMul[i].EXEcycle = MULT_C;
				else if ((RSMul[i].EXEcycle > 0)) // Execute started  & decrement the needed cycles to finish the execution
					RSMul[i].EXEcycle--;

				else if (RSMul[i].EXEcycle == 0)
				{
					InstTable[RSMul[i].instNum].E = clk;  // Add the clk cycle to the Timing Table
					execute(RSMul[i], ROB, Data, NumOfDataItems);
					RSMul[i].wReady = 1;	// RS ready to write
				}
			}
		}
		
		for (int q = 0; q < 2; q++)
		{
			//ISSUING
			if (!stall && checkInstBuffer(instBuffer))
			{
				if (instBuffer[instBufH].inst.type == "LW")
				{
					int i = 0;
					while (RSLoad[i].Busy != 'N' && i < LW_R) i++;

					if (LW_R == i) stall = 1;  // Check if the RS is Free 
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSLoad[i].Busy = 'Y';
							RSLoad[i].Op = "LW";

							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSLoad[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSLoad[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSLoad[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							RSLoad[i].Qk = -1;
							RSLoad[i].Dest = ROBt;
							RSLoad[i].A = atoi(instBuffer[instBufH].inst.op2.c_str());
							Reg[int(instBuffer[instBufH].inst.rd.at(1)) - int('0')].ROB = ROBt; //write in the register array the value of the ROB
							ROB[ROBt].Dest = int(instBuffer[instBufH].inst.rd.at(1) - int('0')); //write the destination in the ROB
							RSLoad[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "SW")
				{
					int i = 0;
					while (RSStore[i].Busy != 'N' && i < SW_R) i++;

					if (SW_R == i) stall = 1;  // Check if the RS is Free 
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSStore[i].Busy = 'Y';
							RSStore[i].Op = "SW";

							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1) // Check availablity of Reg operand OR the the ROB
							{
								RSStore[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val; // Val of Reg
								RSStore[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSStore[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							if (Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB == -1) // Check availablity of Reg operand OR the the ROB
							{
								RSStore[i].Vk = Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].Val; // Val of Reg
								RSStore[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSStore[i].Qk = Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB;

							RSStore[i].Dest = ROBt;
							RSStore[i].A = atoi(instBuffer[instBufH].inst.op2.c_str());
							//Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB = ROBt++;
							RSStore[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "ADD" || instBuffer[instBufH].inst.type == "SUB" || instBuffer[instBufH].inst.type == "ADDI")
				{
					int i = 0;
					while (RSAdd[i].Busy != 'N' && i < ADD_R) i++;  // Check if the RS is Free

					if (ADD_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSAdd[i].Busy = 'Y';
							RSAdd[i].Op = instBuffer[instBufH].inst.type;
							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSAdd[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSAdd[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSAdd[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;
							if (instBuffer[instBufH].inst.type != "ADDI")
							{
								if (Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB == -1)
								{
									RSAdd[i].Vk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].Val;
									RSAdd[i].Qk = -1;
								}
								else  // Instruction is waiting for the ROB value
									RSAdd[i].Qk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB;
							}
							else
							{
								RSAdd[i].Vk = atoi(instBuffer[instBufH].inst.op2.c_str());
								RSAdd[i].Qk = -1;
							}
							RSAdd[i].Dest = ROBt;
							Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB = ROBt;
							ROB[ROBt].Dest = int(instBuffer[instBufH].inst.rd.at(1) - int('0')); //write the destination in the ROB
							RSAdd[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "NAND")
				{
					int i = 0;
					while (RSNand[i].Busy != 'N' && i < NAND_R) i++;  // Check if the RS is Free

					if (NAND_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSNand[i].Busy = 'Y';
							RSNand[i].Op = instBuffer[instBufH].inst.type;
							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSNand[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSNand[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSNand[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							if (Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB == -1)
							{
								RSNand[i].Vk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].Val;
								RSNand[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSNand[i].Qk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB;

							RSNand[i].Dest = ROBt;
							Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB = ROBt;
							ROB[ROBt].Dest = int(instBuffer[instBufH].inst.rd.at(1) - int('0')); //write the destination in the ROB
							RSNand[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "MUL")
				{
					int i = 0;
					while (RSMul[i].Busy != 'N' && i < MULT_R) i++;  // Check if the RS is Free

					if (MULT_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSMul[i].Busy = 'Y';
							RSMul[i].Op = instBuffer[instBufH].inst.type;
							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSMul[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSMul[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSMul[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							if (Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB == -1)
							{
								RSMul[i].Vk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].Val;
								RSMul[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSMul[i].Qk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB;

							RSMul[i].Dest = ROBt;
							Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB = ROBt;
							ROB[ROBt].Dest = int(instBuffer[instBufH].inst.rd.at(1) - int('0')); //write the destination in the ROB
							RSMul[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "BEQ")
				{
					int i = 0;
					while (RSBeq[i].Busy != 'N' && i < BEQ_R) i++;  // Check if the RS is Free

					if (BEQ_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSBeq[i].Busy = 'Y';
							RSBeq[i].Op = instBuffer[instBufH].inst.type;
							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSBeq[i].Vj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSBeq[i].Qj = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSBeq[i].Qj = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							if (Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB == -1)
							{
								RSBeq[i].Vk = Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].Val;
								RSBeq[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSBeq[i].Qk = Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB;

							if (atoi(instBuffer[instBufH].inst.op2.c_str()) > 0)
								ROB[ROBt].Dest = instBuffer[instBufH].instNum + 1; //T=1, NT=0;
							else
								ROB[ROBt].Dest = instBuffer[instBufH].instNum + 1 + atoi(instBuffer[instBufH].inst.op2.c_str());
							RSBeq[i].A = atoi(instBuffer[instBufH].inst.op2.c_str()) + instBuffer[instBufH].instNum + 1;
							RSBeq[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
							RSBeq[i].Dest = ROBt;
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "JMP")
				{
					int i = 0;
					while (RSJmp[i].Busy != 'N' && i < JMP_R) i++;  // Check if the RS is Free

					if (JMP_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSJmp[i].Busy = 'Y';
							RSJmp[i].Op = instBuffer[instBufH].inst.type;
							//RSJmp[i].A = atoi(instBuffer[instBufH].inst.op2.c_str()); //jump unconditionally to the address in A
							RSJmp[i].Qj = -1;
							RSJmp[i].Qk = -1;
							RSJmp[i].Dest = ROBt;
							RSJmp[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "RET")
				{
					int i = 0;
					while (RSJmp[i].Busy != 'N' && i < JMP_R) i++;  // Check if the RS is Free

					if (JMP_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSJmp[i].Busy = 'Y';
							RSJmp[i].Op = instBuffer[instBufH].inst.type;

							if (Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB == -1)
							{
								RSJmp[i].Vk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].Val;
								RSJmp[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSJmp[i].Qk = Reg[int(instBuffer[instBufH].inst.op2.at(1) - int('0'))].ROB;
							RSJmp[i].Qj = -1;

							RSJmp[i].Dest = ROBt;
							RSJmp[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else if (instBuffer[instBufH].inst.type == "JALR")
				{
					int i = 0;
					while (RSJmp[i].Busy != 'N' && i < JMP_R) i++;  // Check if the RS is Free

					if (JMP_R == i) stall = 1;
					else
					{
						if (ROB[ROBt].Type == "FREE")  // Check if the ROB is Free 
						{
							RSJmp[i].Busy = 'Y';
							RSJmp[i].Op = instBuffer[instBufH].inst.type;

							if (Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB == -1)
							{
								RSJmp[i].Vk = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].Val;
								RSJmp[i].Qk = -1;
							}
							else  // Instruction is waiting for the ROB value
								RSJmp[i].Qk = Reg[int(instBuffer[instBufH].inst.op1.at(1) - int('0'))].ROB;

							RSJmp[i].Dest = ROBt;
							RSJmp[i].Vj = instCounter + 1; //returned address
							RSJmp[i].Qj = -1;

							Reg[int(instBuffer[instBufH].inst.rd.at(1) - int('0'))].ROB = ROBt;
							ROB[ROBt].Dest = int(instBuffer[instBufH].inst.rd.at(1) - int('0')); //write the destination in the ROB
							RSJmp[i].instNum = instBuffer[instBufH].instNum; //insert from the head of the instruction buffer to the tail of the ROB table
						}
						else stall = 1;
					}
				}
				else cout << "Issue Error" << endl;

				if (!stall)
				{
					// ROB Filling
					ROB[ROBt].instNum = instBuffer[instBufH].instNum; //for the timing table in Write
					ROB[ROBt].Ready = 'N';
					ROB[ROBt].Type = instBuffer[instBufH].inst.type;
					ROBt = (ROBt + 1) % ROB_entries;

					InstTable[instCounter].I = clk;      // Timing Table ISSUE

					instBuffer[instBufH].validBit = 0; //incrementing the instBuffer head and returning the valid bit back to 0
					instBufH = (instBufH + 1) % instBufferSize;
				}
			}
		}

		for (int q = 0; q < 2; q++)
		{
			// FETCHING
			if (instBuffer[instBufT].validBit == 0 && instCounter < IC)
			{
				instBuffer[instBufT].inst = InstList[instCounter];
				instBuffer[instBufT].validBit = 1;
				instBuffer[instBufT].instNum = instCounter;
				instBufT = (instBufT + 1) % instBufferSize;
				InstTable[instCounter].F = clk;

				//to handle control hazards
				if (InstList[instCounter].type == "JMP")
					instCounter = instCounter + 1 + atoi(InstList[instCounter].op2.c_str());
				else if ((InstList[instCounter].type == "RET" || InstList[instCounter].type == "JALR") && Reg[int(InstList[instCounter].op2.at(1) - int('0'))].ROB == -1)
					instCounter = Reg[int(InstList[instCounter].op2.at(1) - int('0'))].Val;
				else if ((InstList[instCounter].type == "RET" || InstList[instCounter].type == "JALR") && Reg[int(InstList[instCounter].op2.at(1) - int('0'))].ROB != -1)
					instCounter = instCounter;
				else if (InstList[instCounter].type == "BEQ" && atoi(InstList[instCounter].op2.c_str()) < 0)
					instCounter = instCounter + 1 + atoi(InstList[instCounter].op2.c_str());	// BEQ Prediction Taken as imm < 0
				else
					instCounter++;
			}
		}

		clk++;
	}

	cout << "The total execution time: " << clk << endl;
	cout << "IPC : " << IC/double(clk) << " instructions per cycle" << endl;
	if (totalPredictions!=0)
		cout << "Branch misprediction percentage : " << (misprediction/double(totalPredictions))*100  << " % " << endl;
	else
		cout << "Branch misprediction percentage : " << 0 << " % " << endl;
	system("pause");
	return 0;
}