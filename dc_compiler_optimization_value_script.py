#    __author__ = "M.Satya Amarkant"
#    __version__ = "1.0"
#    __credits__ = ["M.SATYA AMARKANT"]
#    __license__ = "Private Domain :  Requires Approval for the use from M.SATYA AMARKANT"
#    __maintainer__ = "M.Satya Amarkant"
#    __contact__ = "amarkant23394@gmail.com"
#    __status__ = "Development"

sub_circuits_id_num = 16
pattern_file_path = "pattern_merge_graphs/best/normal/pattern_"+str(sub_circuits_id_num)+".v"
dc_pattern_file_path = "pattern_merge_graphs/best/dc/dc_pattern_"+str(sub_circuits_id_num)+".v"

with open(pattern_file_path,'r')as fp_file , open(dc_pattern_file_path,'r')as dc_fp_file:
    no_of_cells_in_uncompiled = 0
    no_of_assign_uncompiled = 0
    counting_start = False
    for line in fp_file:
        line = line.lstrip()
        if counting_start == False:
            if line.startswith('module'):
                counting_start = True
        if counting_start == True:
            if line.startswith('endmodule'):
                counting_start = False
        if counting_start == True and line != "":
            if (line.startswith('assign')):
                no_of_assign_uncompiled += 1
            else:
                no_of_cells_in_uncompiled += 1

    no_of_cells_in_uncompiled -= 4
    print("No. of cells originally = "+str(no_of_cells_in_uncompiled))
    print("No. of assigns originally = "+str(no_of_assign_uncompiled))

    no_of_cells_in_compiled = 0
    no_of_assign = 0
    counting_start = False
    wire_started = False
    valid_file = False
    gate_count_start_flag = False

    for line in dc_fp_file:
#        print("line=",line)
        line = line.lstrip(" ")
        if valid_file == False:
            if line.startswith('module'):
                valid_file = True

        if valid_file == True:
            if line.startswith('endmodule'):
                valid_file = False

        if valid_file == True:
            if line.startswith('wire'):
                wire_started = True

        if valid_file == True and wire_started == True:
#            print("line=",line,"end")
            if line == "\n":
                gate_count_start_flag = True
                wire_started = False
            if (line.startswith('assign')):
                no_of_assign += 1

        if valid_file == True and gate_count_start_flag == True and line != "":
            if (line.startswith('assign')):
                no_of_assign += 1
            else:
                if(line.endswith(");\n")):
                    no_of_cells_in_compiled += 1


    if no_of_cells_in_compiled < 0:
        no_of_cells_in_compiled = 0

    print("Optimized to No. of cells  = "+str(no_of_cells_in_compiled))
    print("Optimized to No. of assigns = "+str(no_of_assign))

    cur_optimized_factor = ((no_of_cells_in_uncompiled - no_of_cells_in_compiled)/no_of_cells_in_uncompiled)*100
    print(cur_optimized_factor)
