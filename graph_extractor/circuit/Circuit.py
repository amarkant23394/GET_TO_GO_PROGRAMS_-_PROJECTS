# -*- coding: utf-8 -*-
"""
Created on Sat Feb 20 15:36:48 2021

@author: JuneethKumar Meka
"""

from circuit import Node
import sys
import networkx as nx

class Circuit:
    """Circuit Class"""
    __author__ = "Juneethkumar Meka"
    __version__ = "1.0"
    __credits__ = ["Juneethkumar Meka"]
    __license__ = "Private Domain :  Requires Approval for the use from Juneethkumar Meka"
    __maintainer__ = "Juneethkumar Meka"
    __contact__ = "juneethkumarmeka@gmail.com"
    __status__ = "Development"

    __all__ = ["addNode","updateNode","addEdge","removeEdge","removeNode","getNodes",
               "getInputs","getOutputs","getKeyinputs","getNodes","addDFF"
               "addMux","addAO","addOA","addOAI","addHalfAdder","addFullAdder",
               "getCircuitName"]
    
    def __init__(self,name):
        self.name = name
        self.nodes = {}
        self.inputs = []
        self.keyinputs = []
        self.outputs = []
        self.DFFs = []
        self.levels = {}
        self.graph = nx.DiGraph()
    
    def getGraph(self):
        return self.graph
            
    def getCircuitName(self):
        """getCircuitName Method: Gives the name of the circuit"""
        return self.name
    
    def addNode(self,nodeName,nodeType,*,iskeyinput = False,isinput = False,isoutput = False):
        """addNode Method : Adds node to the Circuit
            arguments : nodeName,nodeType
            optional arguments : isKeyinput,isinput,isoutput.
            The default value for all the optional arguments is False 
            Ex : addNode(N1,"AND")
            Ex : addNode(N1,"input",isInput= True, isKeyinput=True)
        """
        
        self.nodes[nodeName] = Node(nodeName,nodeType)
        self.graph.add_node(nodeName,type= nodeType,isInput= isinput,isOutput=isoutput,isKeyinput = iskeyinput)
        if nodeType == "DFF":
            self.DFFs.append(nodeName)
        if iskeyinput:
            self.nodes[nodeName].setNodeAsKeyinput()
            self.keyinputs.append(nodeName)
        if isinput:
            self.nodes[nodeName].setNodeAsInput()
            self.inputs.append(nodeName)
        if isoutput:
            self.nodes[nodeName].setNodeAsOutput()
            self.outputs.append(nodeName)    
        
        
    def updateNode(self,nodeName,**kwargs):
        """updateNode Method : Updates the node feilds such as nodeType,isinput
        isOuput,isKeyinput 
        arguments accepted : nodeType,isInput,isKeyinput,isOuput
        Ex : updatNode("n1", nodeType="AND", isInput=True, isKeyinput=True)
        """
        if "nodeType" in kwargs:
            nodeType = self.nodes[nodeName].getNodeType()
            if nodeType == "DFF":
                self.DFFs.remove(nodeName)
                
            self.nodes[nodeName].changeNodeType(kwargs["nodeType"])
            self.graph.nodes[nodeName]["type"] = kwargs["nodeType"]
            
            if kwargs["nodeType"] == "DFF":
                self.DFFs.append(nodeName)
                
        if "isInput" in kwargs:        
            if kwargs["isInput"] :
                if nodeName not in self.inputs:
                    self.inputs.append(nodeName)
                    self.nodes[nodeName].setNodeAsInput()
                    self.graph.nodes[nodeName]["isInput"] = True
                    
        if "isKeyinput" in kwargs:    
            if kwargs["isKeyinput"] :
                if nodeName not in self.keyinputs:
                    self.keyinputs.append(nodeName)
                    self.nodes[nodeName].setNodeAsKeyinput()
                    self.graph.nodes[nodeName]["isKeynput"] = True
                    
        if "isOutput" in kwargs:           
            if kwargs["isOutput"] :
                if nodeName not in self.outputs:
                    self.outputs.append(nodeName)
                    self.nodes[nodeName].setNodeAsOutput()
                    self.graph.nodes[nodeName]["isOutput"] = True
            
        
    def addEdge(self,source,destination):
        """addEdge Method : Adds the edge between the source and destinaton """        
        if source not in self.nodes:
            self.addNode(source,"Temp")
            self.graph.add_node(source,type= "Temp")
            
        if destination not in self.nodes:
            self.addNode(destination,"Temp")     
            self.graph.add_node(destination,type= "Temp")
            
        self.nodes[source].addFanout(destination)
        self.nodes[destination].addFanin(source)    
        self.graph.add_edge(source,destination)
        
    def removeEdge(self,source,destination):
        """removeEdge Method: Removes the edge between the source and destination"""
        self.nodes[source].removeFanout(destination)
        self.nodes[destination].removeFanin(source)  
        self.graph.remove_edge(source,destination)
        
        
    def removeNode(self,nodeName):
        """removeNode Method: Removes node from the circuit"""
        fanin = self.nodes[nodeName].getFanin()
        fanout = self.nodes[nodeName].getFanout()
        while len(fanin)>0:
            self.removeEdge(fanin[0],nodeName)
            
        while len(fanout)>0:
            self.removeEdge(nodeName,fanout[0])
            
        del self.nodes[nodeName]
        self.graph.remove_node(nodeName)
        
    def getNodes(self):
        """getNodes Method: Gives all the nodes of the Circuit"""
        return self.nodes
    
    def getInputs(self):
        """getInputs Method: Gives all the input of the Circuit"""
        return self.inputs 
    
    def getOutputs(self):
        """getOutputs Method: Gives all the outputs of the Circuit"""
        return self.outputs
    
    def getKeyinputs(self):
        """getNodes Method: Gives all the keyinputs of the Circuit"""
        return self.keyinputs 
    
    def addDFF(self,*,d ="",clk = "",Q = "",rst = "",Qb = "",st = ""):
        """addDFF Method : Adds the DFF to the Circuit \t
            d: input to the DFF\t
            clk : Clock to the DFF\t
            Q : DFF output (DFF node name)\t
            Qb : Inveter of DFF output \t
            st : Set of the DFF 
            NOTE : Q cannot be empty string \t
        """
        
        dff_name = ""
        if Q != "":
            dff_name = Q
        else: 
            if Qb != "":
                dff_name = Qb+"_internal"
        
        if d != "":
            if d not in self.nodes:
                self.addNode(d,"Temp")
                self.graph.add_node(d,type= "Temp")
        else: 
            print("Error: Input to the DFF {} is Missing ".format(dff_name))
        
        if st != "":
            if st not in self.nodes:
                self.addNode(st,"Temp")
                self.graph.add_node(st,type= "Temp")
        
        if clk != "":   
            if clk not in self.nodes:
                self.addNode(clk,"Temp")
                self.graph.add_node(clk,type= "Temp")
        else:
            print("Error: Clock to the DFF {} is Missing ".format(dff_name))
            
        if (rst != "") and (rst not in self.nodes):
            self.addNode(rst,"Temp")
            self.graph.add_node(rst,type= "Temp")
            
        if (dff_name not in self.nodes and dff_name != ""):
            self.addNode(dff_name,"DFF")
            self.graph.add_node(dff_name,type= "DFF")
            
        elif (dff_name in self.nodes):
            if self.nodes[dff_name].getNodeType() == "Temp":
                self.nodes[dff_name].changeNodeType("DFF")
                self.graph.nodes[dff_name]["type"] = "DFF"
            
        if (Qb != "") and (Qb not in self.nodes):
            self.addNode(Qb,"NOT")
            self.graph.add_node(Qb,type= "NOT")
            
        elif Qb in self.nodes and self.nodes[Qb].getNodeType() == "Temp":
            self.nodes[Qb].changeNodeType("NOT")
            self.graph.nodes[Qb]["type"] = "NOT"
            
        self.addEdge(d,dff_name)
        self.addEdge(clk,dff_name)
        self.graph.add_edge(d,dff_name)
        self.graph.add_edge(clk,dff_name)
        
        if rst != "":
            self.addEdge(rst,dff_name)
            self.graph.add_edge(rst,dff_name)
        if Qb != "":
            self.addEdge(dff_name,Qb)
            self.graph.add_edge(dff_name,Qb)
        if st != "":
            self.addEdge(st,dff_name)
            self.graph.add_edge(st,dff_name)
            
    def addAO(self,**kwargs):
        """addAO Method: Adds AO complex gate to the Circuit\t
            kwargs arguments : inputs,output
            input can be of any length > 2.
            output should be given with the keyword output\t
            Ex : addAO(inputs = [d1,d2,d3,d4],"output" = d5)\t
        """
        if kwargs["output"] not in self.nodes:
            self.addNode(kwargs["output"],"OR")
            self.graph.add_node(kwargs["output"],type="OR")
            
        elif kwargs["output"] in self.nodes and self.nodes[kwargs["output"]].getNodeType() == "Temp":
            self.nodes[kwargs["output"]].changeNodeType("OR")
            self.graph.nodes[kwargs["output"]]["type"] = "OR"
            
        for eachargv in kwargs["inputs"]:
            if eachargv not in self.nodes:
                self.addNode(eachargv,"Temp")
                self.graph.add_node(eachargv,type="Temp")
                
        gatename = kwargs["output"]+"_and_"
        andGateCount = len(kwargs["inputs"])//2
        for i in range(0,andGateCount):
            self.addNode(gatename+str(i),"AND")
            self.graph.add_node(gatename+str(i),type = "AND")
            
            self.addEdge(gatename+str(i),kwargs["output"])
            self.addEdge(kwargs["inputs"][2*i],gatename+str(i))
            self.addEdge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
            self.graph.add_edge(gatename+str(i),kwargs["output"])
            self.graph.add_edge(kwargs["inputs"][2*i],gatename+str(i))
            self.graph.add_edge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
        if len(kwargs["inputs"])%2 != 0:
            self.addEdge(kwargs["inputs"][-1],kwargs["output"])
            self.graph.add_edge(kwargs["inputs"][-1],kwargs["output"])
            
    def addOA(self,**kwargs):
        """addOA Method: Adds OA complex gate to the Circuit\t
            kwargs arguments : inputs,output
            input can be of any length > 2.
            output should be given with the keyword output\t
            Ex : addOA(inputs=[d1,d2,d3,d4],output = d5)\t
        """
        if kwargs["output"] not in self.nodes:
            self.addNode(kwargs["output"],"AND")
            self.graph.add_node(kwargs["output"],type = "AND")
            
        elif kwargs["output"] in self.nodes and self.nodes[kwargs["output"]].getNodeType() == "Temp":
            self.nodes[kwargs["output"]].changeNodeType("AND")
            self.nodes[kwargs["output"]]["type"]="AND"
            
        for eachargv in kwargs["inputs"]:
            if eachargv not in self.nodes:
                self.addNode(eachargv,"Temp")
                self.graph.add_node(eachargv,type="Temp")
                
        gatename = kwargs["output"]+"_or_"
        orGateCount = len(kwargs["inputs"])//2
        for i in range(0,orGateCount):
            self.addNode(gatename+str(i),"OR")
            self.graph.add_node(gatename+str(i),type = "OR")
            
            self.addEdge(gatename+str(i),kwargs["output"])
            self.addEdge(kwargs["inputs"][2*i],gatename+str(i))
            self.addEdge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
            self.graph.add_edge(gatename+str(i),kwargs["output"])
            self.graph.add_edge(kwargs["inputs"][2*i],gatename+str(i))
            self.graph.add_edge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
        if len(kwargs["inputs"])%2 != 0:
            self.addEdge(kwargs["inputs"][-1],kwargs["output"])
            self.graph.add_edge(kwargs["inputs"][-1],kwargs["output"])
            
    def addAOI(self,**kwargs):
        """addAOI Method: Adds AOI complex gate to the Circuit\t
            input can be of any length > 2.
            output should be given with the keyword output\t
            Ex : addAOI(d1,d2,d3,d4,output = d5)\t
        """
        if kwargs["output"] not in self.nodes:
            self.addNode(kwargs["output"],"NOT")
            self.graph.add_node(kwargs["output"],type = "NOT")
            
        elif kwargs["output"] in self.nodes and self.nodes[kwargs["output"]].getNodeType() == "Temp":
            self.nodes[kwargs["output"]].changeNodeType("NOT")
            self.graph.nodes[kwargs["output"]]["type"] = "NOT"
            
        orGateName = kwargs["output"]+"_or"
        if orGateName not in self.nodes:
            self.addNode(orGateName,"OR")
            self.graph.add_node(orGateName,type="OR")
            
        for eachargv in kwargs["inputs"]:
            if eachargv not in self.nodes:
                self.addNode(eachargv,"Temp")
                self.graph.add_node(eachargv,type = "Temp")
        gatename = kwargs["output"]+"_and_"
        andGateCount = len(kwargs["inputs"])//2
        for i in range(0,andGateCount):
            self.addNode(gatename+str(i),"AND")
            self.graph.add_node(gatename+str(i),type = "AND")
            
            self.addEdge(gatename+str(i),orGateName)
            self.addEdge(kwargs["inputs"][2*i],gatename+str(i))
            self.addEdge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
            self.graph.add_edge(gatename+str(i),orGateName)
            self.graph.add_edge(kwargs["inputs"][2*i],gatename+str(i))
            self.graph.add_edge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
        if len(kwargs["inputs"])%2 != 0:
            self.addEdge(kwargs["inputs"][-1],orGateName)
            self.graph.add_edge(kwargs["inputs"][-1],orGateName)
            
        self.addEdge(orGateName,kwargs["output"])
        self.graph.add_edge(orGateName,kwargs["output"])
    
    
    def addOAI(self,**kwargs):
        """addOAI Method: Adds OAI complex gate to the Circuit\t
            input can be of any length > 2.
            output should be given with the keyword output\t
            Ex : addAOI(d1,d2,d3,d4,output = d5)\t
        """
        if kwargs["output"] not in self.nodes:
            self.addNode(kwargs["output"],"NOT")
            self.graph.add_node(kwargs["output"],type = "NOT")
            
        elif kwargs["output"] in self.nodes and self.nodes[kwargs["output"]].getNodeType() == "Temp":
            self.nodes[kwargs["output"]].changeNodeType("NOT")
            self.graph.nodes[kwargs["output"]]["type"]="NOT"
            
        andGateName = kwargs["output"]+"_and"
        if andGateName not in self.nodes:
            self.addNode(andGateName,"AND")
            self.graph.add_node(andGateName,type = "AND")
            
        for eachargv in kwargs["inputs"]:
            if eachargv not in self.nodes:
                self.addNode(eachargv,"Temp")
                self.graph.add_node(eachargv,type = "Temp")
                
        gatename = kwargs["output"]+"_or_"
        orGateCount = len(kwargs["inputs"])//2
        for i in range(0,orGateCount):
            self.addNode(gatename+str(i),"OR")
            self.graph.add_node(gatename+str(i),type = "OR")
            
            self.addEdge(gatename+str(i),andGateName)
            self.addEdge(kwargs["inputs"][2*i],gatename+str(i))
            self.addEdge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
            self.graph.add_edge(gatename+str(i),andGateName)
            self.graph.add_edge(kwargs["inputs"][2*i],gatename+str(i))
            self.graph.add_edge(kwargs["inputs"][(2*i)+1],gatename+str(i))
            
        if len(kwargs["inputs"])%2 != 0:
            self.addEdge(kwargs["inputs"][-1],andGateName)
            self.graph.add_edge(kwargs["inputs"][-1],andGateName)
            
        self.addEdge(andGateName,kwargs["output"])
        self.graph.add_edge(andGateName,kwargs["output"])
        
    def addMux(self,*,sel = "",in0= "",in1= "",output = ""):
        """addMux Method: Adds the Mux Gate to the Circuit\t
        sel = 0 : in0 will be selected \t
        sel = 1 : in1 will be selected\t
        Ex: addMux(sel = n1,in0 = n2,in1 = n3,output = out)\t"""
        
        if output[0] != "":
            if output[0] not in self.nodes:
                self.addNode(output[0],"OR")
                self.graph.add_node(output[0],type = "OR")
                
            elif output[0] in self.nodes and self.nodes[output[0]].getNodeType() == "Temp":
                self.nodes[output[0]].changeNodeType("OR")
                self.graph.nodes[output[0]]["type"]="OR"
        else:
            print("Error: Output to the Mux is Missing ")
            
        if sel != "":
            if sel not in self.nodes:
                self.addNode(sel,"Temp")
                self.graph.add_node(sel,type = "Temp")
        else: 
            print("Error: sel to the Mux {} is Missing ".format(output))
        if in1 != "":
            if in1 not in self.nodes:
                self.addNode(in1,"Temp")
                self.graph.add_node(in1,type = "Temp")
        else:
            print("Error: in1 to the Mux {} is Missing ".format(output))
            
        if in0 != "":
            if in0 not in self.nodes:
                self.addNode(in0,"Temp")
                self.graph.add_node(in0,type ="Temp")
        else:
            print("Error: in0 to the Mux {} is Missing ".format(output))
        
        notGateName = output[0]+"_not"
        andGate0 = output[0]+"_and_0"
        andGate1 = output[0]+"_and_1"
        self.addNode(notGateName,"NOT")
        self.addNode(andGate0,"AND")
        self.addNode(andGate1,"AND")
        self.addEdge(in0,andGate0)
        self.addEdge(in1,andGate1)
        self.addEdge(sel,andGate1)
        self.addEdge(sel,notGateName)
        self.addEdge(notGateName,andGate0)
        self.addEdge(andGate0,output[0])
        self.addEdge(andGate1,output[0])
        
        self.graph.add_node(notGateName,type = "NOT")
        self.graph.add_node(andGate0,type = "AND")
        self.graph.add_node(andGate1,type ="AND")
        self.graph.add_edge(in0,andGate0)
        self.graph.add_edge(in1,andGate1)
        self.graph.add_edge(sel,andGate1)
        self.graph.add_edge(sel,notGateName)
        self.graph.add_edge(notGateName,andGate0)
        self.graph.add_edge(andGate0,output[0])
        self.graph.add_edge(andGate1,output[0])
        
    def addHalfAdder(self,*,in0 = "",in1 = "",s = "",cout = ""):
        """addHalfAdder Method : Adds Halfadder to the Circuit \t
            argument : in0,in1,s,cout.\t
            s -> sum ; cout-> carryout\t
            Ex: addHalfAdder(in0 = A,in1= B,s = sum,cout = cout)    
        """
        if s not in self.nodes:
            self.addNode(s,"XOR")
            self.graph.add_node(s,type = "XOR")
        elif s in self.nodes and self.nodes[s].getNodeType() == "Temp":
            self.nodes[s].changeNodeType("XOR")
            self.graph.nodes[s]["type"]= "XOR"
            
        if cout not in self.nodes:
            self.addNode(cout,"AND")
            self.graph.add_node(cout,type = "AND")
            
        elif cout in self.nodes and self.nodes[cout].getNodeType() == "Temp":
            self.nodes[cout].changeNodeType("AND")
            self.graph.nodes[cout]["type"]="AND"
            
            
        if in0 not in self.nodes:
            self.addNode(in0,"Temp")
            self.graph.add_node(in0,type = "Temp")
        if in1 not in self.nodes:
            self.addNode(in1,"Temp")
            self.graph.add_node(in1,type = "Temp")
          
        self.addEdge(in0,s)
        self.addEdge(in1,s)
        self.addEdge(in0,cout)
        self.addEdge(in1,cout)
        
        self.graph.add_edge(in0,s)
        self.graph.add_edge(in1,s)
        self.graph.add_edge(in0,cout)
        self.graph.add_edge(in1,cout)
        
        
        
    
    def addFullAdder(self,*,in0 = "",in1 = "",cin = "",s = "",cout = ""):
        """addFullAdder Method : Adds fulladder to the Circuit \t
            argument : in0,in1,cin,s,cout.\t
            s -> sum ; cout-> carryout;cin -> carryin \t
            Ex: addFullAdder(in0 = A,in1= B,cin= C,s = sum,cout = cout)    
        """
        if cin not in self.nodes:
            self.addNode(s,"Temp")
            self.graph.add_node(s,type = "Temp")
        if s not in self.nodes:
            self.addNode(s,"XOR")
            self.graph.add_node(s,type = "XOR")
            
        elif s in self.nodes and self.nodes[s].getNodeType() == "Temp":
            self.nodes[s].changeNodeType("XOR")
            self.nodes[s]["type"] = "XOR"
        
            
        if cout not in self.nodes:
            self.addNode(cout,"OR")
            self.graph.add_node(cout,type = "OR")
        elif cout in self.nodes and self.nodes[cout].getNodeType() == "Temp":
            self.nodes[cout].changeNodeType("OR")
            self.nodes[cout]["type"] = "OR"
            
        internalSum = s+"_sum"
        internalCarry0 =cout+"_carry_0"
        internalCarry1 = cout+"_carry_1"
        self.addNode(internalSum,"XOR")
        self.addEdge(in0,internalSum)
        self.addEdge(in1,internalSum)
        self.addEdge(internalSum,s)
        self.addEdge(cin,s)
        self.addNode(internalCarry0,"AND")
        self.addNode(internalCarry1,"AND")
        self.addEdge(cin,internalCarry0)
        self.addEdge(internalSum,internalCarry0)
        self.addEdge(in0,internalCarry1)
        self.addEdge(in1,internalCarry1)
        self.addEdge(internalCarry0,cout)
        self.addEdge(internalCarry1,cout)
        
        self.graph.add_node(internalSum,type = "XOR")
        self.graph.add_edge(in0,internalSum)
        self.graph.add_edge(in1,internalSum)
        self.graph.add_edge(internalSum,s)
        self.graph.add_edge(cin,s)
        self.graph.add_node(internalCarry0,type="AND")
        self.graph.add_nodeNode(internalCarry1,type = "AND")
        self.graph.add_edge(cin,internalCarry0)
        self.graph.add_edge(internalSum,internalCarry0)
        self.graph.add_edge(in0,internalCarry1)
        self.graph.add_edge(in1,internalCarry1)
        self.graph.add_edge(internalCarry0,cout)
        self.graph.add_edge(internalCarry1,cout)
        
    def addZero(self):
        """addZero Method : Adds 1'b0 node to circuit"""
        self.addNode("zero","XOR")
        self.addEdge(self.inputs[0],"zero")
        self.addEdge(self.inputs[0],"zero")
        
        self.graph.add_node("zero",type = "XOR")
        self.graph.add_edge(self.inputs[0],"zero")
        self.graph.add_edge(self.inputs[0],"zero")
        
    def addOne(self):
        """addOne Method : Adds 1'b1 node to circuit"""
        self.addNode("one","XNOR")
        self.addEdge(self.inputs[0],"one")
        self.addEdge(self.inputs[0],"one")
    
        self.graph.add_node("one",type = "XNOR")
        self.graph.add_edge(self.inputs[0],"one")
        self.graph.add_edge(self.inputs[0],"one")
        
    def addLevel(self,nodes):
        nextLevel = []
        for eachnode in nodes:
            fanout = self.nodes[eachnode].getFanout()
            presentLevel = self.nodes[eachnode].getNodeLevel()
            self.nodes[eachnode].setNodeLevelFlag()
            for eachfanout in fanout:
                if not self.nodes[eachfanout].getNodeLevelFlag():
                    nextLevel.append(eachfanout)
                    self.nodes[eachfanout].changeNodeLevel(presentLevel+1)
                if len(list(set(nextLevel))) != 0:
                    try:
                        self.addLevel(nextLevel)
                    except RecursionError:
                        print("Exiting the system")
                        sys.exit()
                else:
                    break
        
    def outputConeOfInfluence(self,node):
        allNodes = list(self.nodes.keys())
        for eachnode in allNodes:
            self.nodes[eachnode].resetNodeLevelFlag()
        nodesInCone = []
        def genFunc(nodes):
            nextLevel = []
            for eachnode in nodes:        
                fanout = self.nodes[eachnode].getFanout()
                self.nodes[eachnode].setNodeLevelFlag()
                for eachfanout in fanout:
                    if not self.nodes[eachfanout].getNodeLevelFlag():
                        nextLevel.append(eachfanout)
                    if len(list(set(nextLevel))) != 0:
                        for each in nextLevel :
                            nodesInCone.append(each)
                        genFunc(nextLevel)
                    else:
                        break
        genFunc([node])
        return list(set(nodesInCone))
        
    
    def inputConeOfInfluence(self,node):
        allNodes = list(self.nodes.keys())
        for eachnode in allNodes:
            self.nodes[eachnode].resetNodeLevelFlag()
        nodesInCone = []
        def genFunc(nodes):
            nextLevel = []
            for eachnode in nodes:   
                fanin = self.nodes[eachnode].getFanin()
                for eachfanin in fanin:
                    if not self.nodes[eachfanin].getNodeLevelFlag():
                        nextLevel.append(eachfanin)
                    if len(list(set(nextLevel))) != 0:
                        for each in nextLevel :
                            nodesInCone.append(each)
                        genFunc(nextLevel)
                    else:
                        break
        genFunc([node])
        
        return list(set(nodesInCone))
            
            
    def splitingLevels(self):
        for eachnode in self.nodes:
            try:
                self.levels[str(self.nodes[eachnode].getNodeLevel())].append(eachnode)
            except:
                self.levels[str(self.nodes[eachnode].getNodeLevel())] = []
                self.levels[str(self.nodes[eachnode].getNodeLevel())].append(eachnode)
                
                
                
        
    def circuitCheck(self):
        for node,value in self.nodes.items():
            fanin = value.getFanin()
            fanout = value.getFanout()
            nodeType = value.getNodeType()
            if (len(fanin) < 2) and (not value.isNodeInput()):
                if(len(fanin) != 1) and (nodeType != "NOT") and (nodeType != "BUF"):
                    print("Error: {} which is {} has {} fanin->{}".format(node,nodeType,len(fanin),fanin))
            if (len(fanin) == 0) and (nodeType == "NOT") and (nodeType == "BUF"):
                    print("Error: {} which is {} has {} fanin->{}".format(node,nodeType,len(fanin),fanin))        
            
            if (len(fanout) == 0) and (not value.isNodeOutput()):
                print("Error: {} have zero fanout and its not output node ".format(node))
                
            
            gatetypeallowed = ["AND","OR","NAND","NOR","XOR","XNOR","AO","AOI","OA","OAI","DFF","MUX",
                            "FULLADDER","HALFADDER","INPUT","NOT","BUF"]
            
            if nodeType not in gatetypeallowed:
                print("Error: {} which is of type {} is not supported".format(node,nodeType))
                    
            
            
                    
            
        
        
        
        
            
    
    
        
        
            
        
    
    
    
