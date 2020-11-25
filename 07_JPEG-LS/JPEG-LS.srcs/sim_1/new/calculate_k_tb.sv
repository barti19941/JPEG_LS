module calculate_k_tb();

parameter integer L = 8;

reg clk;
reg ce;
reg [2*L-1:0] a_in;
reg [2*L-1:0] n_in;

wire [2*L-1:0] k_out;

calculate_k c_k(clk, ce, a_in, n_in, k_out);

always 
begin
    #5 
    clk <= ~clk;
end


initial
begin
    ce <= 1;
    clk <= 0;
    a_in <= 256;
    n_in <= 64;
    #10
    a_in <= 257;
    n_in <= 64;
    #10
    a_in <= 255;
    n_in <= 64;
    #10
    a_in <= 512;
    n_in <= 64;
    #10
    a_in <= 513;
    n_in <= 64;
    #10
    a_in <= 511;
    n_in <= 64;
end

endmodule
