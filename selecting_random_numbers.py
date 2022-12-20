#This program selects random numbers of the specified bit range provided from all possible combinations in bit range specified.
#And,Later for each combination, each input position is sampled for 1 and 0 while keeping other input pos as constants

import random
n_of_input_bits = 16
no_of_inputs_to_be_selected = 15
list_of_random_num = list(random.sample(range(2**n_of_input_bits), (2**no_of_inputs_to_be_selected)))
list_of_random_num.sort()
input_bits = [0]*(2**no_of_inputs_to_be_selected)
dict_nums = {}
select_index = 0
for i in list_of_random_num:
    key_ele = '{i:0>{n}b}'.format(i=i, n=n_of_input_bits)
    dict_nums[key_ele] = 0
print(len(dict_nums))
for key_ele in dict_nums.copy().keys():
    for pin_index in range(n_of_input_bits-1, -1, -1):
        temp_key = key_ele
        if key_ele[pin_index] == '0':
            temp_key = temp_key[:pin_index] + '1' + temp_key[pin_index+1:]
        else:
            temp_key = temp_key[:pin_index] + '0' + temp_key[pin_index+1:]
        dict_nums[temp_key] = 0
print(len(dict_nums))


