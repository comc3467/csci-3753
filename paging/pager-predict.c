 /* File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"



void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int last_page[MAXPROCESSES];
    static int left_this_page[MAXPROCPAGES]; //keeps count of how many times we've left this page
    static int left_to_which_page[MAXPROCPAGES][MAXPROCPAGES];//keeps track of the flow, how many times it goes from one page to another

    
    /* Local vars */
    int proc;
    int pc;
    int page;
    


    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
        for(int i = 0; i < MAXPROCPAGES; i++){
            last_page[i] = -1;
            left_this_page[i] = 0; //makes sure no page seems to have been visited
            for(int j = 0; j < MAXPROCPAGES; j++){
                left_to_which_page[i][j] = 0;
            }
        }
	initialized = 1;
    }
    for(proc=0; proc<MAXPROCESSES; proc++) { 
	/* Is process active? */
        if(q[proc].active) {
            /* Dedicate all work to first active process*/ 
            pc = q[proc].pc; 		        // program counter for process
            page = pc/PAGESIZE; 		// page the program counter needs
            //timestamps[proc][page] = tick; //update the timestamp so we know we've just used it
            /* if(page_flow[proc][page] == -1){
                future_page = page+1; //just gives us the next page cause it might be linear or just needing the next one usually
            } */

            if(last_page[proc] != -1 && last_page[proc] != page){ //meaning we went somewhere last time
                left_to_which_page[last_page[proc]][page] += 1; //keeps count of probability to use later
                left_this_page[last_page[proc]] += 1; //keeps total count leaving that page, so we can calc probabilities
                //last_page[proc] = page; //put the current page in
            }
            last_page[proc] = page;
            int needed[MAXPROCPAGES];
            for(int i=0;i<MAXPROCPAGES;i++){
                needed[i] = 0;
            }
            needed[page] = 1; //since we need the page we're currently on (duh)
            
            if(left_this_page[page]){ //means we've left this page before, so we have something to predict
                for(int i = 0; i < MAXPROCPAGES; i++){
                    if((double) left_to_which_page[page][i]/left_this_page[page] > 0){
                        needed[i] = 1; //need this because there's a chance the next page needs it
                    }
                    if((double) left_to_which_page[page][i]/left_this_page[page] > .7 && left_this_page[i]){ //meaning we've left the next page, and we predict this next to actually be the next (>.6)
                        for(int j = 0; j < MAXPROCPAGES; j++){
                            if((double) left_to_which_page[i][j]/left_this_page[i] > 0){ 
                                needed[j] = 1; //doesn't matter if we already said we needed it
                            }
                        }
                    }
                }
            }
            

            for(int i = 0; i < MAXPROCPAGES; i++){
                if(!needed[i]){
                    pageout(proc,i); //pageout all pages we don't need
                }
            }
            for(int i = 0; i < MAXPROCPAGES; i++){
                if(needed[i] && !q[proc].pages[i]){
                    pagein(proc,i); //pagein the pages we do need
                }
            }
            
        }
    }
}