#include <stdio.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>

using namespace std;

#include "cbp3_def.h"
#include "cbp3_framework.h"

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

// Variable state used for TWO_BIT_PREDICTOR
char state_2b[0xffff] = {1};

// -------- GSELECT STUFF ----------//
// Planning to use 4 bits of history for the XOR
int history = 0; // Initialized to none taken

// GSHARE Prediction Table
// 4 bits of history XOR with 8 bits of branch address
// 0xff, or 255 table entries
char state_gselect[0xff] = {1}; // Begin at state 1

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
	    int index;
	    index = uop->pc & 0x0000ffff;
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

	    // Collect lower byte of branch address
	    uint8_t addr = uop->pc & 0xff;
	    // XOR with lower nibble of history
	    int index_gselect = addr ^ (history & 0xf);
	    // Read from table at calculated index and set prediction 
	    if (state_gselect[index_gselect] < 2) 
		   gpred = false;
	    else
		   gpred = true; 

            assert(report_pred(fe_ptr, false, gpred));

        } else if (runs == GSHARE_PREDICTOR_) {
            // -- PLACE YOUR GSHARE PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)

            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = true; 

            assert(report_pred(fe_ptr, false, gpred));

        }

        // -- UPDATE THE `brh_fetch` branch history register here. See "hints" in
        // the assignment description for more information on this.
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
	    
	    // Collect lower byte of branch address
	    uint8_t addr = uop->pc & 0xff;
	    // XOR with lower nibble of history
	    int index_gselect = addr ^ (history & 0xf);
	    // Collect branch result
	    bool res_gselect = uop->br_taken;
	    // Update history
	    history = (history << 1) | res_gselect;
	    // Update prediction table
	    if (res_gselect) {
		if (state_gselect[index_gselect] < 3) state_gselect[index_gselect]++;
	    }
	    else {
	    	if (state_gselect[index_gselect] > 0) state_gselect[index_gselect]--;
	    }
 
        } else if (runs == GSHARE_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE GSHARE HERE
        }

        // -- UPDATE THE `brh_retire` branch history register here. See "hints" in
        // the assignment description for more information on this.
    }
}

void PredictorRunEnd() {
    runs ++;
    if (runs < 3) // set rewind_marked to indicate that we want more runs
        rewind_marked = true;
}

void PredictorExit() {
}
