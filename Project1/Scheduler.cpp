#include"Processor.h"
#include"SJFprocessor.h"
#include "RRprocessor.h"
#include "FCFSprocessor.h"
#include<cstdlib>
#include<time.h>
#include <fstream>
#include "defs.h"



int Scheduler::checkAvailability(ProcessorType type = ALL)
{
	int min = 10000000000000000;
	int minindex = -1;

	int startIndex = 0;
	int end = Processorsnum;

	if (type == FCFS)
	{
		startIndex = 0;
		end = fcfscount;
	}
	else if (type == SJF)
	{
		startIndex = fcfscount;
		end = fcfscount + sjfcount;
	}
	else if (type == RR)
	{
		startIndex = fcfscount + sjfcount;
		end = Processorsnum;
	}

	minindex = startIndex;

	for (int i = startIndex; i < end; i++)
	{
		int x = pros[i]->gettotalreq();
		if (x < min || (pros[i]->getrunning() == nullptr && pros[i]->getReadyNum() == 0) )
		{
			min = x;
			minindex = i;
		}
	}
	return minindex;
}


void Scheduler::Assign()
{
	Process*p;
	Process*q;

	while(!NEWlist.isEmpty() )
	{
		NEWlist.peek(p);

		if(p->getArrivalTime()==gettime())
		{
			NEWlist.dequeue(q);
			//VI

			int minindex = checkAvailability();
			if (minindex != -1 )
			{

				q->setfirsttimeCPU(gettime());
			}
			pros[minindex]->addprocess(q);	
		}
		else
		{
			break;
		}
	}
}

bool Scheduler::migrate(Process* p, ProcessorType Type)
{
	if (p->getForked() == true)
		return false;

	if (Type == RR)
	{
		if (p->getRemainingTime() < rtf)
		{
			int minindex = checkAvailability(SJF);
			pros[minindex]->addprocess(p);
			return true;
		}
	}
	else if (Type == FCFS)
	{
		if ( ((timestep - p->getArrivalTime()) - p->getexcuted()) > maxW )
		{
			int minindex = checkAvailability(RR);
			pros[minindex]->addprocess(p);
			return true;
		}
	}
	return false;
}

void Scheduler::workSteal()
{
	int shortestQueue = 0;
	int longestQueue = 0;

	for (int i = 1; i < Processorsnum; i++)
	{
		int requiredTime = pros[i]->gettotalreq();

		if (requiredTime < pros[shortestQueue]->gettotalreq())
		{
			shortestQueue = i;
		}

		if (requiredTime > pros[longestQueue]->gettotalreq())
		{
			longestQueue = i;
		}
	}

	if (shortestQueue == longestQueue || pros[longestQueue]->gettotalreq() == 0)
		return;

	while (
		(pros[longestQueue]->gettotalreq() - pros[shortestQueue]->gettotalreq()) /
		pros[longestQueue]->gettotalreq() <= .4
		)
	{
		Process* p;
		pros[longestQueue]->peek(p);

		if (p && p->getForked())
		{
			pros[longestQueue]->storeForked(p);
			bool onlyForked = false;

			if (pros[longestQueue]->gettotalreq() == 0)
				onlyForked = true;

			if (onlyForked == true)
			{
				pros[longestQueue]->restoreForked();
				break;
			}
			else
			{
				pros[longestQueue]->dequeueprocess();
				pros[shortestQueue]->addprocess(p);

				pros[longestQueue]->restoreForked();
			}

		}
		else
		{
			pros[longestQueue]->dequeueprocess();
			pros[shortestQueue]->addprocess(p);
		}

		p = nullptr;
	}

}


void Scheduler::shedule()
{
	// wrong what if 
	for(int i=0;i<Processorsnum;i++)
	{
		pros[i]->ScheduleAlgo();
	}
}


Scheduler::Scheduler()
{
	timestep=1;
	BLKcount=0;
	TRMcount=0;
}


void Scheduler::printpros()
{
	if (Window.getMode() != SILENT_MODE)
	{
		Window.print("Current timestep:");
		Window.print(timestep);
		Window.print("\n");

		Window.print("-----------------     RDY Processes     -----------------");
		Window.print("\n");

		int runCount = 0;

		for (int i = 0; i < Processorsnum; i++)
		{
			Window.print("processor "); Window.print(i + 1);
			Window.print(" ["); Window.print(pros[i]->getType()); Window.print("] : ");

			Window.print(pros[i]->getReadyNum());


			Window.print(" ");

			if (pros[i]->getReadyNum() != 0)
			{
				Window.print("process: ");
				pros[i]->printRdyList();
			}

			if (pros[i]->getrunning() != nullptr)
			{
				runCount++;
			}
			cout << endl;
		}
		// Print processors: ID - Scheduling algorithm - numOfProcesses - processesID
		Window.print("-----------------     BLK Processes     -----------------");
		Window.print("\n");
		Window.print(BLKcount);
		Window.print(" BLK: "); // numOfProcesses - processesID
		BLKlist.print();
		Window.print("\n");

		Window.print("-----------------     RUN Processes     -----------------");
		Window.print("\n");

		Window.print(runCount);
		Window.print(" RUN: ");
		for (int i = 0; i < Processorsnum; i++)
		{
			if (pros[i]->getrunning() != nullptr)
			{
				Window.print(pros[i]->getrunning()->getID());
				Window.print("(P"); Window.print(i + 1); Window.print(") ");
			}
		}
		Window.print("\n");

		Window.print("-----------------     TRM Processes     -----------------");
		Window.print("\n");
		Window.print(TRMcount);
		Window.print(" TRM: "); // numOfProcesses - processesID

		TRMlist.print();
		Window.print("\n");
		Window.printMode();
	}
	else if (TRMcount == processnum)
	{
		Window.printMode();
	}

}

