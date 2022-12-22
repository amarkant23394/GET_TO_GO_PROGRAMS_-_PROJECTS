#open the csv file in binary; read the data and give the data to this function to get a dictionary
def from_csv_to_dict(file_data_bytes):
    dict_ret = {}
    file_data_str = file_data_bytes.decode().split()
    column_names = file_data_str[0].split(",")
    csv_data_table = [x.split(",") for x in file_data_str[1:]]
    for col_index, column_name in enumerate(column_names):
        dict_ret[column_name] = []
        for row_index in range(len(csv_data_table)):
            dict_ret[column_name].append(csv_data_table[row_index][col_index])
    return dict_ret
