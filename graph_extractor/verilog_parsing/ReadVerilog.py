# -*- coding: utf-8 -*-
"""
Created on Thu Feb 25 09:28:22 2021

@author: Juneethkumar Meka
"""
import os
from verilog_parsing import VerilogParsing
from circuit import Circuit


class ReadVerilog:
    """ReadVerilog Class: Reads the Verilog file and build the circuit"""
    
    __author__ = "Juneethkumar Meka"
    __version__ = "1.0"
    __credits__ = ["Juneethkumar Meka"]
    __license__ = "Private Domain :  Requires Approval for the use from Juneethkumar Meka"
    __maintainer__ = "Juneethkumar Meka"
    __contact__ = "juneethkumarmeka@gmail.com"
    __status__ = "Development"

    __all__ = ["getCircuit","addInputs","addOutputs","addInstances",
               "replaceSiglebit","replaceAssign"]
    
    def __init__(self,filename,*,outputPattern=r"Q|Y|Z|Qn",
                 clockPattern=r"CLK|clock|clk|CK",
                 resetPattern=r"rst|reset|RST|Rst|RN",
                 setPattern=r"SN|SET|Set",
                 selectPattern=r"sel|S|Sel",
                 allowedAND=["AND"],
                 allowedOR=["OR"],
                 allowedNOT=["INV", "NOT"],
                 allowedNOR=["NOR"],
                 allowedNAND=["NAND"],
                 allowedXOR=["XOR"],
                 allowedXNOR=["XNOR"],
                 allowedMUX=["MUX", "MX"],
                 allowedDFF=["DFF", "SDFF"],
                 allowedAOI=["AOI"],
                 allowedOAI=["OAI"],
                 allowedAO=["AO"],
                 allowedOA=["OA"],
                 allowedBUF=["BUF","CLKBUF"],
                 allowedHALFADDER=["HALFADDER"],
                 allowedFULLADDER=["FULLADDER","FADD"]):
        self.filename = filename
        self.parser = VerilogParsing(filename,outputPattern=outputPattern,
                                     clockPattern=clockPattern,
                                     resetPattern=resetPattern,
                                     setPattern=setPattern,
                                     selectPattern=selectPattern,
                                     allowedAND=allowedAND,
                                     allowedOR=allowedOR,
                                     allowedNOT=allowedNOT,
                                     allowedNOR=allowedNOR,
                                     allowedNAND=allowedNAND,
                                     allowedXOR=allowedXOR,
                                     allowedXNOR=allowedXNOR,
                                     allowedMUX=allowedMUX,
                                     allowedDFF=allowedDFF,
                                     allowedAOI=allowedAOI,
                                     allowedOAI=allowedOAI,
                                     allowedAO=allowedAO,
                                     allowedOA=allowedOA,
                                     allowedBUF=allowedBUF,
                                     allowedHALFADDER=allowedHALFADDER,
                                     allowedFULLADDER=allowedFULLADDER)
        self.circuit = Circuit(os.path.basename(filename).strip(".v"))
        self.graph = self.circuit.getGraph()
        self.addInputs()
        self.addOutputs()
        self.addInstances()
        self.replaceSiglebit()
        self.replaceAssign()
        self.circuit.circuitCheck()
        self.circuit.addLevel(self.circuit.getInputs())
        self.circuit.splitingLevels()
        
    def getCircuit(self):
        return self.circuit
    
    def getGraph(self):
        return self.circuit.getGraph()
    
    def getInputs(self):
        return self.parser.getInputs()
    
    def getOutputs(self):
        return self.parser.getOutputs()

    def getNodes(self):
        return self.circuit.getNodes()
    
    def addInputs(self):
        for eachinput in self.parser.getInputs():
            self.circuit.addNode(eachinput,"INPUT",isinput=True)
            if eachinput.startswith("keyinput"):
                self.circuit.getNodes()[eachinput].setNodeAsKeyinput()
    
    def addOutputs(self):
        for eachoutput in self.parser.getOutputs():
            self.circuit.addNode(eachoutput,"Temp",isoutput=True)
    
    def replaceSiglebit(self):
        if "1'b1" in self.circuit.getNodes():
            self.circuit.getNodes()["1'b1"].changeNodeType("XNOR")
            self.graph.nodes["1'b1"]["type"] = "XNOR"
            self.circuit.addEdge(self.circuit.getInputs()[0],"1'b1")
            self.circuit.addEdge(self.circuit.getInputs()[0],"1'b1")
        elif "1'b0" in self.circuit.getNodes():
            self.circuit.getNodes()["1'b0"].changeNodeType("XOR")
            self.graph.nodes["1'b0"]["type"] = "XNOR"
            self.circuit.addEdge(self.circuit.getInputs()[0],"1'b0")
            self.circuit.addEdge(self.circuit.getInputs()[0],"1'b0")
    
    def replaceAssign(self):
        for key,value in self.parser.getAssign().items():
            try:
                fanin = self.circuit.getNodes()[key].getFanin()
                fanout = self.circuit.getNodes()[key].getFanout()
                for eachfanin in fanin:
                    self.circuit.getNodes()[value].addFanin(eachfanin)
                    self.graph.add_edge(eachfanin,value)
                for eachfanout in fanout:
                    self.circuit.getNodes()[value].addFanout(eachfanout)
                    self.graph.add_edge(value,eachfanout)
                self.circuit.removeNode(key)
                self.graph.remove_node(key)
            except:
                pass
            
            
    def addInstances(self):
        instances = self.parser.getInstances()
        for key,value in instances.items():
            gateType = value.getGateType()
            output = value.getOutputs()
            inputs = value.getInputs()
            nodes = self.circuit.getNodes()
            excludedGates = ["MUX","AO","AOI","OA","OAI","DFF","HALFADDER","FULLADDER"]
            complexGates = ["AO","AOI","OA","OAI"]
            if len(output)==1:
                if (gateType not in excludedGates):
                    if output[0] not in nodes:
                        self.circuit.addNode(output[0],gateType)
                    if output[0] in nodes:
                        if self.circuit.getNodes()[output[0]].getNodeType() == "Temp":
                            self.circuit.getNodes()[output[0]].changeNodeType(gateType)
                            self.graph.nodes[output[0]]["type"] = gateType
                    for eachinput in inputs:
                        if eachinput.strip(" ") not in nodes:
                            self.circuit.addNode(eachinput,"Temp")
                        self.circuit.addEdge(eachinput.strip(" "),output[0])
                elif gateType in complexGates:
                    updatedinput = []
                    for eachinput in inputs:
                        updatedinput.append(eachinput.strip(" "))
                    if gateType == "AO":
                        self.circuit.addAO(inputs=updatedinput,output=output[0])
                    elif gateType == "AOI":
                        self.circuit.addAOI(inputs=updatedinput,output=output[0])
                    elif gateType == "OA":
                        self.circuit.addOA(inputs=updatedinput,output=output[0])
                    elif gateType == "OAI":
                        self.circuit.addOAI(inputs=updatedinput,output=output[0])
                elif gateType == "DFF":
                    clk = value.getClk()
                    rst = value.getRst()
                    se = value.getSet()
                    Qb = value.getQb()
                    if output[0] == "" and Qb != "":
                        output[0] = Qb+"_internal"    
                    if (rst == None) and (se == None) and (Qb==None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0])
                    elif (rst == None) and (se == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],Qb=Qb)
                    elif (rst == None) and (Qb == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],st=se)
                    elif (set == None) and (Qb == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],rst=rst)
                    elif (rst == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],Qb=Qb,st=se)
                    elif (se == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],Qb=Qb,rst=se)
                    elif (Qb == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],rst=rst,st=se)
                    else:
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=output[0],Qb=Qb,rst=rst,st=se)
                elif gateType == "MUX":
                    sel = value.getSel()
                    inputs = value.getInputs()
                    output = value.getOutputs()
                    self.circuit.addMux(sel=sel,in0=inputs[0],in1=inputs[1],output=output)
                elif gateType == "HALFADDER":
                    inputs = value.getInputs()
                    output = value.getOutputs()
                    cout = value.getCout()
                    self.circuit.addHalfAdder(in0 =inputs[0],in1 = inputs[1],s = output[0],cout = cout)
                elif gateType == "FULLADDER":
                    inputs = value.getInputs()
                    output = value.getOutputs()
                    cout = value.getCout()
                    cin = value.getCin()
                    self.circuit.addFullAdder(in0 =inputs[0],in1 = inputs[1],
                                              s = output[0],cout = cout,cin=cin)
                    
            elif len(output) ==0 :
                if gateType == "DFF":
                    clk = value.getClk()
                    rst = value.getRst()
                    se = value.getSet()
                    Qb = value.getQb()
                    Q = Qb+"_internal"
                    if (rst == None) and (se == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=Q,Qb=Qb)
                    elif (rst == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=Q,st=se,Qb=Qb)
                    elif (set == None):  
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=Q,rst=rst,Qb= Qb)
                    else:
                        self.circuit.addDFF(d=inputs[0],clk=clk,Q=Q,rst=rst,Qb= Qb,st=se)
                else:
                    print("Warning: Please check the Synthesis Again")
                        
            else:
                print("Warning: Please check the Synthesis Again")
                    
