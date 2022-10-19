"""
@author: M.Satya Amarkant
Project Name : Benchmark Generator using Attributed Graph Grammar
Description : This program will extract all the possible subcircuits from the given input directory
and will store it in form json dictionary in the output directory.
"""

from numpy import equal
import verilog_parsing
import networkx as nx
import pathlib
import json
import itertools
import matplotlib.pyplot as plt
import sys
#Modules
#-----------------------------------------------------------------------------#

#-----------------------------------------------------------------------------#

#Source Code
#-----------------------------------------------------------------------------#
class Extractor:
    def __init__(self,input_directory,output_directory,**kwargs):
        self.input_directory = input_directory
        self.output_directory = output_directory
        self.single_input_gates = ["NOT","BUF","INV","CLKBUF","INPUT"]
        self.four_input_gates = ["DFF", "SDFF"]
        self.parser = extractor_test(self)
    
    def __call__(self):
        pass
    
#-----------------------------------------------------------------------------#
"""
This function is used to extract the subgraphs along with frequencies and will store the extracted edge data in a file in form of dictionaries.

"""
#-----------------------------------------------------------------------------#
def extractor_test(self):
    #Will loop through all files in the input directory
    sys.setrecursionlimit(5000)
    for file_path in pathlib.Path(self.input_directory).iterdir():
        if file_path.is_file():
            input_f_path_str = str(file_path)
            print("Reading file = ",file_path)
            output_filename = str(pathlib.Path(file_path).name)
            output_filename = str(self.output_directory)+"/" + output_filename[0:-2]
            i_graph_obj = verilog_parsing.ReadVerilog(input_f_path_str)     #parsing the verilog file of benchmark and convert it into graph object
            d_graph_obj = i_graph_obj.getGraph()
            #nx.draw(d_graph_obj,with_labels=True)
            #plt.show()
            graph_nodes = i_graph_obj.getNodes()
            d_sub_graph = []
            graph_node_attribues=nx.get_node_attributes(d_graph_obj,'type')
            gate_level_range = min(30,d_graph_obj.number_of_nodes())
            for k in range(6,gate_level_range):
                realsub=[]
                print(input_f_path_str," no. of nodes = ",k,"length of subs = ",len(realsub))
                for i in d_graph_obj.nodes(): # For each vertex , make a list of its neighbours
                    vext=[]
                    # print("Selected Node = ",i)
                    succ_nodes=d_graph_obj.successors(i)
                    prev_nodes=d_graph_obj.predecessors(i)
                    for j in succ_nodes:
                        vext.append(j)
                    for j in prev_nodes:
                        vext.insert(0,j)
                    mytmp=[]
                    for v in vext:
                        mytmp.append(v)
                    while len(mytmp)>0: # For each node in the neighbor list call extend_subgraph function
                        vext=[]
                        for v in mytmp:
                            vext.append(v)
                        extend_subgraph_suc([i],vext,k,d_graph_obj,realsub)
                        if mytmp:
                            mytmp.pop(0)

                temp_output_file_name = output_filename+"_"+str(k)+".json"
                outfile = open(temp_output_file_name, "w")
                for sb in realsub:
                    input_node_count = 0
                    output_node_count = 0
                    SG = d_graph_obj.subgraph(sb)
                    nodes_sub = SG.nodes
                    node_attribues=nx.get_node_attributes(SG,'type')
                    for node_element in nodes_sub:
                        node_list = list(nx.bfs_edges(SG,node_element,reverse=True))[:2]
                        input_check_matched=0
                        if(len(node_list)<4):
                            if node_attribues[node_element] in self.single_input_gates:
                                input_node_count += (1 - len(node_list)) if (1 - len(node_list)) > 0 else 0
                                input_check_matched=1
                            if input_check_matched == 0 and node_attribues[node_element] in self.four_input_gates:
                                input_node_count += (4 - len(node_list)) if (4 - len(node_list)) > 0 else 0
                                input_check_matched=1
                            if input_check_matched == 0:
                                input_node_count += (2 - len(node_list)) if (2 - len(node_list)) > 0 else 0

                        node_list = list(nx.bfs_edges(SG,node_element,reverse=False))[:2]
                        if(node_list == []):
                            output_node_count += 1
                    node_attribues=nx.get_node_attributes(SG,'type')
                    grap_dict = nx.to_dict_of_dicts(SG)
                    list_edge = nx.to_edgelist(SG)
                    l_edge = list(list_edge)
                    dictionary ={
                        "NODES" : list(nodes_sub),
                        "SUB_GRAPH" : grap_dict,
                        "SUB_GRAPH_EDGE_LIST" : l_edge,
                        "NODE_ATTRIBUTES"   : node_attribues,
                        "INPUT_COUNT" :input_node_count,
                        "OUTPUT_COUNT" :output_node_count,
                        "NO_OF_NODES" : len(list(nodes_sub)),
                    }
                    json.dump(dictionary, outfile)
                    outfile.write('\n')
                outfile.close()

