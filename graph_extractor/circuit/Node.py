# -*- coding: utf-8 -*-
"""
Created on Sat Feb 20 11:24:56 2021

@author: Juneeth Kumar Meka
"""


class Node:
    """Node Class:\t input : Name of the Node and type of the Node"""
    __author__ = "Juneethkumar Meka"
    __version__ = "1.0"
    __credits__ = ["Juneethkumar Meka"]
    __license__ = "Private Domain :  Requires Approval for the use from Juneethkumar Meka"
    __maintainer__ = "Juneethkumar Meka"
    __contact__ = "juneethkumarmeka@gmail.com"
    __status__ = "Development"
    __all__ = ["getNodeName", "changeNodeType", "getNodeType", "addFanin",
               "removeFanin", "getFanin", "addFanout", "removeFanout",
               "getFanout", "setNodeAsInput", "isNodeInput", "isNodeKeyinput",
               "setNodeAsKeyinput", "setNodeAsOutput", "isNodeOutput",
               "changeNodeLevel", "getNodeLevel", "resetNode"
               ]

    def __init__(self, name, nodeType):
        self.name = name
        self.nodeType = nodeType
        self.fanin = []
        self.fanout = []
        self.isinput = False
        self.iskeyinput = False
        self.isoutput = False
        self.level = 0
        self.levelflag = False

    def getNodeName(self):
        """getName Method : Gives the name of the Node"""
        return self.name

    def changeNodeType(self, nodeType):
        """changeNodeType Method : Changes the type of the object"""
        self.nodeType = nodeType

    def getNodeType(self):
        """getNodeType Method : Returns the type of the node"""
        return self.nodeType

    def addFanin(self, fanin):
        """addFanin Method : Adds Fanin to the object"""
        if type(fanin) != list:
            self.fanin.append(fanin)
        else:
            self.fanin = self.fanin + fanin

    def removeFanin(self, fanin):
        """removeFanin Method : Deletes the fanin value for the node"""
        if fanin in self.fanin:
            self.fanin.remove(fanin)

    def getFanin(self):
        """getFanin Method : Gives the Fanin of the node"""
        return self.fanin

    def addFanout(self, fanout):
        """addFanout Method : Adds Fanout to the object"""
        if type(fanout) != list:
            self.fanout.append(fanout)
        else:
            self.fanout = self.fanout + fanout

    def removeFanout(self, fanout):
        """removeFanout Method : Deletes the fanout value for the node"""
        if fanout in self.fanout:
            self.fanout.remove(fanout)

    def getFanout(self):
        """getFanout Method : Gives the Fanout of the node"""
        return self.fanout

    def setNodeAsInput(self):
        """setNodeAsInput Method : Sets the Node as Input"""
        self.isinput = True

    def isNodeInput(self):
        """isNodeInput Method : Return True if the node is input"""
        return self.isinput

    def setNodeAsKeyinput(self):
        """setNodeAsKeyinput Method : Sets the Node as keyinput"""
        self.iskeyinput = True
        self.isinput = True

    def isNodeKeyinput(self):
        """isNodeKeyInput Method : Return True if the node is keyinput"""
        return self.iskeyinput

    def setNodeAsOutput(self):
        """setNodeAsOutput Method : Sets the Node as output"""
        self.isoutput = True

    def isNodeOutput(self):
        """isNodeOutput Method : Return True if the node is Output"""
        return self.isoutput

    def changeNodeLevel(self, level):
        """changeGateLeval Method : Changes the level of the Node"""
        self.level = level

    def getNodeLevel(self):
        """getGateLevel Method : Return the level Node"""
        return self.level
    
    def setNodeLevelFlag(self):
        """setNodeLevelFlag : sets the flag for the Level """
        self.levelflag = True
    
    def getNodeLevelFlag(self):
        """getNodeLevelFlag : Return whether the node level flag is set or not"""
        return  self.levelflag
    
    def resetNodeLevelFlag(self):
        self.levelflag = False

    def resetNode(self):
        """resetNode Method :Reset node is input,
        keyinput type,output type,level"""
        self.isinput = False
        self.iskeyinput = False
        self.isoutput = False
        self.level = 0
