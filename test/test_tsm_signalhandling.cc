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
        enum tsm_ifc_mrm_func_st mrm_st;
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
    ASSERT_EQ(p_int_sig->lng_ovrd_du_type, NO_LNG_OVERRIDE);
    
    mrm_st = IFC_TOR_LNG_LAT_CTRL;
    // 执行了10个周期
    for (size_t i = 0; i < (K_GasPedalAppliedThresholdTime_Cnt + 2); ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_LNG_OVERRIDE_ST));

    ASSERT_EQ(p_int_sig->lng_ovrd_du_type, INSTANT_LNG_OVERRIDE);
    for (size_t i = 0; i < (K_LngOvrdNeedDetectTouchZoneTime_Cnt + 2); ++ i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_EQ(p_int_sig->lng_ovrd_du_type, SHORT_TIME_LNG_OVERRIDE);

    for (size_t i = 0; i < (K_LngOverrideTakeOverTime_Cnt - 
                            K_LngOvrdNeedDetectTouchZoneTime_Cnt) + 2; ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }

    ASSERT_EQ(p_int_sig->lng_ovrd_du_type, LONG_TIME_LNG_OVERRIDE);

    mrm_st = IFC_PASSIVE;
    tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    ASSERT_EQ(p_int_sig->lng_ovrd_du_type, NO_LNG_OVERRIDE);
}

TEST_F(tsm_signalhandling_test, process_brake_behavior) {
    Dt_RECORD_VehicleSignal2TSM *p_veh_sig = 
        &p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    p_veh_sig->EBB_BrkPedalAppliedSt = 1;
    p_veh_sig->EBB_BrkPedalApplied = 1;
    static const uint16 K_BrakPedalAppliedThresholdTime_Cnt = 10;
    static const uint16 K_BrakeTOR_TimeThreshold_Cnt = 25;

    mrm_st = IFC_TOR_LNG_LAT_CTRL;

    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_SET_BRAKE));
    ASSERT_EQ(p_int_sig->brk_du_type, NO_BRAKE_INTERVENTION);
    for (size_t i = 0; i < (K_BrakPedalAppliedThresholdTime_Cnt + 2); ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }

    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_SET_BRAKE));
    ASSERT_EQ(p_int_sig->brk_du_type, SHORT_INTERVENTION);
    for (size_t i = 0; i < (K_BrakeTOR_TimeThreshold_Cnt + 2); ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_EQ(p_int_sig->brk_du_type, LONG_INTERVENTION);

    mrm_st = IFC_PASSIVE;
    tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    ASSERT_EQ(p_int_sig->brk_du_type, NO_BRAKE_INTERVENTION);
}

TEST_F(tsm_signalhandling_test, process_lat_override) {
    Dt_RECORD_VehicleSignal2TSM *p_veh_sig = 
        &p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    p_veh_sig->EPS_StrngWhlTorqVD = 1;
    p_veh_sig->EPS_StrngWhlTorq = 1.4;
    p_veh_sig->BCS_VehSpd = 15.0;

    mrm_st = IFC_TOR_LNG_LAT_CTRL;

    static const uint16 K_OverrideHandTorqCheckTime_Cnt = 3;

    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                                BITNO_DRVR_HANDTORQUE_OVERRIDE_ST));
    for (size_t i = 0; i < (K_OverrideHandTorqCheckTime_Cnt + 2); ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_DRVR_HANDTORQUE_OVERRIDE_ST));
    p_veh_sig->BCS_VehSpd = 31.0;
    for (size_t i = 0; i < (K_OverrideHandTorqCheckTime_Cnt + 2); ++i) {
        tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    }
    ASSERT_TRUE(tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_DRVR_HANDTORQUE_OVERRIDE_ST));
    
    mrm_st = IFC_PASSIVE;
    tsm_preprocess_input(p_int_sig, p_entry, mrm_st);
    ASSERT_TRUE(!tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                                BITNO_DRVR_HANDTORQUE_OVERRIDE_ST));
}

int
main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

