import Detector
import sub_circuit_extractor
import sys

n=len(sys.argv)
if n>=3:
    sub_circuit_extractor.Extractor(sys.argv[1],sys.argv[2])
else:
    print("Error in command line arguements")



