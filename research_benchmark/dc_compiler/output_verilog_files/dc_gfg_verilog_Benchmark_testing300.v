/////////////////////////////////////////////////////////////
// Created by: Synopsys DC Expert(TM) in wire load mode
// Version   : O-2018.06-SP1
// Date      : Tue Aug  2 20:27:14 2022
/////////////////////////////////////////////////////////////


module gfg_verilog_Benchmark_testing300 ( I51, I59, I67, I75, I83, I91, I3196, 
        I3207, I3249, I3291, I3330, I3344, I3372, I3397, I3411, I3453, I3478, 
        I3506, I3534, I3570, I3595, I3637, I3651, I3676, I3732, I3746, I3788, 
        I3813, I3841, I3869, I3905, I3972, I3986, I3997, I4022, I4078, I4092
 );
  input I51, I59, I67, I75, I83, I91;
  output I3196, I3207, I3249, I3291, I3330, I3344, I3372, I3397, I3411, I3453,
         I3478, I3506, I3534, I3570, I3595, I3637, I3651, I3676, I3732, I3746,
         I3788, I3813, I3841, I3869, I3905, I3972, I3986, I3997, I4022, I4078,
         I4092;
  wire   I3411, I3972, n12, n13, n14, n15;
  assign I3397 = I3411;
  assign I3986 = I3972;
  assign I4078 = 1'b0;
  assign I4092 = 1'b0;
  assign I3813 = 1'b0;
  assign I3478 = 1'b0;
  assign I3372 = 1'b0;
  assign I3453 = 1'b1;
  assign I3732 = 1'b1;
  assign I3997 = 1'b0;
  assign I4022 = 1'b0;
  assign I3249 = 1'b0;
  assign I3207 = 1'b0;
  assign I3196 = 1'b0;
  assign I3651 = 1'b0;
  assign I3637 = 1'b0;
  assign I3595 = 1'b0;
  assign I3746 = 1'b0;
  assign I3676 = 1'b0;
  assign I3570 = 1'b0;
  assign I3534 = 1'b0;
  assign I3905 = 1'b0;
  assign I3344 = 1'b0;
  assign I3330 = 1'b0;
  assign I3291 = 1'b0;

  INVX0 U15 ( .INP(n12), .ZN(I3972) );
  OR2X1 U16 ( .IN1(n13), .IN2(I67), .Q(I3869) );
  NOR2X0 U17 ( .IN1(I67), .IN2(n12), .QN(I3788) );
  NAND2X0 U18 ( .IN1(I91), .IN2(I51), .QN(n12) );
  NAND2X0 U19 ( .IN1(n13), .IN2(I67), .QN(I3506) );
  AND2X1 U20 ( .IN1(n13), .IN2(I59), .Q(I3411) );
  NAND2X0 U21 ( .IN1(n14), .IN2(n15), .QN(n13) );
  NAND2X0 U22 ( .IN1(I59), .IN2(I3841), .QN(n15) );
  INVX0 U23 ( .INP(I67), .ZN(I3841) );
  NAND2X0 U24 ( .IN1(I83), .IN2(I67), .QN(n14) );
endmodule

