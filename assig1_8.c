#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf(1, "Need type and input filename\n");
        exit();
    }

    char *filename;
    filename = argv[2];
    int type = atoi(argv[1]);
    printf(1, "Type is %d and filename is %s\n", type, filename);

    int tot_sum = 0;

    // Read elements from the input file and store them in segment_arr
    int size=1000;
    short arr[size];
    char c;
    int fd = open(filename, 0);
    	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);

    // My code from here
    int noOfProcesses = 8;
    int pidParent = getpid();
    int cid;
    void *partialSumP = (void *)malloc(MSGSIZE);
    void *pmsg = (void *)malloc(MSGSIZE);
    int partialSum = 0;

    int elem_per_proc = size / noOfProcesses;
    int *segment_arr[noOfProcesses];

    // Allocate memory for each individual segment and store their addresses in segment_arr
    for (int i = 0; i < noOfProcesses; i++) {
        segment_arr[i] = malloc(elem_per_proc * sizeof(int));
    }

    for (int i = 0; i < noOfProcesses; i++) {
        for (int j = 0; j < elem_per_proc; j++) {
            segment_arr[i][j] = arr[i*elem_per_proc + j];
            //printf(1, "%d-%d \n",j, arr[i*elem_per_proc + j]);
            //printf(1, "%d-%d \n",j, segment_arr[i][j]);
        }
        //printf(1, "\n \n");
    }
    //printf(1, "%d \n", segment_arr[0][0]);

    if(type == 0){
		for(int i=0; i<noOfProcesses; i++){
			cid = fork();
			if(cid == 0){
				partialSum=0;
				for(int j=0; j<elem_per_proc; j++){
					partialSum += (short)segment_arr[i][j];
				}
				//printf(1, "partial sum = %d ", partialSum);
				//printf(1, "%d %d \n", curStart, curEnd);
				//printf(1, "%d \n",getpid());
				pmsg = (void *)(&partialSum);
				send(getpid(), pidParent, (void *)pmsg);
				exit();
			}
			else{
				wait();
				//printf(1, "%d ",getpid());
				recv((void *)partialSumP);
				//printf(1, "%d \n", *(short *)(partialSumP));
				tot_sum+=*(short *)partialSumP;
			}
			
		}
	} 

    if (type == 0) {
        printf(1, "Sum of array for file %s is %d\n", filename, tot_sum);
    }
    
    exit();
    
}