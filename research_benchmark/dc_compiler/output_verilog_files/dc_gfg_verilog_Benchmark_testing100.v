/////////////////////////////////////////////////////////////
// Created by: Synopsys DC Expert(TM) in wire load mode
// Version   : O-2018.06-SP1
// Date      : Tue Aug  2 20:24:02 2022
/////////////////////////////////////////////////////////////


module gfg_verilog_Benchmark_testing100 ( I51, I59, I67, I75, I83, I91, I1403, 
        I1417, I1445, I1470, I1526, I1540, I1565, I1621, I1635, I1660, I1702, 
        I1716 );
  input I51, I59, I67, I75, I83, I91;
  output I1403, I1417, I1445, I1470, I1526, I1540, I1565, I1621, I1635, I1660,
         I1702, I1716;
  wire   n13, n14, n15, n16, n17, n18, n19, n20;
  assign I1716 = 1'b0;
  assign I1702 = 1'b0;
  assign I1565 = 1'b0;
  assign I1403 = 1'b0;
  assign I1540 = 1'b0;
  assign I1470 = 1'b0;

  NAND2X0 U18 ( .IN1(n13), .IN2(n14), .QN(I1660) );
  INVX0 U19 ( .INP(I91), .ZN(n13) );
  NOR2X0 U20 ( .IN1(I75), .IN2(n15), .QN(I1635) );
  AND3X1 U21 ( .IN1(I67), .IN2(I51), .IN3(I75), .Q(I1621) );
  NAND2X0 U22 ( .IN1(n16), .IN2(n17), .QN(I1526) );
  OR2X1 U23 ( .IN1(n14), .IN2(I91), .Q(n17) );
  NAND2X0 U24 ( .IN1(I51), .IN2(n14), .QN(n16) );
  NAND2X0 U25 ( .IN1(I83), .IN2(I59), .QN(n14) );
  NOR2X0 U26 ( .IN1(n15), .IN2(n18), .QN(I1417) );
  INVX0 U27 ( .INP(I1445), .ZN(n15) );
  NAND2X0 U28 ( .IN1(n19), .IN2(n20), .QN(I1445) );
  NAND2X0 U29 ( .IN1(I75), .IN2(n18), .QN(n20) );
  INVX0 U30 ( .INP(I67), .ZN(n18) );
  NAND2X0 U31 ( .IN1(I67), .IN2(I51), .QN(n19) );
endmodule

