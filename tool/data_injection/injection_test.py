import csv
import ctypes

import data

import matplotlib.pyplot as plt

csv_headers = \
    [
        "cangate2tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorqVD",
        "cangate2tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorq"
    ]

if __name__ == '__main__':
    # create simu 
    cangate_to_tsm = data.CangateToTsm()
    ctrlarb_to_tsm = data.CtrlArbToTsm()
    deciarb_to_tsm = data.DeciArbToTsm()
    diag_to_tsm = data.DiagToTsm()
    planlite_to_tsm = data.PlanliteToTsm()

    tsm_to_planlite = data.TsmToPlanlite()
    tsm_to_ctrlarb = data.TsmToCtrlArb()
    tsm_to_deciarb = data.TsmToDeciArb()
    tsm_to_diag = data.TsmToDiag()
    tsm_to_hmi = data.TsmToHmi()
    tsm_to_cangate = data.TsmToCangate()

    # get data from csv
    csv_file = "../../data/tsm.csv"
    so_path = "../../output/lib/libtsm.so"

    scalib = ctypes.cdll.LoadLibrary(so_path)

    fig,ax = plt.subplots()
    time = []
    lat_override = []
    hand_torque = []

    with open(csv_file) as f:
        f_csv = csv.DictReader(f)
        it_csv = iter(f_csv)
        i = 0
        while True:
            try:
                inject_data = next(it_csv)
                # print(inject_data)
                for cur_header in csv_headers:
                    header_list = cur_header.split('.')
                    if cur_header.startswith("cangate2tsm"):
                        cangate_to_tsm.set_attr(header_list[-1], \
                                                inject_data[cur_header])
                    # todo
                
                scalib.MRM_Swc_V_TSM.argtype = [ctypes.POINTER(data.CtrlArbToTsm), \
                                                ctypes.POINTER(data.DeciArbToTsm), \
                                                ctypes.POINTER(data.CangateToTsm), \
                                                ctypes.POINTER(data.DiagToTsm), \
                                                ctypes.POINTER(data.PlanliteToTsm), \
                                                ctypes.POINTER(data.TsmToPlanlite), \
                                                ctypes.POINTER(data.TsmToCtrlArb), \
                                                ctypes.POINTER(data.TsmToDeciArb), \
                                                ctypes.POINTER(data.TsmToDiag), \
                                                ctypes.POINTER(data.TsmToHmi), \
                                                ctypes.POINTER(data.TsmToCangate)]
                scalib.MRM_Swc_V_TSM.restype = ctypes.c_void_p

                scalib.MRM_Swc_V_TSM(ctypes.byref(ctrlarb_to_tsm), \
                                     ctypes.byref(deciarb_to_tsm), \
                                     ctypes.byref(cangate_to_tsm), \
                                     ctypes.byref(diag_to_tsm), \
                                     ctypes.byref(planlite_to_tsm), \
                                     ctypes.byref(tsm_to_planlite), \
                                     ctypes.byref(tsm_to_ctrlarb), \
                                     ctypes.byref(tsm_to_deciarb), \
                                     ctypes.byref(tsm_to_diag), \
                                     ctypes.byref(tsm_to_hmi), \
                                     ctypes.byref(tsm_to_cangate))
                
                time.append(i)
                lat_override.append(tsm_to_ctrlarb.lat_override_flag)
                hand_torque.append(cangate_to_tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorq)
                ax.cla()
                ax.plot(time, lat_override)
                ax.plot(time, hand_torque, color = 'red')
                plt.pause(0.2)
                i = i + 1
                
            except StopIteration:
                print("injection test done!")
                break
        plt.show()


    