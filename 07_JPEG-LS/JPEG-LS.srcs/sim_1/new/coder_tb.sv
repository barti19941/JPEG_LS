
module coder_tb();

parameter integer L = 8;
parameter integer k = 2;

reg clk;
reg ce;
reg [L-1:0] pixel_in;

wire [L-1:0] ix_out;
wire eoline_out;
wire [k-1:0] mode_out;
wire [k:0] runcnt_out;

coder c(clk, ce, pixel_in, ix_out, eoline_out, mode_out, runcnt_out); 


always 
begin
    #5 
    clk <= ~clk;    
end

always
begin
    #10
    pixel_in <= (pixel_in + 1);
end

initial
begin
    ce <= 1;
    clk <= 0;
    pixel_in <= 0;
end

endmodule
