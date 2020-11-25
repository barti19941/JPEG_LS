module gradient_merging_tb();

parameter integer L = 8;
parameter integer k = 2;

reg clk;
reg ce;
reg signed [2*k:0] q1_in;
reg signed [2*k:0] q2_in;
reg signed [2*k:0] q3_in;
reg [k-1:0] mode_in;
reg ritype_in;

wire [L:0] q_out;

gradient_merging g_m(clk, ce, q1_in, q2_in, q3_in, mode_in, ritype_in, q_out); 

always 
begin
    #5 
    clk <= ~clk;
end

initial
begin
    ce <= 1;
    clk <= 0;
    ritype_in <= 1;
    mode_in <= 0;  
    q1_in <= -3;
    q2_in <= -3;
    q3_in <= -3;         
    #10
    q1_in <= 3;
    q2_in <= 3;
    q3_in <= 3;             
    #10
    q1_in <= 0;
    q2_in <= -3;
    q3_in <= 3;             
    #10
    q1_in <= 0;
    q2_in <= 3;
    q3_in <= -3; 
    #10
    q1_in <= 0;
    q2_in <= 0;
    q3_in <= -3;    
    #10
    q1_in <= 0;
    q2_in <= 0;
    q3_in <= 3; 
    #10
    mode_in <= 2;  
    #10
    ritype_in <= 0;
end

endmodule
