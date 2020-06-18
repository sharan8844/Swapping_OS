/*
BY:HARSH SHARAN 

*PROGRAM IMPLEMENTING FIFO SWAPPING IN OS WITH FCFS PROCESS SCHEDULING (small scale)
*WE ASSUME SWAP TIME IS VERY SMALL COMPARED TO 1 sec
*/
#include<bits/stdc++.h>
using namespace std;

//defining structure for process 
/*process ID is generated randomly by genID() function 
 Process size ,bursttime ,arrival or intime & flag(if I/O is required or not) are given as input
*/
struct Process{
  string pid; 
  int size;  
  int burstTime; 
  int inTime; 
  bool flag;
} ;

deque<struct Process> q;//double ended queue is used to implement minimum queue so that process with min inTime are at q.front()
int RAM= 4096; //in MB --Size of the RAM is 4 GB or 4096 MB
struct Process memory[2][2];   //Data structure for RAM  

int swap_area = 8192; //in MB --Size of the swap partition is 8 GB or 8192 MB i.e. twice of RAM as per guidelines by CentOS for systems with RAM <= 4GB 
vector < struct Process > swap_partition; //Data structure for Backing store 
int clock_time=0; //For time during process execution

//funtion to store process from RAM into swap Space
void post_swap_process(Process p){ 
    swap_partition.push_back(p);                          
}

/* To add newly added processes in queue ,making sure processes that arrive earlier are at front (as FCFS is used)
*/
void queue_push( Process p){
    stack< struct Process > stk; //to store all processes popped out of the queue while positioning new process
    while (!q.empty() && q.back().inTime > p.inTime){
    struct Process temp= q.back();
    q.pop_back();
    stk.push(temp);        
    }
    q.push_back(p); //adding new process at correct position
    
    //adding back previously stored elements in queue
    while(!stk.empty()){
        struct Process temp=stk.top();
        stk.pop();
        q.push_back(temp);
    }
    
}

//function to remove elements from queue for execution 
struct Process queue_pop(){

    struct Process x= q.front();
     q.pop_front();
    return x;             
    
}

//to generate unique ID for each process 
string genID(){
    string idd;
    srand (time(NULL));
    int t=5;
    while(t--){
        idd += rand()%10+97;
              }
    idd +=64;
    cout<<"ID of process added: "<<idd<<endl;
    return idd;
}

//function to carry out prcoess execution
void execute(){
    
    cout<<"------|-----------------------------------------------"<<endl;
    cout<<"time: |     Execution.. "<<endl;
    cout<<"------|-----------------------------------------------"<<endl;
    
        while(!q.empty() || !swap_partition.empty() ){
            cout<<" "<<clock_time<<"    |";
            int init_time=clock_time;
            
            vector<struct Process> newproc; //stores processes with arrival time at or before the current time i.e for execution
            
            //processes in swap space are added to newproc
            if(!swap_partition.empty()){
                cout<<"       Swapped in processes with ID: ";
                for( Process p: swap_partition){
                    newproc.push_back(p);
                    cout<<p.pid<<" ";
                }  
                cout<<endl;              
            }
            swap_partition.clear(); //swap space is now empty
            swap_area=8192;            

            while( !q.empty() && q.front().inTime <= clock_time){
                newproc.push_back(queue_pop());
            }
                        
            auto iterr= newproc.begin();  //defining iterator to loop through processes  
                    
            while( iterr != newproc.end()){
                struct Process proc= *iterr; //current process
 
                if( RAM - proc.size >= 0){ //if RAM is not full
                    struct Process tempo=proc; //temporary variable to store current process

                    if(proc.flag == 1){ //if I/O operations are required

                        if(swap_area - proc.size >= 0){  //if swap is not full
                          swap_area -= proc.size;
                          cout<<"       swapped out for I/O process with ID : "<<proc.pid<<endl; 
                          proc.flag=0; // I/O will complete after process is swapped out
                          post_swap_process(proc); 
                        }
                        else{ //if swap is full 
                             
                          cout<<"       (err):swap full-I/O can't be completed!"<<endl;
                          q.push_front(tempo);
                       
                        }
                        
                    }
                    else{  //if no I/O operations needed , execute process
                        RAM-=proc.size;
                        cout<<"       executed process with ID:"<<proc.pid<<endl;
                        clock_time += proc.burstTime;
                    }
                    
                 iterr=newproc.erase(iterr);   //operated processes are removed from newproc
                }
                
                else iterr++;  //if current process needs for RAM than availabe , check other processes in newproc              
            }
            
         /*processes that need more than available RAM at current time arrive here:
         1.if swap has enough space process is swapped out
         2.if swap is also full , system crashes
         */
            for( Process proc: newproc){
                 if(swap_area - proc.size >= 0 && proc.size <= 4096){
                          swap_area -= proc.size;
                          cout<<"       swapped out (RAM full) process with ID : "<<proc.pid<<endl;  
                          proc.flag=0;
                          post_swap_process(proc);                    
                }
                else{
                    cout<<"       (err):OOPS the system crashed! for process_ID"<<proc.pid<<endl;
                    return ;
                }                                 
            }
            
            RAM=4096; // all processes are either swapped out or executed , so RAM is now empty i.e has 4096 MB available space 
            newproc.clear();     
        
        //if no process is executed manually increment time for further execution    
        if(clock_time == init_time)
        clock_time++;
       
        }        //!q.empty() loop ends here        
    
}     //execution ends here

/*main function starts here 
 - the program is menu driven as shown below
*/
int main(){
    while(1){
        cout<<"Enter choice:\n1.to add process\n2.to display swapping involved\n3.to exit "<<endl; //what to do?
        int i;cin>>i;
        switch(i){
            case 1: 
            //to enter process
            {
                cout<<"Enter process Size(in MB) , Burst time , time of arrival(in any order) & if I/O operations required? ( Y/N ) :"<<endl;
                int x,y,z;char c;
                cin>>x>>y>>z>>c;
                bool bb= (c =='Y')?1:0;
                queue_push( Process{ genID(),x,y,z,bb } );
                break;
            }
            case 2:
            //to execute program for processes added 
            {
                execute();
                cout<<"EXECUTION ENDED"<<endl;
                return 0;
                break;
            }
            case 3:
            {
            //to exit program
            return 0;break;
            }
        }//switch statement ends here
    
    }//while loop ends here

    return 0;
    
} 
//-----------------------------------------------------------------END------------------------------------------------------------------