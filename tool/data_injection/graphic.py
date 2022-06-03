import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation

csv_file = "../../data/hand_torque.csv"

header_data = {"Time": [], "BCS_VehSpd": [], "EPS_StrngWhlTorq": []}
torque_threshold_table = [[0.0, 1.5], [30.0, 1.3], [60.0, 1.2], [90.0, 1.0]]

fig = plt.figure()
handtorq_axe = fig.add_subplot(111)
handtorq_axe.set(title = 'Eps StrngWhlTorq Change Figure', xlabel = \
    'time(s)', ylabel = 'hand torque')
ln, = handtorq_axe.plot([], [])

smoothed_torque = []
torque_threshold = []
torque_threshold_minus = []
dict_csv = []
header_detail = {}

x_data = []
y_data = []

def get_header_detail(data):
    header_detail = {}
    for data_ele in data:
        for header_key in header_data.keys():
            if header_key in data_ele:
                header_detail[header_key] = data_ele
    return header_detail

def smooth_hand_torq(cur_torque, last_torque):
    return float(float(0.7) * last_torque + float((1.0 - 0.7)) * cur_torque)

def set_header_data(data, header_detail):
    for header in header_data.keys():
        data_ele = data[header_detail[header]]
        if data_ele == '':
            if len(header_data[header]):
                data_ele = header_data[header][-1]   # 取上一把的值
            else:
                data_ele = 0.5
        
        if header == 'EPS_StrngWhlTorq':
            if len(header_data[header]):
                smooth_torq = \
                    smooth_hand_torq(float(data_ele), \
                                     header_data[header][-1])
                smoothed_torque.append(smooth_torq)
            else:
                smoothed_torque.append(float(data_ele))

        if header == "BCS_VehSpd":
            for i in range((len(torque_threshold_table) - 1), -1, -1):
                if float(data_ele) > torque_threshold_table[i][0]:
                    torque_threshold.append(torque_threshold_table[i][1])
                    torque_threshold_minus.append(-torque_threshold_table[i][1])
                    break
            # if (i == 1):
            #     print(i, float(data_ele))

        header_data[header].append(float(data_ele))

def init():
    handtorq_axe.set_xlim(0, 170)
    handtorq_axe.set_ylim(-0.9, 0.9)
    return ln,

def update_smoothed_hand_torq(frame):
    x_data.append(header_data['Time'][frame])
    y_data.append(smoothed_torque[frame])
    ln.set_data(x_data, y_data)
    return ln,

# def data_gen():
#     step = 0
#     while step < len(header_data['Time']):
#         yield step
#         step = step + 1

if __name__ == '__main__':

    with open(csv_file) as f:
        f_csv = csv.reader(f, delimiter=',')
        try:
            header_name = next(iter(f_csv))
            header_detail = get_header_detail(header_name)
        except StopIteration:
            print("no header_name")
    
    with open(csv_file) as f:
        dict_csv  = csv.DictReader(f)
        for data in dict_csv:
            set_header_data(data, header_detail)
    
    # anim = animation.FuncAnimation(fig, func = update_smoothed_hand_torq, \
    #                                frames = len(header_data['Time']), \
    #                                init_func = init, \
    #                                interval = 5, \
    #                                blit= False)
    # plt.show()
    
    
    # fig = plt.figure()

    # veh_spd_axe = fig.add_subplot(121)
    # veh_spd_axe.set(title = 'Vehicle Speed Change Figure', xlabel = 'time(s)', \
    #     ylabel = 'veh_spd(kph)')
    # veh_spd_axe.plot(header_data['Time'], header_data['BCS_VehSpd'])
    
    # handtorq_axe = fig.add_subplot(111)
    # handtorq_axe.set(title = 'Eps StrngWhlTorq Change Figure', xlabel = \
    #     'time(s)', ylabel = 'hand torque')
    # handtorq_axe.plot(header_data['Time'], header_data['EPS_StrngWhlTorq'])
    # handtorq_axe.plot(header_data['Time'], header_data['BCS_VehSpd'])
    handtorq_axe.plot(header_data['Time'], smoothed_torque)
    handtorq_axe.plot(header_data['Time'], torque_threshold, color = 'green')
    handtorq_axe.plot(header_data['Time'], torque_threshold_minus, color = 'green')
    plt.show()
    
    
            
            