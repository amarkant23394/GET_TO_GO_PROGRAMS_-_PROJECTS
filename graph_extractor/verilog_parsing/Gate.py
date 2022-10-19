# -*- coding: utf-8 -*-
"""
Created on Wed Feb 24 08:25:19 2021

@author: 15133
"""


class Gate:
    """Gate Class : Used for Storing the data of the Gates
    supported arguments : inputs,outputs
    Ex: Gate("AND")
    Ex: Gate("AND",inputs=[N1,N2])
    Ex: Gate("AND",inputs=[N1,N2], outputs=[N3,N4])
    """
    __author__ = "Juneethkumar Meka"
    __version__ = "1.0"
    __credits__ = ["Juneethkumar Meka"]
    __license__ = "Private Domain :  Requires Approval for the use from Juneethkumar Meka"
    __maintainer__ = "Juneethkumar Meka"
    __contact__ = "juneethkumarmeka@gmail.com"
    __status__ = "Development"
    __all__ = ["addInput", "getInputs", "getGateType", "addOutput",
               "getOutputs"]

    def __init__(self, gateType, **kwargs):
        self.gateType = gateType
        if "inputs" in kwargs:
            self.inputs = kwargs["inputs"]
        else:
            self.inputs = []

        if "outputs" in kwargs:
            self.outputs = kwargs["outputs"]
        else:
            self.outputs = []

    def addInput(self, inputs):
        """addInput Method: Adds input to the Gate.\t
        note: Accepts single input or the List of inputs\t
        """
        if type(inputs) != list:
            self.inputs.append(inputs)
        else:
            for eachinput in inputs:
                self.inputs.append(eachinput)

    def getGateType(self):
        """getGateType Method: Return the type of the Gate"""
        return self.gateType

    def getInputs(self):
        """getInputs Method: Return the inputs of the Gate"""
        return self.inputs

    def addOutput(self, outputs):
        """addOutput Method: Adds output to the Gate\t
        note: Accepts single output or the list of outputs\t"""
        if type(outputs) != list:
            self.outputs.append(outputs)
        else:
            for eachoutput in outputs:
                self.outputs.append(eachoutput)

    def getOutputs(self):
        """getOutputs Method: Returns the Outputs"""
        if type(self.outputs) == list:
            return self.outputs
        else:
            return [self.outputs]


class DFF(Gate):
    """DFF Class is the Child class of the Gate Class\t
    Arguments accpeted : Q,d,clk,rst,set,Qb\t
    Ex:DFF(d=D,clk=CLK, Q=Q, Qb=Qb, rst=Rst, set=Set)"""
    __version__ = "1.0"
    __author__ = "Juneethkumar Meka"
    __all__ = ["setClk", "getClk", "setQb", "getQb", "setRst", "getRst",
               "setSet", "getSet", "getInputs", "addInput", "getOutputs",
               "addOutput"]

    def __init__(self, **kwargs):
        if "Q" in kwargs:
            self.outputs = kwargs["Q"]
        else:
            self.outputs = []

        if "d" in kwargs:
            self.inputs = kwargs["d"]
        else:
            self.inputs = []

        if "clk" in kwargs:
            self.clk = kwargs["clk"]
        else:
            self.clk = None

        if "rst" in kwargs:
            self.rst = kwargs["rst"]
        else:
            self.rst = None

        if "set" in kwargs:
            self.set = kwargs["st"]
        else:
            self.set = None

        if "Qb" in kwargs:
            self.Qb = kwargs["Qb"]
        else:
            self.Qb = None

    def setClk(self, clk):
        """setClk Method: Adds Clock to the DFF"""
        self.clk = clk

    def getClk(self):
        """getClk Method: Returns Clock of the DFF"""
        return self.clk

    def setQb(self, Qb):
        """setQb Method: Adds Qb to the DFF"""
        self.Qb = Qb

    def getQb(self):
        """getQb Method: Returns Qb of the DFF"""
        return self.Qb

    def setRst(self, rst):
        """setRst Method: Adds Reset to the DFF"""
        self.rst = rst

    def getRst(self):
        """getRst Method: Returns Rst of the DFF"""
        return self.rst

    def setSet(self, st):
        """setSet Method: Adds Set to the DFF"""
        self.set = st

    def getSet(self):
        """getSet Method: Returns Set of the DFF"""
        return self.set

    def getGateType(self):
        """getGateType Method: Returns DFF"""
        return "DFF"


class Mux(Gate):
    """Mux is the Child class of the Gate Class.\t
    Accpeted arguments: inputs,output,sel.\t
    Ex: Mux(inputs=[A,B], sel=sel, output=out)
    """
    __version__ = "1.0"
    __author__ = "Juneethkumar Meka"
    __all__ = ["addInput", "getInputs", "getGateType", "addOutput",
               "getOutputs", "setSel", "getSel"]

    def __init__(self, **kwargs):
        if "inputs" in kwargs:
            self.inputs = kwargs["inputs"]
        else:
            self.inputs = []

        if "sel" in kwargs:
            self.sel = kwargs["sel"]
        else:
            self.sel = None

        if "output" in kwargs:
            self.outputs = kwargs["output"]
        else:
            self.outputs = []

    def setSel(self, sel):
        self.sel = sel

    def getSel(self):
        return self.sel

    def getGateType(self):
        return "MUX"


class HalfAdder(Gate):
    """HalfAdder is the child Class of the Gate Class\t
    Accepted arguments: inputs,sum,cout
    Ex: HalfAdder(inputs=[A,B], cout=cout, sum=sum)
    """
    __version__ = "1.0"
    __author__ = "Juneethkumar Meka"
    __all__ = ["addInput", "getInputs", "getGateType", "addOutput",
               "getOutputs", "setCout", "getCout"]

    def __init__(self, **kwargs):
        if "inputs" in kwargs:
            self.inputs = kwargs["inputs"]
        else:
            self.inputs = []

        if "sum" in kwargs:
            self.outputs = kwargs["sum"]
        else:
            self.outputs = []

        if "cout" in kwargs:
            self.cout = kwargs["cout"]
        else:
            self.cout = None

    def setCout(self, cout):
        self.cout = cout

    def getCout(self):
        return self.cout

    def getGateType(self):
        return "HALFADDER"


class FullAdder(Gate):
    """FullAdder is the child class of HalfAdder,
    which is the child Class of the Gate Class\t
    Accepted arguments: inputs,sum,cout,cin
    Ex: FullAdder(inputs=[A,B], cout=cout, sum=sum, cin=cin)
    """
    __version__ = "1.0"
    __author__ = "Juneethkumar Meka"
    __all__ = ["addInput", "getInputs", "getGateType", "addOutput",
               "getOutputs", "setCout", "getCout", "setCin", "getCin"]

    def __init__(self, **kwargs):
        if "inputs" in kwargs:
            self.inputs = kwargs["inputs"]
        else:
            self.inputs = []

        if "sum" in kwargs:
            self.outputs = kwargs["sum"]
        else:
            self.outputs = []

        if "cin" in kwargs:
            self.cin = kwargs["cin"]
        else:
            self.cin = None

        if "cout" in kwargs:
            self.cout = kwargs["cout"]
        else:
            self.cout = None

    def setCin(self, cin):
        self.cin = cin

    def getCin(self):
        return self.cin
    
    def setCout(self, cout):
        self.cout = cout

    def getCout(self):
        return self.cout

    def getGateType(self):
        return "FULLADDER"
