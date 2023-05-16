#pragma once
#include"Process.h"
#include"Scheduler.h"
#include"defs.h"
#include"ProcessQueues.h"
#include"ProcessQueue.h"
class Processor{

protected:
int RunningTime;
Scheduler*psh;
ProcessorState State;
ProcessorType Type;
//

//

//ProcessQueue rdylist;

Process* Running;
int neededio;

public:

	int readynum;


Processor()
{
	
	RunningTime=0;
	Running=nullptr;
	State=IDLE;
}

void setstate(ProcessorState S)
{
	State=S;


}

int getReadyNum()
{
	return readynum;
}

Processor(Scheduler*sh)
{
	readynum=0;
	psh=sh;
	RunningTime=0;
	Running=nullptr;
	State=IDLE;
}
Process* setrunning()
{
	Process*q=Running;
	Running=nullptr;
	setstate(IDLE);
	return q;
	
}
Process* getrunning()
{
	return Running;

}
virtual void addprocess(Process* p)
{
	/*if(p!=nullptr)
	{
	rdylist.enqueue(p);

	readynum++;
	}*/
}
virtual Process* dequeueprocess(){return nullptr;};
virtual int gettotalreq()=0;

virtual void peek(Process*) = 0;

virtual void ScheduleAlgo()

{
	/*Process*p;
	if(State==IDLE)
	{
		if(!rdylist.isEmpty())
		{
			rdylist.peek(p);
			if(p!=nullptr)
			{
	rdylist.dequeue(Running);
	readynum--;
	State=BUSY;
			}
		}
	
	}
	*/



};
void setType(ProcessorType T)
{
	Type=T;
}

virtual void printRdyList()
{
	//rdylist.print();
}

virtual void storeForked(Process* p) = 0;

virtual void restoreForked() = 0;

char* getType() const
{
	if (Type == FCFS)
	{
		return "FCFS";
	}
	if (Type == SJF)
	{
		return "SJF";
	}
	return "RR";
}
//virtual bool FindProcessByPID(int id);

virtual bool removebyid(int id, Process*& p)=0;
};
