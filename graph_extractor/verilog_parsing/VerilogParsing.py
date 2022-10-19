# -*- coding: utf-8 -*-
"""
Created on Mon Feb 22 05:07:11 2021

@author: Juneethkumar Meka
"""
import os
import sys
import re
from verilog_parsing import Gate,DFF,Mux,HalfAdder,FullAdder

class VerilogParsing:
    """Verilog Parsing Class :
    Takes verilog file as input and parse the file"""
    __author__ = "Juneethkumar Meka"
    __version__ = "1.0"
    __credits__ = ["Juneethkumar Meka"]
    __license__ = "Private Domain :  Requires Approval for the use from Juneethkumar Meka"
    __maintainer__ = "Juneethkumar Meka"
    __contact__ = "juneethkumarmeka@gmail.com"
    __status__ = "Development"
    __all__ = ["fileCheck", "setOutputPattern", "setClkPattern", "setResetPattern",
               "setSetPattern", "extractLines", "patternFunction", "extraction",
               "extractNets", "getInstances","getAssign"]

    def __init__(self, filePath,*,outputPattern=r"Q|Y|Z|Qn",
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
        
        self.filepath = filePath
        self.lines = []
        self.inputs = []
        self.outputs = []
        self.patterns = {}
        self.instances = {}
        self.assign = {}
        self.allowedGates = {"AND":allowedAND,
                             "OR": allowedOR,
                             "NOT":allowedNOT,
                             "NOR":allowedNOR,
                             "NAND":allowedNAND,
                             "XOR":allowedXOR,
                             "XNOR":allowedXNOR,
                             "MUX":allowedMUX,
                             "DFF":allowedDFF,
                             "AOI":allowedAOI ,
                             "OAI": allowedOAI,
                             "OA": allowedOA,
                             "AO": allowedAO,
                             "BUF":allowedBUF,
                             "HALFADDER" : allowedHALFADDER,
                             "FULLADDER" :allowedFULLADDER} 
        
        self.outputPattern = outputPattern
        self.clockPattern = clockPattern
        self.resetPattern = resetPattern
        self.setPattern = setPattern
        self.selectPattern = selectPattern
        self.fileCheck()
        self.extractLines()
        self.patternFunction()
        self.extraction()
        self.extractNets()
        

    def fileCheck(self):
        """fileCheck Method: Checks for the file path and file format"""
        if not os.path.exists(self.filepath):
            print("Error: Invalid,File Path {}\n".format(self.filepath))
            sys.exit()
        else:
            if not self.filepath.endswith(".v"):
                print("Error: Doesnot supports other file formats {}".format(self.filepath))
                sys.exit()

    def extractLines(self):
        """extractLines Method : This medthod extracts the lines and
        removes the commented lines. Extract lines with respect to semi-colon.
        """
        fp = open(self.filepath, "r")
        previousLine = ";"
        for eachLine in fp.readlines():
            if not (eachLine.startswith("//") or eachLine.startswith("\n")):
                currentLine = eachLine.strip()
                if currentLine.endswith(";"):
                    if previousLine.endswith(";"):
                        self.lines.append(currentLine)
                        previousLine = currentLine
                    else:
                        previousLine = previousLine+currentLine
                        self.lines.append(previousLine)
                else:
                    if previousLine.endswith(";"):
                        previousLine = currentLine
                    else:
                        previousLine = previousLine+currentLine

    def patternFunction(self):
        """PatternFunction method: Adds the regular expression"""
        self.patterns["multibit"] = r"\w+\s*\[(\d+):(\d+)\]\s*.*$"
        self.patterns["input0"] = r"input\s+wire\s+(.*);$"
        self.patterns["input1"] = r"input\s+wire\s+\[\d+\s*:\d+\]\s*(.*);$"
        self.patterns["input2"] = r"input\s*\[\d+\s*:\d+\]\s*(.*);$"
        self.patterns["input3"] = r"input\s*(.*);$"
        self.patterns["output0"] = r"output\s+wire\s+(.*);$"
        self.patterns["output1"] = r"output\s+wire\s+\[\d+\s*:\d+\]\s*(.*);$"
        self.patterns["output2"] = r"output\s+reg\s+(.*);$"
        self.patterns["output3"] = r"output\s+reg\s+\[\d+\s*:\d+\]\s*(.*);$"
        self.patterns["output4"] = r"output\s+\[\d+\s*:\d+\]\s*(.*);$"
        self.patterns["output5"] = r"output\s+(.*);$"
        self.patterns["net"] = r"(.*?)\s+(.*?)\s*\((.*)\);"
        self.patterns["namedport"] = r"\.(.*?)\s*\(.*\)"
        self.patterns["namedportvalue"] = r"\..*?\s*\((.*)\)"
        self.patterns["assign"] = r"assign\s*(\S+?)\s*=\s*(\S+);"

    def getPatternDetect(self, pattern, string):
        """getPatternDetect: Detect the pattern in the
        string variable basing on the pattern.\t
        pattern arguments: multibit,inputs,outputs,net,namedport,namedportvalue,assign
        string argument is the input line from which the pattern to be detected
        Ex: getPatternDetect(net,"nand u1 (N1,N2,n3)")
        """
        if pattern == "multibit":
            stringPattern = ["multibit"]
        elif pattern == "inputs":
            stringPattern = ["input0", "input1", "input2", "input3"]
        elif pattern == "outputs":
            stringPattern = ["output0", "output1", "output2",
                             "output3", "output4", "output5"]
        elif pattern == "net":
            stringPattern = ["net"]
        elif pattern == "namedport":
            stringPattern = ["namedport"]
        elif pattern == "namedportvalue":
            stringPattern = ["namedportvalue"]
        elif pattern == "assign":
            stringPattern = ["assign"]
        

        for eachStringPattern in stringPattern:
            matchobj = re.match(self.patterns[eachStringPattern], string,
                                re.M | re.I)
            if matchobj:
                return matchobj
        return matchobj

    def gatePatterns(self, string):
        """gatePatterns Method: Stores all the Names for the Gates"""
        string = string.upper()
        for key, value in self.allowedGates.items():
            for each in value:
                if string.startswith(each):
                    return key
                

    def extraction(self):
        """extraction Method: Extracts all the input and output Nodes"""
        for eachline in self.lines:
            if eachline.startswith("output"):
                multibit = self.getPatternDetect("multibit", eachline)
                outputNodes = self.getPatternDetect("outputs", eachline)
                if multibit and outputNodes:
                    msb = 0
                    lsb = 0
                    if int(multibit.group(1)) > int(multibit.group(2)):
                        msb = multibit.group(1)
                        lsb = multibit.group(2)
                    else:
                        msb = multibit.group(2)
                        lsb = multibit.group(1)
                    for i in range(int(lsb), int(msb)+1):
                        for j in outputNodes.group(1).split(","):
                            nodeName = "{}[{}]".format(j.strip(" "), str(i))
                            self.outputs.append(nodeName)
                elif not multibit and outputNodes:
                    for eachnode in outputNodes.group(1).split(","):
                        self.outputs.append(eachnode.strip(" "))

            if eachline.startswith("input"):
                multibit = self.getPatternDetect("multibit", eachline)
                inputNodes = self.getPatternDetect("inputs", eachline)
                if multibit and inputNodes:
                    msb = 0
                    lsb = 0
                    if int(multibit.group(1)) > int(multibit.group(2)):
                        msb = multibit.group(1)
                        lsb = multibit.group(2)
                    else:
                        msb = multibit.group(2)
                        lsb = multibit.group(1)
                    for i in range(int(lsb), int(msb)+1):
                        for j in inputNodes.group(1).split(","):
                            nodeName = "{}[{}]".format(j.strip(" "), str(i))
                            self.inputs.append(nodeName)
                elif not multibit and inputNodes:
                    for eachnode in inputNodes.group(1).split(","):
                        self.inputs.append(eachnode.strip(" "))
            if eachline.startswith("assign"):
                assignobj = self.getPatternDetect("assign", eachline)
                if assignobj:
                    self.assign[assignobj.group(1)] = assignobj.group(2)

    def setOutputPattern(self, string):
        """setOutputPattern Method : Takes Regular expression for the
        recognizing the Output portnames\t
        Ex: setOutputPattern(r'Q|Qn|Y')
        """
        self.outputPattern = string

    def setClkPattern(self, string):
        """setClkPattern Method : Takes Regular expression for the
        recognizing the Clock portnames\t
        Ex: setClkPattern(r'CLK')
        """
        self.clockPattern = string

    def setResetPattern(self, string):
        """setResetPattern Method : Takes Regular expression for the
        recognizing the reset portnames\t
        Ex: setResetPattern(r'RN')
        """
        self.resetPattern = string

    def setSetPattern(self, string):
        """setOutputPattern Method : Takes Regular expression for the
        recognizing the Set portnames\t
        Ex: setSetPattern(r'SN')
        """
        self.setPattern = string

    def setSelectPattern(self, string):
        """setSelectPattern Method : Takes Regular expression for the
        recognizing the Set portnames\t
        Ex: setSelectPattern(r'SN')
        """
        self.setPattern = string

    def extractNets(self):
        """extractNets: Extracts the nets in the file and stores in instances"""
        for eachline in self.lines:
            if not (eachline.startswith("module") or
                    eachline.startswith("input") or
                    eachline.startswith("wire") or
                    eachline.startswith("reg") or
                    eachline.startswith("assign") or
                    eachline.startswith("output") or
                    eachline.startswith("endmodule")):
                netobj = self.getPatternDetect("net", eachline)
                complexGates = ["MUX", "DFF", "HALFADDER", "FULLADDER"]
                if netobj:
                    nodes = netobj.group(3).split(",")
                    gateType = self.gatePatterns(netobj.group(1))
                    #print(gateType)
                    instance = netobj.group(2)
                    if gateType not in complexGates:
                        self.instances[instance] = Gate(gateType)
                    else:
                        if gateType == "DFF":
                            self.instances[instance] = DFF()
                        elif gateType == "MUX":
                            self.instances[instance] = Mux()
                        elif gateType == "HALFADDER":
                            self.instances[instance] = HalfAdder()
                        elif gateType == "FULLADDER":
                            self.instances[instance] = FullAdder()

                    for eachnode in nodes:
                        namedPort = self.getPatternDetect("namedport",
                                                          eachnode.strip(" "))
                        namedPortValue = self.getPatternDetect("namedportvalue",
                                                               eachnode.strip(" "))

                        if gateType not in complexGates:
                            if namedPort:
                                matchobj = re.match(self.outputPattern, namedPort.group(1), re.M | re.I)
                                if matchobj:
                                    self.instances[instance].addOutput(
                                        namedPortValue.group(1).strip(" "))
                                else:
                                    self.instances[instance].addInput(
                                        namedPortValue.group(1).strip(" "))
                            else:
                                if nodes.index(eachnode) == 0:
                                    self.instances[instance].addOutput(eachnode)
                                else:
                                    self.instances[instance].addInput(eachnode)
                        else:
                            if gateType == "DFF":
                                if namedPort:
                                    port = namedPort.group(1).upper()
                                    portvalue = namedPortValue.group(1)
                                    if port == "Q":
                                        self.instances[instance].addOutput(portvalue)
                                    elif port == "QN" or port == "QB":
                                        self.instances[instance].setQb(portvalue)
                                    elif port == "D":
                                        self.instances[instance].addInput(portvalue)
                                    elif re.match(self.clockPattern, port, re.M | re.I):
                                        self.instances[instance].setClk(portvalue)
                                    elif re.match(self.resetPattern, port, re.M | re.I):
                                        self.instances[instance].setRst(portvalue)
                                    elif re.match(self.setPattern, port, re.M | re.I):
                                        self.instances[instance].setSet(portvalue)
                                else:
                                    portvalue = eachnode.strip(" ")
                                    if nodes.index(eachnode) == 0:
                                        self.instances[instance].addOutput(portvalue)
                                    elif nodes.index(eachnode) == 1:
                                        self.instances[instance].setQb(portvalue)
                                    elif nodes.index(eachnode) == 2:
                                        self.instances[instance].addInput(portvalue)
                                    elif nodes.index(eachnode) == 3:
                                        self.instances[instance].setClk(portvalue)
                                    elif nodes.index(eachnode) == 4:
                                        self.instances[instance].setRst(portvalue)
                                    elif nodes.index(eachnode) == 5:
                                        self.instances[instance].setSet(portvalue)
                            elif gateType == "MUX":
                                if namedPort:
                                    port = namedPort.group(1).upper()
                                    portvalue = namedPortValue.group(1)
                                    matchobj = re.match(self.outputPattern,
                                                        namedPort.group(1), re.M | re.I)
                                    selobj = re.match(self.selectPattern,
                                                        namedPort.group(1), re.M | re.I)
                                    if matchobj:
                                        self.instances[instance].addOutput(portvalue)
                                    elif selobj:
                                        self.instances[instance].setSel(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)
                                else:
                                    if nodes.index(eachnode) == 0:
                                        self.instances[instance].addOutput(portvalue)
                                    elif nodes.index(eachnode) == 1:
                                        self.instances[instance].setSel(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)
                            elif gateType == "HALFADDER":
                                if namedPort:
                                    port = namedPort.group(1).upper()
                                    portvalue = namedPortValue.group(1)
                                    matchobj = re.match(self.outputPattern, 
                                                        namedPort.group(1), re.M | re.I)
                                    if port == "SUM" or port == "S":
                                        self.instances[instance].addOutput(portvalue)
                                    elif port == "COUT":
                                        self.instances[instance].setCout(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)
                                else:
                                    if nodes.index(eachnode) == 0:
                                        self.instances[instance].addOutput(portvalue)
                                    elif nodes.index(eachnode) == 1:
                                        self.instances[instance].setCout(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)
                            elif gateType == "FULLADDER":
                                if namedPort:
                                    port = namedPort.group(1).upper()
                                    portvalue = namedPortValue.group(1)
                                    matchobj = re.match(self.outputPattern,
                                                        namedPort.group(1), re.M | re.I)
                                    if port == "SUM" or port == "S":
                                        self.instances[instance].addOutput(portvalue)
                                    elif port == "COUT":
                                        self.instances[instance].setCout(portvalue)
                                    elif port == "CIN":
                                        self.instances[instance].setCin(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)
                                else:
                                    if nodes.index(eachnode) == 0:
                                        self.instances[instance].addOutput(portvalue)
                                    elif nodes.index(eachnode) == 1:
                                        self.instances[instance].setCout(portvalue)
                                    elif nodes.index(eachnode) == 2:
                                        self.instances[instance].setCin(portvalue)
                                    else:
                                        self.instances[instance].addInput(portvalue)

    def getInstances(self):
        """getInstance Method : Returns all the instances"""
        return self.instances
    
    def getAssign(self):
        """getAssign Method : Returns all the Assign statements"""
        return self.assign
    
    def getInputs(self):
        """getInputs Method : Returns all the input Nodes"""
        return self.inputs
    
    def getOutputs(self):
        """getOutputs Method : Returns all the output Nodes"""
        return self.outputs
