#include <gtest/gtest.h>

#include "tsm_monitor.h"
#include "tsm_parameter.h"

#define INITILIZE_MEMORY(data) { \
    memset(data, 0, sizeof(*data)); \
}

// prehandling and posthandling for tsm_monitor_test test suits
class tsm_monitor_test : public ::testing::Test {
    protected:
        virtual void SetUp() {
            p_entry = new struct tsm_entry();
            p_entry->in_can_gate = new Dt_RECORD_CANGATE2TSM();
            p_int_sig = new struct tsm_intermediate_sig();
        }

        virtual void TearDown() {
            delete p_entry->in_can_gate;
            delete p_entry;
            delete p_int_sig;
        }
    
        struct tsm_entry* p_entry;
        struct tsm_intermediate_sig* p_int_sig;
};

TEST_F(tsm_monitor_test, monitor_nda_false_activate) {

    p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State = 
        NDA_STANDBY_NORMAL;
    p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt =
        VEH_STANDSTILL_ST_STANDSTILL;
    tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                             BITNO_NDA_TRANSIT_NORMAL_FLAG);
    tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                             BITNO_NDA_AVL_BEFORE_ACT);
    tsm_run_monitor(p_entry, p_int_sig);
    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                          BITNO_NDA_TRANSIT_NORMAL_FLAG));
    p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State = 
        NDA_ACTIVE_HANDS_ON_NORMAL;
    tsm_run_monitor(p_entry, p_int_sig);
    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                           BITNO_NDA_TRANSIT_NORMAL_FLAG));

    INITILIZE_MEMORY(p_entry->in_can_gate);
    INITILIZE_MEMORY(p_int_sig);

    p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State = 
        NDA_STANDBY_NORMAL;
    p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt =
        VEH_STANDSTILL_ST_NOT_STANDSTILL;
    tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                             BITNO_NDA_TRANSIT_NORMAL_FLAG);
    tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                             BITNO_NDA_AVL_BEFORE_ACT);
    tsm_run_monitor(p_entry, p_int_sig);
    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                          BITNO_NDA_TRANSIT_NORMAL_FLAG));
    p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State = 
        NDA_ACTIVE_HANDS_ON_STAND_ACTIVE;
    tsm_run_monitor(p_entry, p_int_sig);
    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                           BITNO_NDA_TRANSIT_NORMAL_FLAG));
}

int 
main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}