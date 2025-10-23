#include <stdio.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>

using namespace std;

#include "cbp3_def.h"
#include "cbp3_framework.h"
#include "predictor.h"

/*
# Seperate Predictor Types

In each function (mainly PredictorReset() and PredictorRunACycle()
there will be a if-else statement that switches between the different
types of predictor. Code specific to each type of predictor should
be implemented in the branch of the if-else statement corresponding
to the specific type of branch predictor. In general,

- the predictor will use saturating-counter in the first run,
- the gselect predictor will be used in the second run, and
- the gshare predictor will be used in the third run.
*/
#define TWO_BIT_PREDICTOR_ 0U
#define GSELECT_PREDICTOR_ 1U
#define GSHARE_PREDICTOR_  2U

/*
# Two branch history registers

the framework provids real branch results at fetch stage to simplify branch history
update for predicting later branches. however, they are not available until execution stage
in a real machine. therefore, you can only use them to update predictors at or after the
branch is executed.
*/

// cost: depending on predictor size
uint32_t brh_fetch;
uint32_t brh_retire;

// Count number of runs, this is used to switch between
// the different predictors.
uint32_t runs;

// This function runs ONCE when the simulation starts. Globals
// state should be initialized here, if there is any that is
// shared between predictors. (There probably will not
// be any modifications here)
void PredictorInit() {
    runs = 0;
    printf("G-Select table size\t %x\n", G_SHARE_TABLE_SIZE);
    printf("History bitmask\t\t %d\n", G_SHARE_HIS_BITS);
    printf("\n");
}

// This function is called before EVERY run
// It is used to reset predictors and change configurations
void PredictorReset() {
    
    // Predictor Specific Setup
    if (runs == TWO_BIT_PREDICTOR_)
        printf("Predictor: 2-bit saturating-counter\n");
    else if (runs == GSELECT_PREDICTOR_)
        printf("Predictor: gseletct\n");
    else if (runs == GSHARE_PREDICTOR_)
        printf("Predictor: gshare\n");

    // Branch History Register Resets
    brh_fetch = 0;
    brh_retire = 0;
}

// ------ 2-BIT SATURATION STUFF -------//
uint8_t state_2b[B2_TABLE_SIZE] = {1};

// -------- GSHARE STUFF ----------//

// GSHARE Prediction Table
char state_gshare[G_SHARE_TABLE_SIZE] = {1}; // Begin at state 1

void PredictorRunACycle() {
    // Stores info about what uops are being processed at each pipeline stage
    const cbp3_cycle_activity_t *cycle_info = get_cycle_info();

    /*
    This `for` loop handles all instructions that come in during the
    fetch stage of the processor. During the fetch stage the branch
    predictor makes a prediction (taken or not-taken), but doesn't
    actually know if the branch was taken or not, so it can't
    update itself with new information. In the loop below, add
    the logic for predicting whether or not the branch should be
    taken according to the current state of the predictor. 
     */
    for (int i = 0; i < cycle_info->num_fetch; i++) {
        uint32_t fe_ptr = cycle_info->fetch_q[i];
        const cbp3_uop_dynamic_t *uop = &fetch_entry(fe_ptr)->uop;

        if (!(uop->type & IS_BR_CONDITIONAL)) continue;
        if (runs == TWO_BIT_PREDICTOR_) {
            // -- PLACE YOUR TWO BIT SATURATING COUNTER PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)
	    // Set `gpred` based off whether or not a branch should be taken
	    bool gpred = true; 
	    int index = uop->pc & ((int) B2_TABLE_SIZE);
	    if (state_2b[index] < 2)
	    {
		gpred = false;
	    }
	    else
	    {
		gpred = true;
	    }

            assert(report_pred(fe_ptr, false, gpred));

        } else if (runs == GSELECT_PREDICTOR_) {
            // -- PLACE YOUR GSELECT PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)

            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = true;

            
	    assert(report_pred(fe_ptr, false, gpred));

        } else if (runs == GSHARE_PREDICTOR_) {
            // -- PLACE YOUR GSHARE PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)

            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = true; 

	    // Collect lower byte of branch address
	    int addr = uop->pc & ((1 << G_SHARE_ADDR_BITS) - 1);
	    int hist = brh_fetch & ((1 << G_SHARE_HIS_BITS) - 1);
	    // XOR with N bits of history
	    int index_gselect = addr ^ hist; 
	    // Read from table at calculated index and set prediction 
	    if (state_gshare[index_gselect] < 2) 
		   gpred = false;
	    else
		   gpred = true; 
	   
	    
            assert(report_pred(fe_ptr, false, gpred));

        }

        // -- UPDATE THE `brh_fetch` branch history register here. See "hints" in
        // the assignment description for more information on this.

	// Update brh_fetch GSelect
	brh_fetch = (brh_fetch << 1) | uop->br_taken;

    }

    /*
    This loop handles all instsructions during the retire stage of the
    pipeline. At this stage it is known if the branch was actually taken
    or not (accessible by uop->br_taken variable) so the state of each
    of the predictors can be updated to reflect the result of the branch
    instruction.
    */
    for (int i = 0; i < cycle_info->num_retire; i++) {
        uint32_t rob_ptr = cycle_info->retire_q[i];
        const cbp3_uop_dynamic_t *uop = &rob_entry(rob_ptr)->uop;

        if(!(uop->type & IS_BR_CONDITIONAL)) continue;

        if (runs == TWO_BIT_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE TWO BIT SATURATING COUNTER HERE
	    int index;
            index = uop->pc & 0x0000ffff;
	    if (uop->br_taken == 0)
	    {
		if (state_2b[index] != 0)
		{
		    state_2b[index]--;
		}
	    }
	    else
	    {
		if (state_2b[index] != 3)
		{
		    state_2b[index]++;
		}
	    }
        } else if (runs == GSELECT_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE GSELECT HERE
	    
	     	    
	    
        } else if (runs == GSHARE_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE GSHARE HERE
	    
	    // Collect lower byte of branch address
	    int addr = uop->pc & ((1 << G_SHARE_ADDR_BITS) - 1);
	    int hist = brh_retire & ((1 << G_SHARE_HIS_BITS) - 1);
	    // XOR with N bits of history
	    int index_gshare = addr ^ hist; 
	    // Collect branch result
	    bool res_gshare = uop->br_taken;
	    // Update prediction table
	    if (res_gshare) {
		if (state_gshare[index_gshare] < 3) state_gshare[index_gshare]++;
	    }
	    else {
	    	if (state_gshare[index_gshare] > 0) state_gshare[index_gshare]--;
	    }

        }

        // -- UPDATE THE `brh_retire` branch history register here. See "hints" in
        // the assignment description for more information on this.
	
	// Update brh_retire
	brh_retire = (brh_retire << 1) | uop->br_taken;

    }
}

void PredictorRunEnd() {
    runs ++;
    if (runs < 3) // set rewind_marked to indicate that we want more runs
        rewind_marked = true;
}

void PredictorExit() {
}