void Scheduler:: addtoBLK(Process*p)
{
	if(p!=nullptr)
	{
	BLKcount++;
	
	BLKlist.enqueue(p);
	}
}
	void Scheduler::addtoTRM(Process*p)
	{
		if(p!=nullptr)
		{
	TRMcount++;
	TRMlist.enqueue(p);
		}
	}

	void Scheduler::Load()
	{
		ifstream fileinput("newtesting.txt");

		if (fileinput.is_open())
		{
			fileinput >> fcfscount;
			fileinput >> sjfcount;
			fileinput >> rrcount;
			Processorsnum = fcfscount + sjfcount + rrcount;
			fileinput >> timeslice >> rtf >> maxW >> stl;
			int forkprobperc;
			fileinput >> forkprobperc;
			//forkprob = forkprobperc / 100.0;
			fileinput >> processnum;

			for (int i = 1; i <= processnum; i++)
			{
				int AT, ID, CT, N, first, second;
				fileinput >> AT>> ID>> CT>> N ;
				
				Process* a=new Process(ID, AT, CT, N);

				for (int i = 0; i < N; i++)
				{
					if (i == 0)
					{
						fileinput.ignore();
					}
					fileinput.ignore();
					fileinput >> first;
					fileinput.ignore();
					fileinput >> second;
					fileinput.ignore();
					
					a->setPair(first, second);
				}
				NEWlist.enqueue(a);
				

			}
			pros = new Processor * [Processorsnum];
			if (fcfscount)
			{
				for (int i = 0; i < fcfscount; i++)
				{
					pros[i] = new FCFSprocessor(this);


				}
			}
			if (sjfcount)
			{
				for (int i = fcfscount; i < fcfscount + sjfcount; i++)
				{
					pros[i] = new SJFprocessor(this);


				}
			}
			if (rrcount)
			{
				for (int i = fcfscount + sjfcount; i < Processorsnum; i++)
				{
					pros[i] = new RRprocessor(this);


				}
			}
			int killpaira, killpairb;

			while (fileinput >> killpaira)
			{
				fileinput.ignore();
				fileinput >> killpairb;
				Pairs newsig (killpaira, killpairb);
				Sigkilllist.enqueue(newsig);

			}
			
			fileinput.close();

		}

		else
		{
			cout << "error could not open the file";
		}

	}


	ostream& operator<<(ostream& s,Process* p)
{

	s<<p->getID()<<"";
	return s;
}

	
	//Start of Forking Functions:
	bool Scheduler::CanForkChild(Process* Process)
	{
		bool canFork;
		//test for running,forked before or not,FCFS or not
		if (Process->getProcessState() == RUN )
			canFork = true;
		if (Process->getForkedBefore() == true)
			canFork = false;
		return canFork;
	}
	bool Scheduler::TestingProbability(double Probability)
	{
		double randNum = ((rand() % 100)+1);
		if (randNum <= Probability)
			return true;
		else
			return false;
    }

	void Scheduler::forkChild(Process* process)
	{
		processnum++;
		int ID = processnum;
		int AT = timestep;
		//remaing cpu time of parent done
		int CT = process->getRemainingTime();
		//add to children list
		Process* child = new Process(ID, AT, CT);
		//add to shortest list
		process->setForkedBefore();
	}

	/*Processor* Scheduler::findShortestRdyList()
	{
		int shortestTime = 999999999;
		Processor* shortestProcessor;
		for (int i = 0; i < Processorsnum; i++)
		{
			if (pros[i]->getType() == "FCFS")
			{
				if (arr[i]->sumCpu() < shortesttime)
					shortesttime = arr[i]->sumCpu();
				shoertestprocessor = arr[i];
			}
		}
		return shoertestprocessor;
	}*/

	// end of Forking Functions
	// Start of Kill signal and kill orphans Functions
	void Scheduler:: KillSignal()
	{
		Pairs Killsignal;
		Pairs deleted;
		Sigkilllist.peek(Killsignal);
		int KillTime = Killsignal.getfirst();
		int PID = Killsignal.getsecond();
		if (timestep == KillTime)
		{
			bool found;
			for (int i = 0; i < Processorsnum; i++)
			{
				if (pros[i]->getType() == "FCFS")
				{
					Process* Processptr;
					pros[i]->removebyid(PID, Processptr);
					addtoTRM(Processptr);
				}
			}
			if(found)
			    Sigkilllist.dequeue(deleted);
		}

	}
	// end of Kill signal and kill orphans Functions
	///////Start of io handling
	void Scheduler::IOHandling(Process* &run, int neededio)
	{
		run->incrementIOD(neededio);
		int x = neededio + gettime();
		run->setIOneeded(x);
		addtoBLK(run);
		run = nullptr;
	}


	void Scheduler::CheckBlock()
	{
		Process* blkpro;
		
		BLKlist.peek(blkpro);

			if (!BLKlist.isEmpty())
			{
				int iodone = blkpro->getIOneeded();
				if (iodone <= timestep)
				{
					BLKlist.dequeue(blkpro);
					Process* newblk;
					BLKlist.peek(newblk);
					if (!newblk)
					{
						newblk->setIOneeded(newblk->getIOneeded() + timestep);
					}
					Assign(blkpro);
					BLKcount--;
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		
	}

	//Completion//
	void Scheduler::Completed(Process*&run)
	{

		run->setTermination(gettime());
		addtoTRM(run);
		run = nullptr;

	}


	void Scheduler::Assign(Process * p)
	{
	
	
			int minindex = checkAvailability();
			pros[minindex]->addprocess(p);
		
	}

	void Scheduler::OutputFile()
	{

		ofstream Outputfile("OutPutFile.txt");
		Process* p;
		int currentWT, currentRT, currentTRT;
		int totalWT = 0, totalRT = 0;
		int totalTRT =0;
		int forkperc =0, killperc =0;
		int processorload, processorutilization;
		int avgUtilization;

		Outputfile << "TT" << "\t" << "PID" << "\t" << "AT" << "\t" << "CT" << "\t" << "IO_D" << "\t" << "WT" << "\t" << "RT" << "\t" << "TRT" << endl;

		while (TRMlist.dequeue(p))
		{
			Outputfile << p->getTermination() << "\t";
			Outputfile << p->getID() << "\t";
			Outputfile << p->getArrivalTime() << "\t";
			Outputfile << p->getCpuTime() << "\t";
			Outputfile << p->getTotalIOD() << "\t";
			currentWT = (p->getTermination() - p->getArrivalTime()) - p->getCpuTime();
			Outputfile << currentWT << "\t";
			totalWT += currentWT;
			currentRT = p->getArrivalTime() - p->getfirsttimeCPU();
			Outputfile << currentRT << "\t";
			totalRT += currentRT;
			currentTRT = p->getTermination() - p->getArrivalTime();
			Outputfile << currentTRT << endl;
			totalTRT += currentTRT;

		}
		int totalprocesses = getprocessnum();
		Outputfile << "\n" << "Processes: " << totalprocesses << endl;
		Outputfile << "avg WT = " << totalWT / totalprocesses << ",\t";
		Outputfile << "avg RT = " << totalRT / totalprocesses << ",\t";
		Outputfile << "avg TRT = " << totalTRT / totalprocesses << endl;
		Outputfile << "Migration %:\t RTF= " << MigRTF / totalprocesses << "%,\t MaxW = " << MigMaxW / totalprocesses << "%" << endl;
		Outputfile << "Work Steal %:" << workstealperc / totalprocesses << "%" << endl;
		Outputfile << "Forked Process: " << forkperc / totalprocesses << "%" << endl;
		Outputfile << "Killed Process: " << killperc / totalprocesses << "%" << endl << endl;
		int totalprocessors = fcfscount + sjfcount + rrcount;
		Outputfile << "Processors: " << totalprocessors << " [" << fcfscount << " FCFS, " << sjfcount << " SJF, " << rrcount << " RR]" << endl;
		Outputfile << "Processors Load" << endl;
		for (int i = 1; i <= totalprocessors; i++)
		{
			Outputfile << "p" << i  << "=" << (pros[i-1]->getbusy()) / (float)totalTRT << "%,\t";
			
		}
		Outputfile << endl << endl;
		Outputfile << "Processors Utiliz" << endl;
		float totalut = 0;
		float currut = 0;
		for (int i = 1; i <= totalprocessors; i++)
		{
			if (pros[i - 1]->getidle() != 0)
			{
				currut = ((float)(pros[i - 1]->getbusy()) / ((pros[i - 1]->getbusy()) + (pros[i - 1]->getidle())));
				
				totalut += currut;
			}
			Outputfile << "p" << i << "=" << currut << "%,\t";
	
		}
		Outputfile << endl;
		Outputfile << "Avg utilization = " << totalut/totalprocessors << "%";

		Outputfile.close();
	}