def extend_subgraph_suc(vsub,vext1,k,g,realsub):
    if len(vsub)==k-1:
        print(" no. of nodes = ",k,"length of subs = ",len(realsub))
        for i in vext1:
            lst=[]
            for j in vsub:
                lst.append(j)
            lst.append(i)
            realsub.append(lst)
            return
    
    stack_list = []
    stack_list.append(vsub)
    stack_list.append(vext1)
    while(len(vext1) != 0):
        ele=vext1.pop(0)
        if len(vsub)<k-1:
            for n in g.successors(ele):
                if n not in vsub:
                    if n not in vext1:
                        vext1.insert(0,n)
            for n in g.predecessors(ele):
                if n not in vsub:
                    if n not in vext1:
                        vext1.insert(0,n)
        vsub.append(ele)
        stack_list.append(vsub)
        stack_list.append(vext1)
        if len(vsub)==k-1:
            print(" no. of nodes = ",k,"length of subs = ",len(realsub))
            for i in vext1:
                lst=[]
                for j in vsub:
                    lst.append(j)
                lst.append(i)
                realsub.append(lst)
        vext1=stack_list.pop(-1)
        vsub=stack_list.pop(-1)

# #### Function to extend a subgraph
# def extend_subgraph_suc(vsub,vext1,k,g,realsub):
#     if len(vsub)==k-1:
#         print(" no. of nodes = ",k,"length of subs = ",len(realsub))
#         for i in vext1:
#             lst=[]
#             for j in vsub:
#                 lst.append(j)
#             lst.append(i)
#             realsub.append(lst)
#         return
#     while vext1:
#         ele=vext1.pop(0)
#         for n in g.successors(ele):
#             if n not in vsub:
#                 if n not in vext1:
#                     vext1.insert(0,n)
#         for n in g.predecessors(ele):
#             if n not in vsub:
#                 if n not in vext1:
#                     vext1.insert(0,n)
#         vsub.append(ele)
#         extend_subgraph_suc(vsub,vext1,k,g,realsub)

            # i=0
            # for nb_nodes in range(2,d_graph_obj.number_of_nodes()):
            #     for SG in (d_graph_obj.subgraph(selected_nodes) for selected_nodes in itertools.combinations(d_graph_obj, nb_nodes)):
            #         print("Serial Number : ",i," Number of nodes = ",nb_nodes)
            #         if nx.is_weakly_connected(SG):
            #             print("Serial Number : ",i," Number of nodes = ",nb_nodes," connected")
            #             outfile = open(output_filename, "a")
            #             input_node_count = 0
            #             output_node_count = 0
            #             nodes_sub = SG.nodes
            #             node_attribues=nx.get_node_attributes(SG,'type')
            #             for node_element in nodes_sub:
            #                 node_list = list(nx.bfs_edges(SG,node_element,reverse=True))[:2]
            #                 if((node_list == []) or ((len(node_list)==1) and (node_attribues[node_element] not in self.single_input_gates))):
            #                     if node_list == []:
            #                         input_node_count += 2
            #                     else:
            #                         input_node_count += 1
            #                 node_list = list(nx.bfs_edges(SG,node_element,reverse=False))[:2]
            #                 if(node_list == []):
            #                     output_node_count += 1
            #             node_attribues=nx.get_node_attributes(SG,'type')
            #             grap_dict = nx.to_dict_of_dicts(SG)
            #             list_edge = nx.to_edgelist(SG)
            #             l_edge = list(list_edge)
            #             dictionary ={
            #                 "Nodes : " : list(nodes_sub),
            #                 # "SUB_GRAPH" : grap_dict,
            #                 # "SUB_GRAPH_EDGE_LIST" : l_edge,
            #                 # "NODE_ATTRIBUTES"   : node_attribues,
            #                 # "INPUT_COUNT" :input_node_count,
            #                 # "OUTPUT_COUNT" :output_node_count,
            #             }
            #             json.dump(dictionary, outfile)
            #             outfile.write('\n')
            #             outfile.close()
            #         i+=1            
