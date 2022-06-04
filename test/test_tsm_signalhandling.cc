#include <gtest/gtest.h>

#include "util/common.h"
#include "tsm_signalhandling.h"

class tsm_signalhandling_test : public ::testing::Test {
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

    protected:
        struct tsm_entry* p_entry;
        struct tsm_intermediate_sig* p_int_sig;
};

TEST_F(tsm_signalhandling_test, process_lng_override) {
    Dt_RECORD_VehicleSignal2TSM *p_veh_sig = 
        &p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    static const float32 K_GasPedalPosThresholdValue = 20;
    static const uint16 K_LngOverrideTakeOverTime_Cnt = 250U;
    static const uint16 K_LngOvrdNeedDetectTouchZoneTime_Cnt = 25U;
    static const uint16 K_GasPedalAppliedThresholdTime_Cnt = 10U;

    p_veh_sig->EMS_GasPedalActPstforMRRVD = 1;
    p_veh_sig->EMS_GasPedalActPstforMRR = K_GasPedalPosThresholdValue + 1.0;

    ASSERT_FALSE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                                BITNO_LNG_OVERRIDE_ST));
    
    enum tsm_ifc_mrm_func_st mrm_st = IFC_PASSIVE;
    // 执行了10个周期
    for (size_t i = 0; i < K_GasPedalAppliedThresholdTime_Cnt + 2; ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_LNG_OVERRIDE_ST));

}

int
main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

